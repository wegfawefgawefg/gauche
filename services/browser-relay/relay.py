#!/usr/bin/env python3
"""Browser frames: big-endian uint16 service port + unmodified signed datagram.

Only the existing roomd rendezvous and relay are reachable. They authenticate
all membership and traffic. No browser-supplied host/address or arbitrary ports.
"""
import asyncio
import collections
import struct
import time
from aiohttp import web, WSMsgType

ROOMD = '45.77.123.14'
PORTS = (8789, 8790)
ORIGINS = {'https://teeming.pages.dev', 'http://localhost:8787', 'http://127.0.0.1:8787'}
connections = collections.Counter()


class Replies(asyncio.DatagramProtocol):
    def __init__(self, queue):
        self.queue = queue

    def datagram_received(self, data, address):
        if address[0] != ROOMD or address[1] not in PORTS or len(data) > 1400:
            return
        if not self.queue.full():
            self.queue.put_nowait(struct.pack('!H', address[1]) + data)


async def relay(request):
    if request.headers.get('Origin') not in ORIGINS:
        raise web.HTTPForbidden()
    # Nginx overwrites this header; the bridge listens only on loopback.
    ip = request.headers.get('X-Real-IP', request.remote)
    if connections[ip] >= 24 or sum(connections.values()) >= 256:
        raise web.HTTPTooManyRequests()
    connections[ip] += 1
    udp = sender = None
    ws = web.WebSocketResponse(max_msg_size=1402, heartbeat=20, compress=False)
    try:
        await ws.prepare(request)
        queue = asyncio.Queue(maxsize=4096)
        udp, _ = await asyncio.get_running_loop().create_datagram_endpoint(
            lambda: Replies(queue), local_addr=('0.0.0.0', 0))

        async def send_replies():
            while True:
                await asyncio.wait_for(ws.send_bytes(await queue.get()), timeout=10)

        sender = asyncio.create_task(send_replies())
        tokens, last = 8000.0, time.monotonic()
        async for message in ws:
            if message.type != WSMsgType.BINARY:
                await ws.close(code=1003, message=b'Binary datagrams required')
                break
            data = message.data
            now = time.monotonic()
            tokens = min(8000, tokens + (now-last)*4000) - 1
            last = now
            if tokens < 0 or len(data) < 3:
                await ws.close(code=1008, message=b'Packet limit')
                break
            port = struct.unpack('!H', data[:2])[0]
            if port not in PORTS:
                await ws.close(code=1008, message=b'Invalid service')
                break
            udp.sendto(data[2:], (ROOMD, port))
    finally:
        if sender:
            sender.cancel()
            await asyncio.gather(sender, return_exceptions=True)
        if udp:
            udp.close()
        connections[ip] -= 1
        if not connections[ip]:
            del connections[ip]
    return ws


app = web.Application(client_max_size=1402)
app.router.add_get('/browser-relay', relay)
if __name__ == '__main__':
    web.run_app(app, host='127.0.0.1', port=8791, access_log=None)

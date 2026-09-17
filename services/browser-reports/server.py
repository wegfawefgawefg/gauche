#!/usr/bin/env python3
"""Private, bounded collection of browser technical failures. No read endpoint."""
import collections
import json
import logging
from logging.handlers import RotatingFileHandler
from pathlib import Path
import re
import time
from aiohttp import web

ORIGINS = {'https://teeming.pages.dev', 'http://localhost:8787', 'http://127.0.0.1:8787'}
KINDS = {'startup-failed','javascript-error','unhandled-rejection','desync-storm',
         'join-failed','disconnect','relay-disconnected'}
STATE_KEYS = {'seed','floor','tick','recoveries','host','rtt','confirmed','players',
              'snapshot','phase','ready','started','over','playing','renderSize','windowSize'}
EVENT_KEYS = {'tick','floor','ready','players','recoveries','rtt'}


def numeric_fields(value, allowed):
    if not isinstance(value, dict) or set(value)-allowed:
        raise ValueError('Invalid fields')
    for key, item in value.items():
        if key == 'seed':
            if not isinstance(item,str) or not re.fullmatch(r'[0-9]{1,20}',item):
                raise ValueError('Invalid seed')
        elif key in ('renderSize','windowSize'):
            if not isinstance(item,list) or len(item)!=2 or any(type(n)!=int or not 1<=n<=32768 for n in item):
                raise ValueError('Invalid dimensions')
        elif not isinstance(item,(int,float,bool)) or not -1<=item<=1e15:
            raise ValueError('Invalid number')
    return value


def validate(data):
    if not isinstance(data,dict) or set(data)!={'version','build','kind','message','browser','state','timing','events','network'}:
        raise ValueError('Invalid report')
    if data['version']!=1 or data['kind'] not in KINDS:
        raise ValueError('Invalid version or kind')
    if not isinstance(data['build'],str) or not re.fullmatch(r'[a-f0-9]{0,40}',data['build']):
        raise ValueError('Invalid build')
    for key,limit in [('message',1200),('browser',300)]:
        if not isinstance(data[key],str) or len(data[key])>limit:raise ValueError('Invalid text')
    numeric_fields(data['state'],STATE_KEYS)
    numeric_fields(data['timing'],{'callbackFps','renderFps','cap','workMs'})
    if not isinstance(data['events'],list) or len(data['events'])>20:raise ValueError('Invalid events')
    for event in data['events']:numeric_fields(event,EVENT_KEYS)
    if not isinstance(data['network'],list) or len(data['network'])>20:raise ValueError('Invalid network events')
    for line in data['network']:
        if not isinstance(line,str) or len(line)>1024 or not re.fullmatch(r'(?:[a-z_]+=(?:-?[0-9]+|[a-z_]+) ?)+',line):
            raise ValueError('Invalid network event')
    return data


def create_app(directory):
    logger=logging.Logger('reports')
    directory=Path(directory);directory.mkdir(parents=True,exist_ok=True)
    handler=RotatingFileHandler(directory/'reports.jsonl',maxBytes=2*1024*1024,backupCount=5)
    handler.setFormatter(logging.Formatter('%(message)s'));logger.addHandler(handler)
    peers={};global_times=collections.deque()

    async def receive(request):
        if request.headers.get('Origin') not in ORIGINS:raise web.HTTPForbidden()
        ip=request.headers.get('X-Real-IP',request.remote)
        now=time.monotonic()
        for key in list(peers):
            if peers[key][-1]<now-3600:del peers[key]
        times=peers.get(ip,collections.deque())
        while times and times[0]<now-3600:times.popleft()
        while global_times and global_times[0]<now-60:global_times.popleft()
        if len(times)>=20 or len(global_times)>=120 or (ip not in peers and len(peers)>=4096):
            raise web.HTTPTooManyRequests()
        times.append(now);peers[ip]=times;global_times.append(now)
        try:
            data=validate(await request.json())
        except (ValueError,TypeError,KeyError):raise web.HTTPBadRequest()
        # IPs are used only for in-memory limiting, never included in stored reports.
        logger.info(json.dumps({'received':int(time.time()),**data},separators=(',',':'),allow_nan=False))
        return web.Response(status=204)

    async def close(app):handler.close()
    app=web.Application(client_max_size=32768)
    app.router.add_post('/browser-reports',receive)
    app.on_cleanup.append(close)
    return app


if __name__=='__main__':
    web.run_app(create_app('/var/lib/teeming-reports'),host='127.0.0.1',port=8792,access_log=None)

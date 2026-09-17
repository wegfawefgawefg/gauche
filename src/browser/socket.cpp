#include "../net_socket.hpp"
#include <emscripten.h>
#include <array>
#include <cstdio>

namespace {
constexpr std::uint32_t server_address=45U | (77U<<8U) | (123U<<16U) | (14U<<24U);
EM_JS(int, open_socket, (), {
    Module.sockets ??= new Map();
    const id=(Module.socketSequence=(Module.socketSequence||0)+1);
    const socket=new WebSocket(Module.relayUrl || 'wss://45.77.123.14/browser-relay');
    socket.binaryType='arraybuffer';
    const state={socket,received:[],pending:[],failed:false};
    Module.sockets.set(id,state);
    socket.onopen=()=>{ for(const bytes of state.pending)socket.send(bytes);state.pending=[]; };
    socket.onmessage=event=>{
        const bytes=new Uint8Array(event.data);
        if(bytes.length<3 || bytes.length>1402 || state.received.length>=4096){state.failed=true;socket.close();return;}
        state.received.push(bytes);
    };
    socket.onerror=()=>{state.failed=true;};
    socket.onclose=()=>{state.failed=true;};
    return id;
});
EM_JS(void, close_socket, (int id), {
    const state=Module.sockets?.get(id);
    if(state){state.socket.close();Module.sockets.delete(id);}
});
EM_JS(int, send_socket, (int id,int port,const void* data,int size), {
    const state=Module.sockets?.get(id);
    if(!state||state.failed)return 0;
    const bytes=new Uint8Array(size+2);
    new DataView(bytes.buffer).setUint16(0,port);
    bytes.set(HEAPU8.subarray(data,data+size),2);
    if(state.socket.readyState===0){
        if(state.pending.length>=256)return 0;
        state.pending.push(bytes);
    }else{
        if(state.socket.bufferedAmount>2097152)return 0;
        state.socket.send(bytes);
    }
    return 1;
});
EM_JS(int, poll_socket, (int id,void* data), {
    const state=Module.sockets?.get(id);
    if(!state||state.failed)return -1;
    const bytes=state.received.shift();
    if(!bytes)return 0;
    HEAPU8.set(bytes,data);return bytes.length;
});
}
UdpSocket::~UdpSocket(){close();}
bool UdpSocket::open(std::uint16_t,std::string&){close();handle_=static_cast<std::uintptr_t>(open_socket());bound_port_=1;return true;}
void UdpSocket::close(){if(handle_)close_socket(static_cast<int>(handle_));handle_=0;bound_port_=0;}
bool UdpSocket::send(NetEndpoint to,std::span<const std::uint8_t> bytes,std::string& error){
    // Browsers only use the authenticated room protocols. Direct probes are ignored.
    if(to.port!=8789 && to.port!=8790)return true;
    if(to.address!=server_address||to.relayed||bytes.size()>1400){error="Invalid browser relay destination";return false;}
    if(!send_socket(static_cast<int>(handle_),to.port,bytes.data(),static_cast<int>(bytes.size()))){error="Browser relay disconnected or congested";return false;}
    return true;
}
bool UdpSocket::poll(Datagram& packet,std::string& error){
    std::array<std::uint8_t,1402> bytes{};
    const int count=poll_socket(static_cast<int>(handle_),bytes.data());
    if(count<0){error="Browser relay disconnected";return false;}
    if(count==0)return false;
    packet.from={server_address,static_cast<std::uint16_t>((bytes[0]<<8U)|bytes[1])};
    packet.bytes.assign(bytes.begin()+2,bytes.begin()+count);return true;
}
bool resolve_endpoint(const std::string& host,std::uint16_t port,NetEndpoint& endpoint,std::string& error){
    if(host=="45.77.123.14")endpoint={server_address,port};
    else if(host=="127.0.0.1"||host=="localhost")endpoint={0x0100007f,port};
    else{error="This browser build uses the Teeming room service";return false;}
    return true;
}
std::string endpoint_text(NetEndpoint endpoint){
    if(endpoint.relayed)return "relay:"+std::to_string(endpoint.port);
    return (endpoint.address==server_address ? "45.77.123.14:" : "127.0.0.1:")+std::to_string(endpoint.port);
}

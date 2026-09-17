#undef NDEBUG
#include "../src/net_session_internal.hpp"
#include "../src/net/party.hpp"
#include "../src/net/fragment.hpp"
#include "../src/items/tension_spring.hpp"
#include <cassert>
#include <memory>
#include <cstdio>
#include <algorithm>

namespace {
Game small_game() {
    Game g;
    g.started=true; g.rng=123; g.run.floor=2; g.run.phase=RunPhase::Playing;
    g.stage.width=48; g.stage.height=24; g.stage.tiles.assign(48*24,{TileKind::Grass});
    g.run.spawn={20,12};
    auto& member=player_state(g,0); member.online=true;
    member.controlled=spawn_entity(g,EntityKind::Player,g.run.spawn);
    get_entity(g,member.controlled)->owner=0;
    return g;
}

void party_check(int count) {
    auto host=std::make_unique<NetSession>(); std::string error;
    assert(host_game(*host,0,123,DeathPolicy::NextFloor,error));
    host->admission_limit=count+2; host->next_player_id=1001;
    begin_rollback(host->rollback,small_game());
    std::vector<std::unique_ptr<NetSession>> clients;
    std::uint64_t now=1000;
    const auto pump=[&](int rounds) {
        for(int r=0;r<rounds;++r) {
            now+=17;
            for(auto& c:clients) pump_network(*c,now);
            pump_network(*host,now);
            for(auto& c:clients) pump_network(*c,now);
        }
    };
    for(int i=1;i<count;++i) {
        auto c=std::make_unique<NetSession>();
        assert(join_game(*c,"127.0.0.1",host->socket.bound_port(),static_cast<std::uint64_t>(9000+i),error));
        clients.push_back(std::move(c)); pump(35);
    }
    pump(80);
    assert(host->rollback.game.players.size()==static_cast<std::size_t>(count));
    for(const auto& c:clients) {
        if(!c->ready) std::fprintf(stderr,"join failure %s\n",c->status.c_str());
        assert(c->ready && c->local_owner>=1001);
        assert(game_hash(c->rollback.game)==game_hash(host->rollback.game));
    }
    host->admission_limit=count;
    auto excess=std::make_unique<NetSession>();
    assert(join_game(*excess,"127.0.0.1",host->socket.bound_port(),9888777,error));
    for(int retry=0;retry<6;++retry){pump_network(*excess,now);pump(1);pump_network(*excess,now);}
    assert(!excess->ready && excess->status.find("full")!=std::string::npos);
    leave_network_game(*excess);host->admission_limit=count+2;
    host->rollback.max_history=16;
    for(auto& c:clients) c->rollback.max_history=16;
    const auto sent=host->diagnostics.sent_bytes;
    for(int tick=0;tick<6;++tick) {
        for(auto& c:clients) client_step(*c,{});
        pump(1); host_step(*host,{}); pump(3);
    }
    for(const auto& c:clients) assert(!c->rollback.needs_snapshot && game_hash(c->rollback.game)==game_hash(host->rollback.game));
    std::printf("%d players: %.0f host bytes/tick (all recipients), %zu participants\n",count,
        static_cast<double>(host->diagnostics.sent_bytes-sent)/6,host->rollback.game.players.size());
    // A late input must repair a sparse ID across every client.
    Input late; late.move={1,0};
    auto& last=*clients.back();
    const auto revision=host->timeline_revision;
    PacketWriter late_packet=begin_packet(WireKind::Input);
    late_packet.u64(last.local_identity);
    late_packet.u32(last.timeline_revision);
    late_packet.u64(last.rollback.confirmed_through);
    late_packet.u8(1); late_packet.u64(3); late_packet.input(late);
    send_wire(last,last.host_endpoint,late_packet);
    pump(25);
    assert(host->timeline_revision>revision);
    for(const auto& c:clients) {
        assert(!c->rollback.needs_snapshot);
        assert(game_hash(c->rollback.game)==game_hash(host->rollback.game));
    }
    // Ready state is a list, including IDs that do not fit a byte or bitmask.
    for(auto& c:clients) {c->party_ready=true;send_party_state(*c);}
    pump(35); assert(host->ready_players.size()==static_cast<std::size_t>(count));
    for(const auto& c:clients) assert(c->ready_players==host->ready_players);
    const auto id=last.local_owner; const auto identity=last.local_identity;
    const auto old=player_state(host->rollback.game,id).controlled;
    leave_network_game(last); pump(4);
    assert(!host->peers.contains(id) && !get_entity(host->rollback.game,old));
    auto newcomer=std::make_unique<NetSession>();
    assert(join_game(*newcomer,"127.0.0.1",host->socket.bound_port(),9999888,error));
    clients.push_back(std::move(newcomer)); pump(80);
    assert(clients.back()->ready && clients.back()->local_owner>id);
    leave_network_game(*clients.back()); pump(4); clients.pop_back();
    assert(join_game(last,"127.0.0.1",host->socket.bound_port(),identity,error)); pump(80);
    assert(last.ready && last.local_owner==id);
    assert(player_state(host->rollback.game,id).controlled!=old);
    // Floor rewards wait on every living participant, not only the first four.
    auto& g=host->rollback.game;
    get_entity(g,player_state(g,id).controlled)->health=0;
    finish_floor(g);
    for(auto& [owner,member]:g.players) for(auto& offer:member.offers)
        offer={RewardKind::Health,ItemKind::None,ArtifactKind::None,5};
    std::vector<PlayerId> living;
    for(const auto& [owner,member]:g.players) if(owner!=id) living.push_back(owner);
    for(std::size_t i=0;i+1<living.size();++i) choose_reward(g,living[i],0);
    assert(g.run.phase==RunPhase::Reward);
    choose_reward(g,living.back(),0); assert(g.run.phase==RunPhase::Shop);
    for(const auto owner:living) player_state(g,owner).shop_ready=true;
    advance_run(g);
    assert(g.run.floor==3 && get_entity(g,player_state(g,id).controlled)->health>0);
    for(const auto& [owner,member]:g.players) {
        const auto* p=get_entity(g,member.controlled); assert(p && p->owner==owner);
        const Tile* t=g.stage.at(p->cell); assert(t && walkable(*t));
        for(const auto& [other,m]:g.players) if(other!=owner) assert(get_entity(g,m.controlled)->cell!=p->cell);
    }
    std::puts("  joins, hash sync, correction, readiness, reconnect, rewards and revival passed");
}

void fragments() {
    auto sender=std::make_unique<NetSession>(), receiver=std::make_unique<NetSession>();
    std::string error; assert(sender->socket.open(0,error)); assert(receiver->socket.open(0,error));
    NetEndpoint to; assert(resolve_endpoint("127.0.0.1",receiver->socket.bound_port(),to,error));
    receiver->role=NetRole::Client;
    assert(resolve_endpoint("127.0.0.1",sender->socket.bound_port(),receiver->host_endpoint,error));
    CanonicalFrame frame; frame.tick=77; frame.hash=123;
    for(int i=0;i<128;++i) frame.inputs[i*10000]={};
    PacketWriter packet=begin_packet(WireKind::Canonical); write_frame(packet,frame);
    send_wire(*sender,to,packet);
    std::vector<Datagram> parts; Datagram d;
    while(receiver->socket.poll(d,error)) {assert(d.bytes.size()<=game_datagram_bytes); parts.push_back(std::move(d));}
    assert(parts.size()>1);
    std::reverse(parts.begin(),parts.end());
    // One missing part cannot be delivered; duplicate parts do not complete early.
    for(std::size_t i=0;i+1<parts.size();++i) for(int copy=0;copy<2;++copy) {
        auto part=parts[i]; PacketReader reader{part.bytes}; WireKind kind; assert(read_packet_header(reader,kind));
        assert(!receive_fragment(*receiver,part,reader));
    }
    auto part=parts.back(); PacketReader reader{part.bytes}; WireKind kind; assert(read_packet_header(reader,kind));
    assert(receive_fragment(*receiver,part,reader)); assert(part.bytes==packet.bytes);
    reader=PacketReader{part.bytes}; assert(read_packet_header(reader,kind));
    const auto decoded=read_frame(reader); assert(reader.finished() && decoded.inputs==frame.inputs);
    PacketWriter bad; bad.u64(0);bad.u64(0);bad.u32(UINT32_MAX);
    PacketReader invalid{bad.bytes}; read_frame(invalid); assert(!invalid.okay);
    std::puts("128-input message: bounded datagrams, reorder/duplicate/missing fragment and malformed count passed");
}

void control_identity() {
    auto g=std::make_unique<Game>(small_game());
    const auto original=player_state(*g,0).controlled;
    const auto wolf=spawn_entity(*g,EntityKind::Wolf,{10,10});
    player_state(*g,123456).coins=87;
    assert(bind_player_control(*g,123456,wolf));
    assert(!bind_player_control(*g,0,wolf));
    assert(player_state(*g,0).controlled==original && player_state(*g,123456).coins==87);
    const auto spring_player=spawn_entity(*g,EntityKind::Player,{15,15});
    assert(bind_player_control(*g,123456,spring_player));
    get_entity(*g,spring_player)->inventory.slots[0]=make_item(ItemKind::TensionSpring);
    assert(place_tension_spring(*g,spring_player.slot,{1,0}));
    assert(g->stage.prop_owners.at(15*g->stage.width+15)==123456);
    auto decoded=std::make_unique<Game>(); std::string error;
    assert(decode_game(encode_game(*g),*decoded,error));
    assert(game_hash(*g)==game_hash(*decoded));
    assert(decoded->players.size()==2 && player_state(*decoded,123456).controlled==spring_player);
}

void full_world_rejoin() {
    auto host=std::make_unique<NetSession>(), client=std::make_unique<NetSession>();
    std::string error;
    assert(host_game(*host,0,123,DeathPolicy::NextFloor,error));
    begin_rollback(host->rollback,small_game());
    auto& game=host->rollback.game;
    while(spawn_entity(game,EntityKind::Wolf,{2,2}).slot>=0) {}
    host->departed[998877].id=12345;
    const auto before=game_hash(game);
    assert(join_game(*client,"127.0.0.1",host->socket.bound_port(),998877,error));
    for(std::uint64_t now=1000;now<1100;now+=17) {
        pump_network(*client,now); pump_network(*host,now); pump_network(*client,now);
    }
    assert(!client->ready && client->status.find("full")!=std::string::npos);
    assert(!has_player(game,12345) && host->peers.empty());
    assert(game_hash(game)==before && host->departed.contains(998877));
}
}
void player_network_tests() {fragments();control_identity();full_world_rejoin();for(int count:{4,8,16,24})party_check(count);}

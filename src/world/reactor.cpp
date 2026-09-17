#include "reactor.hpp"
#include "../surfaces/interaction.hpp"
#include "../items/emergency_foam.hpp"

#include <algorithm>
#include <functional>
#include <queue>
#include <utility>

namespace {
constexpr Cell sides[]{{1,0},{-1,0},{0,1},{0,-1}};
bool sealed(const Tile& tile) {return tile.kind==TileKind::Wall && tile.break_rule==BreakRule::Unbreakable;}
Cell event_cell(const Game& game,const ReactorEvent& event) {
    return {static_cast<int>(event.tile)%game.stage.width,static_cast<int>(event.tile)/game.stage.width};
}
void schedule_front(Game& game,Cell origin) {
    // One finite Dijkstra pass at shutdown, with (arrival,index) tie-breaking.
    // Conductive partitions carry the front faster than open floors. A mined
    // shortcut still helps the party and changes the front's path at activation.
    std::vector<int> arrival(game.stage.tiles.size(),reactor_deadline);
    using Entry=std::pair<int,int>;
    std::priority_queue<Entry,std::vector<Entry>,std::greater<Entry>> queue;
    for (Cell cell:{origin,origin+Cell{-6,-7},origin+Cell{6,-7},origin+Cell{-6,4},origin+Cell{6,4}}) {
        if (!game.stage.in_bounds(cell)) continue;
        const int index=cell.y*game.stage.width+cell.x;
        arrival[static_cast<std::size_t>(index)]=240;queue.push({240,index});
    }
    while (!queue.empty()) {
        const auto [time,index]=queue.top();queue.pop();
        if (arrival[static_cast<std::size_t>(index)]!=time) continue;
        const Cell cell{index%game.stage.width,index/game.stage.width};
        for (Cell side:sides) {
            const Cell next=cell+side;const Tile* tile=game.stage.at(next);
            if (!tile || sealed(*tile)) continue;
            const auto at=static_cast<std::size_t>(next.y*game.stage.width+next.x);
            const int due=time+(tile->kind==TileKind::Wall ? 18 : 100);
            if (due>=reactor_deadline || due>=arrival[at]) continue;
            arrival[at]=due;queue.push({due,static_cast<int>(at)});
        }
    }
    game.reactor_front.clear();
    for (std::size_t index=0;index<arrival.size();++index)
        if (arrival[index]<reactor_deadline)
            game.reactor_front.push_back({static_cast<std::uint32_t>(index),static_cast<std::uint16_t>(arrival[index]),0});
    std::sort(game.reactor_front.begin(),game.reactor_front.end(),[](const ReactorEvent& a,const ReactorEvent& b) {
        return a.due==b.due ? a.tile<b.tile : a.due<b.due;
    });
}
}

const Entity* reactor_core(const Game& game) {
    if (game.run.layout!=FloorLayout::LastShift) return nullptr;
    for (const Entity& e:game.entities)
        if (e.kind==EntityKind::Switch && e.sprite==Sprite::ReactorCore) return &e;
    return nullptr;
}

bool activate_reactor(Game& game,Entity& core) {
    if (game.run.phase!=RunPhase::Playing || game.run.layout!=FloorLayout::LastShift || core.sprite!=Sprite::ReactorCore || core.fixture_open) return false;
    // The core reuses Switch: counters a/b are warning/release cursors; c is
    // elapsed fixed ticks. fixture_open latches the irreversible shutdown.
    core.fixture_open=true;core.counter_a=core.counter_b=core.counter_c=0;
    schedule_front(game,core.cell);game.run.has_key=true;
    for (Entity& e:game.entities) {
        if (e.kind==EntityKind::Door) {e.fixture_open=true;e.impassable=e.hard_blocker=false;}
        if (e.kind==EntityKind::Exit) e.light={8,1300,{105,255,131}};
    }
    emit_sound(game,SoundId::ReactorShutdown,core.cell,false);
    return true;
}

void step_reactor(Game& game) {
    const Entity* found=reactor_core(game);
    if (!found || !found->fixture_open || game.run.phase!=RunPhase::Playing) return;
    Entity& core=game.entities[static_cast<std::size_t>(found-game.entities.data())];
    const int elapsed=++core.counter_c;
    const int remaining=reactor_deadline-elapsed;
    core.light={8,remaining<600 && elapsed%60<30 ? 1700 : 900,{165,214,76}};
    if (elapsed==1 || (remaining>0 && remaining%(remaining<=600 ? 60 : 300)==0))
        emit_sound(game,SoundId::ReactorAlarm,core.cell,false);
    int budget=reactor_budget;
    while (budget-- && core.counter_a<static_cast<int>(game.reactor_front.size())) {
        ReactorEvent& event=game.reactor_front[static_cast<std::size_t>(core.counter_a)];
        if (event.due>elapsed+reactor_warning) break;
        event.warned_at=static_cast<std::uint16_t>(elapsed);++core.counter_a;
    }
    budget=reactor_budget;
    bool ruptured=false;
    while (budget-- && core.counter_b<core.counter_a) {
        const ReactorEvent& event=game.reactor_front[static_cast<std::size_t>(core.counter_b)];
        // Backlogs may delay ignition, never shorten the on-screen warning.
        if (event.due>elapsed || event.warned_at+reactor_warning>elapsed) break;
        ++core.counter_b;
        Tile& tile=*game.stage.at(event_cell(game,event));
        if (sealed(tile) || surface_wet(tile) || tile.kind==TileKind::Ice || live_foam(tile.prop)) continue;
        tile.surface.reactor_fire=true;
        tile.surface.fire_ticks=static_cast<std::uint16_t>(std::max(1,remaining+1));
        ruptured=true;
    }
    if (ruptured && elapsed%30==0) emit_sound(game,SoundId::ReactorRupture,core.cell);
    if (remaining>0) return;
    // Terminal event ignores the ordinary Entrance respawn policy. Paused fixed
    // steps and completed floors never reach here; disconnected players cannot
    // return after detonation to revive a finished run.
    for (Handle handle : controlled_entities(game))
        if (const Entity* player=get_entity(game,handle);player && player->health>0)
            crush_entity(game,handle.slot,core.cell);
    game.game_over=true;
    emit_sound(game,SoundId::ReactorBlast,core.cell,false);
}

bool exposed_reactor_cell(const Stage& stage,Cell cell) {
    if (stage.at_or_border(cell).kind!=TileKind::Wall) return true;
    for (Cell side:sides)
        if (const Tile* tile=stage.at(cell+side);tile && tile->kind!=TileKind::Wall) return true;
    return false;
}

bool valid_reactor(const Game& game) {
    const Entity* core=reactor_core(game);
    if (!core) return game.run.layout!=FloorLayout::LastShift && game.reactor_front.empty();
    if (!core->fixture_open) return game.reactor_front.empty() && core->counter_a==0 && core->counter_b==0 && core->counter_c==0;
    if (core->counter_c<0 || core->counter_c>reactor_deadline || core->counter_b<0 ||
        core->counter_b>core->counter_a || core->counter_a>static_cast<int>(game.reactor_front.size()) ||
        game.reactor_front.empty() || !game.run.has_key) return false;
    std::vector<bool> seen(game.stage.tiles.size(),false);
    std::pair<int,std::uint32_t> previous{};
    for (std::size_t i=0;i<game.reactor_front.size();++i) {
        const ReactorEvent& e=game.reactor_front[i];
        const std::pair<int,std::uint32_t> order{e.due,e.tile};
        if (e.tile>=seen.size() || seen[e.tile] || e.due<240 || e.due>=reactor_deadline || order<previous) return false;
        const bool warned=i<static_cast<std::size_t>(core->counter_a);
        if (warned ? e.warned_at==0 || e.warned_at>core->counter_c || e.due>e.warned_at+reactor_warning : e.warned_at!=0) return false;
        if (i<static_cast<std::size_t>(core->counter_b) && (e.due>core->counter_c || e.warned_at+reactor_warning>core->counter_c)) return false;
        seen[e.tile]=true;previous=order;
    }
    return true;
}

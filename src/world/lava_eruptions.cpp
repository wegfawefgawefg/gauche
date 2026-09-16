#include "lava_eruptions.hpp"
#include "../items/emergency_foam.hpp"
#include "../items/fire.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include "../entities/steam_leech.hpp"
#include <algorithm>
#include <optional>

namespace {
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool quest_near(const Game& game,Cell cell,int radius) {
    for (const Entity& e:game.entities) {
        if (distance(e.cell,cell)>radius) continue;
        switch (e.kind) {
        case EntityKind::Key: case EntityKind::Switch: case EntityKind::Door:
        case EntityKind::Exit: case EntityKind::EncounterGate: return true;
        default:break;
        }
    }
    return distance(cell,game.run.spawn)<8;
}
std::optional<Cell> shore_target(const Game& game,Cell source,int first) {
    for (int turn=0;turn<4;++turn) {
        const Cell dir=directions[(first+turn)%4];
        for (int n=1;n<=3;++n) {
            const Cell cell=source+Cell{dir.x*n,dir.y*n};
            const Tile* tile=game.stage.at(cell);
            if (!tile || tile->kind==TileKind::Wall) break;
            if (tile->kind==TileKind::Lava) continue;
            // Water and gaps are natural firebreaks, not targets to shoot over.
            if (walkable(tile->kind) && !surface_wet(*tile) && !live_foam(tile->prop) &&
                !quest_near(game,cell,2)) return cell;
            break;
        }
    }
    return {};
}
void cool(Game& game,LavaVent& vent) {
    vent.phase=LavaPhase::Cooling;vent.target=vent.source;
    vent.ticks=static_cast<std::uint16_t>(360+random_u32(game)%361);
}
void impact(Game& game,const LavaVent& vent) {
    Tile* tile=game.stage.at(vent.target);
    if (!tile) return;
    if (surface_wet(*tile) || live_foam(tile->prop) || leech_drains_cell(game,vent.target)) {
        emit_sound(game,SoundId::WaterDouse,vent.target);return;
    }
    emit_sound(game,SoundId::LavaSizzle,vent.target);
    // One marked tile receives the physical hit. Fire only catches actual fuel;
    // bare concrete is never turned into a permanent new lava source.
    hit_prop(game,vent.target,12,vent.source);
    ignite_surface(game,vent.target);
    for (int slot=0;slot<max_entities;++slot) {
        Entity& e=game.entities[static_cast<std::size_t>(slot)];
        if (e.cell!=vent.target || e.health<=0 || e.toss.ticks>0 || e.move_interval<=0) continue;
        if (e.kind==EntityKind::SlagSnail || e.kind==EntityKind::WalkingKiln ||
            e.kind==EntityKind::Ember || e.kind==EntityKind::SteamLeech || e.kind==EntityKind::FurnaceMoth) continue;
        damage_entity(game,slot,14,vent.source,false);
        if (e.health>0) ignite_struck_actor(game,slot);
    }
    // At most one additional eligible neighbour is tested per impact. Existing
    // fire propagation owns any subsequent spread; no recursive eruption work.
    if (random_u32(game)%4==0) {
        const Cell neighbour=vent.target+directions[random_u32(game)%4];
        if (!quest_near(game,neighbour,1)) ignite_surface(game,neighbour);
    }
}
}

void place_lava_vents(Game& game) {
    if (!industrial_floor(game.run.floor)) return;
    std::vector<Cell> candidates;
    for (int y=1;y<game.stage.height-1;++y) for (int x=1;x<game.stage.width-1;++x) {
        const Cell cell{x,y};
        if (game.stage.at_or_border(cell).kind==TileKind::Lava && shore_target(game,cell,0)) candidates.push_back(cell);
    }
    const int budget=5+biome_stage(game.run.floor);
    while (!candidates.empty() && static_cast<int>(game.lava_vents.size())<budget) {
        const auto index=random_u32(game)%candidates.size();const Cell source=candidates[index];
        candidates[index]=candidates.back();candidates.pop_back();
        bool close=false;for (const auto& vent:game.lava_vents) if (distance(vent.source,source)<10) close=true;
        if (close) continue;
        LavaVent vent;vent.source=source;cool(game,vent);game.lava_vents.push_back(vent);
    }
}

void step_lava_eruptions(Game& game) {
    int active=0;for (const auto& vent:game.lava_vents) active+=vent.phase!=LavaPhase::Cooling;
    // Only one new warning per half-second, at most three warnings/flights alive.
    bool start_allowed=game.tick%30==0;
    for (auto& vent:game.lava_vents) {
        const Tile& source=game.stage.at_or_border(vent.source);
        if (vent.phase!=LavaPhase::Airborne && source.kind!=TileKind::Lava) continue;
        if (vent.phase!=LavaPhase::Airborne && (source.surface.still_ticks>0 || leech_drains_cell(game,vent.source))) continue;
        if (vent.ticks>0) --vent.ticks;
        if (vent.ticks>0) continue;
        if (vent.phase==LavaPhase::Cooling) {
            if (!start_allowed || active>=lava_active_budget) continue;
            const auto target=shore_target(game,vent.source,static_cast<int>(random_u32(game)%4));
            if (!target) {cool(game,vent);continue;}
            vent.target=*target;vent.phase=LavaPhase::Swelling;vent.ticks=lava_warning_ticks;
            ++active;start_allowed=false;emit_sound(game,SoundId::LavaRumble,vent.source);
        } else if (vent.phase==LavaPhase::Swelling) {
            vent.phase=LavaPhase::Airborne;vent.ticks=lava_flight_ticks;
            emit_sound(game,SoundId::LavaPop,vent.source);
        } else {impact(game,vent);cool(game,vent);--active;}
    }
    // Cooling the source before launch cancels the tell. A launched glob keeps
    // its committed flight; quench the landing spot or leave its warning instead.
    std::erase_if(game.lava_vents,[&](const LavaVent& vent) {
        return vent.phase!=LavaPhase::Airborne && game.stage.at_or_border(vent.source).kind!=TileKind::Lava;
    });
}

bool valid_lava_vents(const Game& game) {
    if (game.lava_vents.size()>max_lava_vents) return false;
    int active=0;
    for (std::size_t i=0;i<game.lava_vents.size();++i) {
        const auto& vent=game.lava_vents[i];
        if (!game.stage.in_bounds(vent.source) || !game.stage.in_bounds(vent.target) || vent.phase>LavaPhase::Airborne) return false;
        for (std::size_t j=0;j<i;++j) if (vent.source==game.lava_vents[j].source) return false;
        if (vent.phase==LavaPhase::Cooling) {if (vent.ticks>720 || vent.target!=vent.source) return false;continue;}
        ++active;const Cell d=vent.target-vent.source;
        if (distance({},d)<1 || distance({},d)>3 || (d.x!=0 && d.y!=0) || vent.ticks==0 ||
            vent.ticks>(vent.phase==LavaPhase::Swelling ? lava_warning_ticks : lava_flight_ticks)) return false;
    }
    return active<=lava_active_budget;
}

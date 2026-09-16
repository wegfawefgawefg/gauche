#include "fissures.hpp"
#include "water.hpp"
#include "../entities/steam_leech.hpp"
#include "../items/emergency_foam.hpp"
#include "../items/fire.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

namespace {
bool emitting(const Fissure& f) {
    return f.phase==FissurePhase::Pressure || f.phase==FissurePhase::Release;
}
bool open(const Game& game,Cell cell) {
    const auto& tile=game.stage.at_or_border(cell);
    return (tile.kind==TileKind::Ruin || tile.kind==TileKind::Empty || tile.kind==TileKind::Grass) &&
        !prop_blocks(tile.prop);
}
void cool(Fissure& f,int ticks) {
    if (f.phase!=FissurePhase::Cooling) f.ticks=0;
    f.phase=FissurePhase::Cooling;f.heat=0;
    f.ticks=static_cast<std::uint16_t>(std::max(ticks,static_cast<int>(f.ticks)));
}
}

Cell fissure_cell(const Fissure& f,int offset) {return f.center+Cell{f.axis.x*offset,f.axis.y*offset};}
bool fissure_contains(const Fissure& f,Cell cell) {
    for (int n=-1;n<=1;++n) if (fissure_cell(f,n)==cell) return true;
    return false;
}
bool fissure_hot(const Game& game,Cell cell) {
    for (const auto& f:game.fissures)
        if (emitting(f) && f.heat>0 && fissure_contains(f,cell) && open(game,cell)) return true;
    return false;
}
bool fissure_flame(const Game& game,Cell cell) {
    for (const auto& f:game.fissures)
        if (f.kind==FissureKind::Lava && f.phase==FissurePhase::Release &&
            fissure_contains(f,cell) && open(game,cell)) return true;
    return false;
}
bool cool_fissure(Game& game,Cell cell) {
    for (auto& f:game.fissures) if (fissure_contains(f,cell)) {
        const bool changed=f.phase!=FissurePhase::Cooling;cool(f,360);return changed;
    }
    return false;
}
int extract_fissure_heat(Game& game,Cell cell,int limit) {
    if (limit<=0) return 0;
    for (auto& f:game.fissures) if (fissure_contains(f,cell) && emitting(f) && open(game,cell)) {
        const int amount=std::min(limit,static_cast<int>(f.heat));
        f.heat=static_cast<std::uint16_t>(f.heat-amount);
        // A deep enough draw collapses pressure; the residual warmth dissipates.
        if (f.heat<120) {cool(f,360);emit_sound(game,SoundId::FissureCool,cell);}
        return amount;
    }
    return 0;
}
void contact_fissure(Game& game,int slot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (!wading_actor(actor)) return;
    for (const auto& f:game.fissures) {
        if (f.phase!=FissurePhase::Release || !fissure_contains(f,actor.cell) || !open(game,actor.cell)) continue;
        const auto& tile=game.stage.at_or_border(actor.cell);
        if (surface_wet(tile) || live_foam(tile.prop) || leech_drains_cell(game,actor.cell)) continue;
        const bool lava=f.kind==FissureKind::Lava;
        if (lava && (actor.kind==EntityKind::SlagSnail || actor.kind==EntityKind::WalkingKiln ||
            actor.kind==EntityKind::SteamLeech || actor.kind==EntityKind::Ember)) continue;
        damage_entity(game,slot,lava ? 12 : 8,f.center,false);
        if (lava && actor.health>0) ignite_struck_actor(game,slot);
        emit_sound(game,lava ? SoundId::LavaSizzle : SoundId::BoilerScald,actor.cell);
        return;
    }
}
void step_fissures(Game& game) {
    for (auto& f:game.fissures) {
        bool damp=false,still=false,clear=false;
        for (int n=-1;n<=1;++n) {
            const Cell cell=fissure_cell(f,n);const auto& tile=game.stage.at_or_border(cell);
            damp|=surface_wet(tile) || live_foam(tile.prop) || leech_drains_cell(game,cell);
            still|=tile.surface.still_ticks>0;clear|=open(game,cell);
        }
        if (damp || !clear) {
            if (emitting(f)) emit_sound(game,SoundId::FissureCool,f.center);
            cool(f,180);
        }
        if (still) continue;
        if (f.ticks>0) --f.ticks;
        if (f.ticks==0) {
            switch (f.phase) {
            case FissurePhase::Idle:
                f.phase=FissurePhase::Pressure;f.ticks=fissure_warning_ticks;f.heat=360;
                emit_sound(game,SoundId::FissurePressure,f.center);break;
            case FissurePhase::Pressure:
                f.phase=FissurePhase::Release;f.ticks=fissure_release_ticks;
                emit_sound(game,f.kind==FissureKind::Steam ? SoundId::FissureSteam : SoundId::FissureLava,f.center);
                if (f.kind==FissureKind::Lava) for (int n=-1;n<=1;++n) {
                    const Cell cell=fissure_cell(f,n);if (open(game,cell)) ignite_surface(game,cell);
                }
                break;
            case FissurePhase::Release:cool(f,180);break;
            case FissurePhase::Cooling:
                f.phase=FissurePhase::Idle;f.ticks=static_cast<std::uint16_t>(240+random_u32(game)%241);break;
            }
        }
        if (f.phase==FissurePhase::Release && f.ticks%30==0) {
            for (int slot=0;slot<max_entities;++slot)
                if (fissure_contains(f,game.entities[static_cast<std::size_t>(slot)].cell)) contact_fissure(game,slot);
        }
    }
}
bool valid_fissures(const Game& game) {
    if (game.fissures.size()>max_fissures) return false;
    for (std::size_t i=0;i<game.fissures.size();++i) {
        const auto& f=game.fissures[i];
        if ((f.axis!=Cell{1,0} && f.axis!=Cell{0,1}) || f.kind>FissureKind::Lava ||
            f.phase>FissurePhase::Cooling || f.ticks==0 || f.heat>360) return false;
        const int max_ticks=f.phase==FissurePhase::Pressure ? fissure_warning_ticks :
            f.phase==FissurePhase::Release ? fissure_release_ticks : f.phase==FissurePhase::Cooling ? 360 : 480;
        if (f.ticks>max_ticks || (emitting(f) ? f.heat<120 : f.heat!=0)) return false;
        for (int n=-1;n<=1;++n) {
            const Cell cell=fissure_cell(f,n);if (!game.stage.in_bounds(cell)) return false;
            for (std::size_t j=0;j<i;++j) if (fissure_contains(game.fissures[j],cell)) return false;
        }
    }
    return true;
}

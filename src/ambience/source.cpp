#include "../props/conveyor.hpp"
#include "../entities/attacks.hpp"
#include "system.hpp"
#include "../world/water.hpp"
#include "../props/stove.hpp"
#include <algorithm>
#include <cmath>

Cell ambient_source_cell(const AmbientSource& source, const Game& game) {
    if (const auto* owner=get_entity(game,source.owner)) return owner->cell;
    return source.cell;
}

void add_ambient_source(AmbientAudio& audio, const Game& game, Cell listener,
    AmbientCue cue, Cell cell, bool global, Handle owner, PropKind prop) {
    if (audio.sources.size()>=128) return;
    int siblings=0;
    for (const auto& source:audio.sources) {
        if (source.cue!=cue) continue;
        if (!global && distance(ambient_source_cell(source,game),cell)<12) return;
        ++siblings;
    }
    if (siblings>=4) return;
    const auto& spec=ambient_specs[static_cast<std::size_t>(cue)];
    audio.sources.push_back({cue,cell,owner,prop,spec.cooldown*.5F,global,
        static_cast<float>(distance(cell,listener))<=spec.trigger_radius,false});
}

float ambient_source_gain(const AmbientSource& source, const Game& game, Cell listener) {
    const Entity* owner=get_entity(game,source.owner);
    if (source.owner.slot>=0 && (!owner || owner->health<=0)) return 0;
    if (source.has_scene && !ice_scene_alive(source.scene,game)) return 0;
    const Cell cell=ambient_source_cell(source,game);
    const Tile* tile=game.stage.at(cell);
    if (source.prop!=PropKind::None && (!tile || tile->prop.kind!=source.prop || tile->prop.broken)) return 0;
    // CONDITIONS: No humming empty tanks, frozen water loops or dead chimneys.
    switch (source.cue) {
    case AmbientCue::BeltRollers: {
        bool rolling=false;
        for (int y=-8;y<=8 && !rolling;++y) for (int x=-8;x<=8 && !rolling;++x) {
            const Cell section=cell+Cell{x,y};
            const auto& prop=game.stage.at_or_border(section).prop;
            rolling=live_belt(prop) && !(prop.variant&belt_manual) && prop.growth_ticks==0 &&
                clear_sight(game,listener,section);
        }
        if (!rolling) return 0;
        break;
    }
    case AmbientCue::BoilerIdle: if (!owner || owner->counter_b<=0) return 0; break;
    case AmbientCue::ChimneyDraft: if (!tile || !stove_lit(tile->prop)) return 0; break;
    case AmbientCue::IceGroan: if (!tile || tile->kind!=TileKind::Ice) return 0; break;
    case AmbientCue::SnowSettling: if (!tile || tile->kind!=TileKind::Snow) return 0; break;
    case AmbientCue::Stream: case AmbientCue::WallTrickle: case AmbientCue::PoolDrips:
    case AmbientCue::ReedHiss: case AmbientCue::Frogs: case AmbientCue::UnderIce:
    case AmbientCue::ThawDrip: case AmbientCue::SlushLap: case AmbientCue::Waterwheel:
    case AmbientCue::SubmergedKnock:
        if (!tile || (tile->kind!=TileKind::Water && !shallow_water(tile->kind))) return 0;
        break;
    default: break;
    }
    const auto& spec=ambient_specs[static_cast<std::size_t>(source.cue)];
    if (source.global) {
        const auto floor=game.stage.at_or_border(listener).kind;
        const bool inside=floor==TileKind::Ruin;
        if (source.cue==AmbientCue::ThinWind || source.cue==AmbientCue::FarIceCall)
            return spec.gain*(inside ? .25F : 1);
        return spec.gain;
    }
    const float dx=static_cast<float>(listener.x-cell.x),dy=static_cast<float>(listener.y-cell.y);
    const float range=std::sqrt(dx*dx+dy*dy);
    const float t=std::clamp((range-spec.near_radius)/(spec.far_radius-spec.near_radius),0.0F,1.0F);
    return spec.gain*(1-t)*(1-t);
}

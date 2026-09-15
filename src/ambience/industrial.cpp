#include "industrial.hpp"
#include "../entities/ember.hpp"
#include "../entities/magnet_crane.hpp"
#include "../entities/mine_crew.hpp"
#include "../props/conveyor.hpp"
#include "../surfaces/interaction.hpp"

namespace {
bool moving_machine(const Entity& owner) {
    return owner.freeze_ticks==0 && owner.stun_ticks==0 && owner.sleep_ticks==0;
}
}

bool industrial_ambient_active(const AmbientSource& source,const Game& game,Cell listener) {
    const Entity* owner=get_entity(game,source.owner);
    const Tile& tile=game.stage.at_or_border(ambient_source_cell(source,game));
    switch (source.cue) {
    case AmbientCue::FurnaceBreath:
        if (!owner || owner->freeze_ticks>0) return false;
        return owner->kind==EntityKind::Ember ? stoker_hot(*owner) :
            owner->kind==EntityKind::BoilerTank && owner->counter_b>0;
    case AmbientCue::DistantPicks:
        if (!owner || owner->kind!=EntityKind::ShiftForeman || owner->attack_wait>0 ||
            distance(listener,owner->cell)<6 || !moving_machine(*owner)) return false;
        for (const Entity& worker:game.entities)
            if (worker.kind==EntityKind::Pickhand && worker.health>0 &&
                worker.entity_a==source.owner && worker.attack_wait==0 &&
                worker.label_a==CrewCut && moving_machine(worker)) return true;
        return false;
    case AmbientCue::ChainSway:
        return owner && owner->kind==EntityKind::MagnetCrane && moving_machine(*owner) &&
            (owner->label_a==CraneTravel || owner->label_a==CraneLock || owner->label_a==CraneReturn);
    case AmbientCue::CoolingTicks:
        return owner && (owner->freeze_ticks>0 || (owner->kind==EntityKind::BoilerTank &&
            owner->counter_a<25 && surface_wet(tile)));
    case AmbientCue::SlagBubbling: return tile.kind==TileKind::Lava;
    case AmbientCue::WaterHammer:
        return owner && owner->kind==EntityKind::BoilerTank && owner->counter_b>0 &&
            owner->counter_a>30 && owner->freeze_ticks==0;
    default: return true;
    }
}

void place_industrial_ambience(AmbientAudio& audio,const Game& game,Cell listener) {
    // Ownership follows actual actors; invalid generations silence removed machines.
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.health<=0) continue;
        const auto add=[&](AmbientCue cue) {
            add_ambient_source(audio,game,listener,cue,actor.cell,false,{slot,actor.generation});
        };
        if (actor.kind==EntityKind::Ember || actor.kind==EntityKind::BoilerTank) add(AmbientCue::FurnaceBreath);
        if (actor.kind==EntityKind::ShiftForeman) add(AmbientCue::DistantPicks);
        if (actor.kind==EntityKind::MagnetCrane) add(AmbientCue::ChainSway);
        if (actor.kind==EntityKind::MagnetCrane || actor.kind==EntityKind::BoilerTank) add(AmbientCue::CoolingTicks);
        if (actor.kind==EntityKind::BoilerTank) add(AmbientCue::WaterHammer);
    }
    add_ambient_source(audio,game,listener,AmbientCue::ShiftBell,game.run.spawn,true);
    for (int y=0;y<game.stage.height;++y) for (int x=0;x<game.stage.width;++x) {
        const Tile& tile=*game.stage.at({x,y});
        if (tile.kind==TileKind::Lava) add_ambient_source(audio,game,listener,AmbientCue::SlagBubbling,{x,y});
        if (live_belt(tile.prop)) add_ambient_source(audio,game,listener,AmbientCue::BeltRollers,{x,y});
    }
}

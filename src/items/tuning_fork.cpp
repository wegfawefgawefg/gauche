#include "tuning_fork.hpp"
#include "../props/interaction.hpp"
#include "../entities/hearing.hpp"

namespace {
constexpr RegionalItem tuning_fork_def{"Tuning Fork",
    "Vibration crosses touching crystal. Shatters weak growths; disrupts shard nodes. Gaps, stone and felt stop it.",
    Sprite::TuningFork,{1,8,0,8,48,PatternEffect::Damage,true},
    ItemAction::Material,22,1,false,24,0,0,0,0,SoundId::ForkRing};
bool crystal(const Prop& prop) {
    return !prop.broken && (prop.kind==PropKind::CrystalGrowth ||
        prop.kind==PropKind::MirrorShard || prop.kind==PropKind::CrystalLens);
}
}

const RegionalItem* tuning_fork_item(ItemKind kind) {
    return kind==ItemKind::TuningFork ? &tuning_fork_def : nullptr;
}

bool ring_tuning_fork(Game& game, int slot, Cell direction) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    const ItemPattern pattern=item_pattern(*user.inventory.held());
    if (distance({},direction)!=1) return false;
    make_noise(game,user.cell,10);
    // CONDUCTION: Sample each cell before breaking its crystal. One empty endpoint
    // receives the vibration, but a gap cannot transmit it onward to a second vein.
    for (int step=1;step<=pattern.maximum;++step) {
        const Cell cell=user.cell+Cell{direction.x*step,direction.y*step};
        Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(tile->kind) || (tile->prop.covered && crystal(tile->prop))) break;
        bool gate=false;
        for (const Entity& actor : game.entities)
            if ((actor.kind==EntityKind::Door || actor.kind==EntityKind::EncounterGate) &&
                actor.impassable && actor.cell==cell) gate=true;
        if (gate) break;
        bool carries=crystal(tile->prop);
        if (prop_blocks(tile->prop) && !carries) break;
        for (const Entity& actor : game.entities)
            if (actor.kind==EntityKind::ShardColony && actor.health>0 && actor.cell==cell) carries=true;
        emit_sound(game,SoundId::ForkTravel,cell);
        for (int victim=0;victim<max_entities;++victim) {
            Entity& actor=game.entities[static_cast<std::size_t>(victim)];
            if (actor.kind==EntityKind::None || actor.health<=0 || actor.cell!=cell || victim==slot) continue;
            if (actor.kind==EntityKind::Door || actor.kind==EntityKind::EncounterGate) continue;
            damage_entity(game,victim,pattern.damage,user.cell);
        }
        if (crystal(tile->prop)) hit_prop(game,cell,pattern.damage,user.cell);
        if (!carries) break;
    }
    return true;
}

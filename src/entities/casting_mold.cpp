#include "casting_mold.hpp"
#include "../world/loot.hpp"

// A rare gameplay container needs the actual full Item, so it is an entity.
// Ordinary decoration remains compact Prop storage. fixture_open controls access;
// ground_item is the sole cargo, preserving ammunition, wear and modifiers.
void init_casting_mold(Entity& mold) {
    mold.health=mold.max_health=60;mold.impassable=mold.hard_blocker=true;
    mold.fixture_open=true;mold.sprite=Sprite::MoldOpen;
}
bool empty_casting_mold(const Entity& mold) {
    return mold.kind==EntityKind::CastingMold && mold.health>0 && mold.fixture_open && mold.ground_item.kind==ItemKind::None;
}
bool seal_casting_mold(Game& game,Entity& mold,Item& cargo) {
    if (!empty_casting_mold(mold) || cargo.kind==ItemKind::None || cargo.count<=0) return false;
    mold.ground_item=cargo;cargo={};mold.fixture_open=false;mold.sprite=Sprite::MoldClosed;
    emit_sound(game,SoundId::MoldSeal,mold.cell);return true;
}
Item recoverable_mold_item(const Game& game,Cell cell) {
    for (const Entity& mold:game.entities)
        if (mold.kind==EntityKind::CastingMold && mold.health>0 && mold.fixture_open && mold.cell==cell)
            return mold.ground_item;
    return {};
}
int release_mold_item(Game& game,Cell source,Cell destination) {
    for (Entity& mold:game.entities) {
        if (mold.kind!=EntityKind::CastingMold || mold.health<=0 || !mold.fixture_open || mold.cell!=source ||
            mold.ground_item.kind==ItemKind::None || mold.ground_item.count<=0) continue;
        const Handle handle=spawn_entity(game,EntityKind::GroundItem,destination);
        Entity* loose=get_entity(game,handle);
        if (!loose) return -1;
        loose->ground_item=mold.ground_item;loose->sprite=item_sprite(loose->ground_item);
        mold.ground_item={};emit_sound(game,SoundId::MoldRattle,source);return handle.slot;
    }
    return -1;
}
bool finish_mold_death(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health>0 || (actor.kind!=EntityKind::CastingMold && actor.kind!=EntityKind::MoldThief)) return false;
    const Entity dead=actor;const Item cargo=actor.ground_item;
    remove_entity(game,{slot,actor.generation});
    if (cargo.kind!=ItemKind::None && cargo.count>0) {
        // The dead body's own slot guarantees salvage even with a full pool.
        // New generation prevents an old carrier/mold handle targeting the loot.
        actor.kind=EntityKind::GroundItem;actor.generation=dead.generation+1;
        actor.birth_tick=game.tick;actor.cell=dead.cell;actor.ground_item=cargo;actor.sprite=item_sprite(cargo);
    }
    drop_enemy_loot(game,dead);
    return true;
}
bool valid_casting_mold(const Entity& mold) {
    if (mold.kind!=EntityKind::CastingMold) return true;
    return mold.max_health==60 && mold.health>0 && mold.health<=60 && mold.impassable && mold.hard_blocker &&
        mold.move_interval==0 && (mold.fixture_open || (mold.ground_item.kind!=ItemKind::None && mold.ground_item.count>0)) &&
        mold.ground_item.flight.slot<0 && mold.ground_item.anchor.slot<0;
}

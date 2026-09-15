#include "circuits.hpp"
#include "../surfaces/temperature.hpp"
#include "../surfaces/interaction.hpp"

bool circuit_prop(const Prop& prop) {
    return !prop.broken && prop.hp>0 &&
        (prop.kind==PropKind::CopperWire || prop.kind==PropKind::GroundingSpike);
}
bool grounding_ready(const Prop& prop) {
    return circuit_prop(prop) && prop.kind==PropKind::GroundingSpike && prop.variant==0;
}

// SLOTS: variant 0 ready / 1 hot spent / 2 cool spent; growth_ticks cooling, hp condition.
// Cooling never rearms it in place. A second pulse can pass the discharged spike.
void cool_grounding_spike(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || tile->prop.kind!=PropKind::GroundingSpike || tile->prop.broken || tile->prop.variant!=1) return;
    tile->prop.variant=2; tile->prop.growth_ticks=0;
    emit_sound(game,SoundId::SpikeCool,cell);
}

bool absorb_grounded_shock(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || !grounding_ready(tile->prop)) return false;
    tile->prop.variant=1; tile->prop.growth_ticks=180;
    emit_sound(game,SoundId::SpikeSink,cell);
    if (surface_wet(*tile)) cool_grounding_spike(game,cell);
    else warm_surface(game,cell,2);
    return true;
}

void step_grounding_spike(Game& game,Cell cell) {
    Tile& tile=*game.stage.at(cell);
    Prop& prop=tile.prop;
    if (prop.broken || prop.variant!=1) return;
    if (prop.growth_ticks>0) --prop.growth_ticks;
    if (prop.growth_ticks==0 || surface_wet(tile)) { cool_grounding_spike(game,cell); return; }
    // HEAT: Real residual warmth, not a permanent source or a new flame emitter.
    warm_surface(game,cell,2);
}

Item recoverable_spike(const Prop& prop) {
    if (!circuit_prop(prop) || prop.kind!=PropKind::GroundingSpike || prop.variant==1) return {};
    Item item=make_item(ItemKind::GroundingSpike);
    item.durability=prop.hp;
    return item;
}

int release_grounding_spike(Game& game,Cell cell,Cell destination) {
    Tile* tile=game.stage.at(cell);
    if (!tile) return -1;
    const Item item=recoverable_spike(tile->prop);
    if (item.kind==ItemKind::None) return -1;
    // CAPACITY: Reserve the loose item before removing the placed spike.
    const Handle handle=spawn_entity(game,EntityKind::GroundItem,destination);
    Entity* loose=get_entity(game,handle);
    if (!loose) return -1;
    loose->ground_item=item; loose->sprite=Sprite::GroundingSpike;
    tile->prop={};
    emit_sound(game,SoundId::SpikeLift,cell);
    return handle.slot;
}

#include "tension_spring.hpp"
#include "interaction.hpp"
#include "../combat/toss.hpp"
#include "../surfaces/temperature.hpp"
#include <array>

Cell tension_direction(std::uint8_t variant) {
    constexpr std::array<Cell,4> directions{{{1,0},{0,1},{-1,0},{0,-1}}};
    return directions[variant&3U];
}
bool trigger_tension_spring(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    Tile* tile=game.stage.at(actor.cell);
    if (!tile || tile->prop.kind!=PropKind::TensionSpring || tile->prop.broken || tile->prop.hp==0 ||
        tile->prop.growth_ticks>0 || actor.toss.ticks>0) return false;
    Prop& prop=tile->prop;
    const int cell=actor.cell.y*game.stage.width+actor.cell.x;
    const auto attribution=game.stage.prop_owners.find(cell);
    const PlayerId owner=attribution==game.stage.prop_owners.end() ? -1 : attribution->second;
    const Handle instigator=owner>=0 && has_player(game, owner) ? player_state(game, owner).controlled : Handle{};
    // Shared toss refuses flyers, rooted/gripping actors, sled riders and fixtures.
    // Refusal leaves the spring armed for a susceptible entrant.
    if (!toss_actor(game,slot,tension_direction(prop.variant),actor.cell,instigator)) return false;
    game.stage.prop_owners.erase(cell);
    prop.hp=0;prop.broken=true;prop.growth_ticks=0;
    emit_sound(game,SoundId::TensionLaunch,actor.cell);
    return true;
}
void step_tension_spring(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || tile->prop.kind!=PropKind::TensionSpring || tile->prop.broken) return;
    // Exposed fire and residual heat soften the finite mechanism.
    if (game.tick%30==0 && hot_cell(game,cell)) {
        hit_prop(game,cell,2,cell);
        if (tile->prop.broken) return;
    }
    bool newly_armed=false;
    if (tile->prop.growth_ticks>0) {
        --tile->prop.growth_ticks;
        newly_armed=tile->prop.growth_ticks==0;
        if (tile->prop.growth_ticks==0) emit_sound(game,SoundId::TensionReady,cell);
    }
    if (tile->prop.growth_ticks>0) return;
    // Entry triggers immediately. Recheck stationary occupants at 10 Hz, spread
    // across cells, so losing grip also triggers without an actor scan every frame.
    const auto phase=static_cast<std::uint64_t>(cell.x+cell.y);
    if (!newly_armed && (game.tick+phase)%6!=0) return;
    // A user who stays on their placed spring launches when it finishes arming.
    for (int slot=0;slot<max_entities;++slot)
        if (game.entities[static_cast<std::size_t>(slot)].cell==cell && trigger_tension_spring(game,slot)) return;
}

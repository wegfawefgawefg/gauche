#include "player_movement.hpp"

namespace {

bool free_step(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile != nullptr && walkable(*tile) && entity_at(game, cell, true) < 0;
}

Cell movement_axis(const Game& game, const Entity& player, Cell input) {
    if (input.x == 0 || input.y == 0) return input;
    const Cell horizontal{input.x, 0}, vertical{0, input.y};
    const Cell first = player.counter_c == 0 ? horizontal : vertical;
    const Cell second = player.counter_c == 0 ? vertical : horizontal;
    // CORNERS: Probe before moving; a failed move would spend this movement beat.
    if (!free_step(game, player.cell+first) && free_step(game, player.cell+second)) return second;
    return first;
}

} // namespace

void move_player(Game& game, int slot, Cell input) {
    Entity& player = game.entities[static_cast<std::size_t>(slot)];
    // SLOTS: counter_c records the last successful axis (1 horizontal, 0 vertical).
    // label_a/point_a latch a blocked destination; timer_a limits rapid bump taps.
    if (input == Cell{}) { player.label_a = 0; return; }
    const bool diagonal = input.x != 0 && input.y != 0;
    if (diagonal && player.move_wait > 0) return;
    const Cell movement = movement_axis(game, player, input);
    player.facing = movement;
    if (player.move_wait > 0) return;
    const Cell destination = player.cell + movement;
    if (move_entity(game, slot, destination)) {
        player.label_a = 0;
        player.counter_c = movement.x != 0 ? 1 : 0;
        return;
    }
    if ((player.label_a == 0 || player.point_a != destination) && player.timer_a == 0) {
        const Tile* tile = game.stage.at(destination);
        const bool wood = tile != nullptr &&
            (tile->prop.kind == PropKind::Crate || tile->prop.kind == PropKind::RottenLog);
        emit_sound(game, wood ? SoundId::BumpWood : SoundId::BumpStone, destination);
        player.timer_a = 15;
    }
    player.label_a = 1;
    player.point_a = destination;
}

#include "behavior.hpp"
#include "hearing.hpp"
#include "../items/fire.hpp"
#include "../lighting/shape.hpp"
#include "../props/growth.hpp"
#include "dispatch.hpp"
#include "attacks.hpp"

#include <algorithm>

namespace {

// SLOTS: entity_a emitter target; point_a brightest source cell;
// label_a orbit/dust; timer_a dust tell; timer_b source search; counter_a orbit turn.
void find_lamp(Game& game, Entity& moth) {
    int best = 0;
    moth.entity_a = {};
    moth.point_a = moth.cell;
    const auto consider = [&](Cell cell, LightEmitter light, Handle handle,Cell facing=Cell{1,0}) {
        const int range = distance(cell, moth.cell);
        if (range > 10 || light.strength <= 0 || range > light.radius ||
            !clear_sight(game, moth.cell, cell)) return;
        const int score = light.strength*light_direction_factor(light.shape,facing,moth.cell-cell) / (1000*(range + 1));
        if (score > best) { best = score; moth.entity_a = handle; moth.point_a = cell; }
    };
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& source = game.entities[static_cast<std::size_t>(slot)];
        if (&source == &moth || source.kind == EntityKind::None || (source.health <= 0 && source.kind != EntityKind::GroundItem)) continue;
        if (source.kind == EntityKind::Player && source.owner >= 0 && has_player(game, source.owner) &&
            !player_state(game, source.owner).online) continue;
        const Handle handle{slot, source.generation};
        consider(source.cell, source.light, handle,source.facing);
        consider(source.cell, item_light(*source.inventory.held()), handle,source.facing);
        if (source.kind == EntityKind::GroundItem) consider(source.cell, item_light(source.ground_item), handle,source.facing);
    }
    // PLANTS: Search a bounded patch, using the same emitter as the light renderer.
    for (int y = std::max(0, moth.cell.y - 10); y <= std::min(game.stage.height - 1, moth.cell.y + 10); ++y)
        for (int x = std::max(0, moth.cell.x - 10); x <= std::min(game.stage.width - 1, moth.cell.x + 10); ++x)
            consider({x, y}, prop_light(game.stage.at({x, y})->prop), {});
    // CANOPY: Gaps compete as fixed environmental sources, not fake entities.
    for (int i = 0; i < game.run.roof_light_count; ++i) {
        const StageLight& gap = game.run.roof_lights[static_cast<std::size_t>(i)];
        consider(gap.cell, gap.light, {});
    }
    moth.timer_b = 30;
}

} // namespace

void init_lantern_moth(Entity& moth) {
    moth.sprite = Sprite::LanternMoth;
    moth.health = moth.max_health = 14;
    moth.move_interval = 18;
    moth.impassable = true;
    moth.point_a = moth.cell;
}

void step_lantern_moth(Game& game, int slot) {
    Entity& moth = game.entities[static_cast<std::size_t>(slot)];
    if (moth.label_a == 1) {
        if (moth.timer_a > 0) return;
        resolve_enemy_attack(game, slot, 0, SoundId::MothDust, 60);
        moth.label_a = 0;
        moth.attack_wait = 120;
        return;
    }
    if (step_hearing(game, slot)) return;
    if (moth.timer_b == 0) find_lamp(game, moth);
    if (const Entity* source = get_entity(game, moth.entity_a); source != nullptr && source->health > 0)
        moth.point_a = source->cell;
    const int target = nearest_player(game, moth.cell, 1);
    if (target >= 0 && moth.attack_wait == 0) {
        moth.label_a = 1;
        moth.timer_a = 30;
        emit_sound(game, SoundId::MothFlutter, moth.cell);
        return;
    }
    if (distance(moth.cell, moth.point_a) > 2) pursue(game, slot, moth.point_a);
    else if (moth.move_wait == 0) {
        constexpr Cell orbit[]{{-2, 0}, {0, -2}, {2, 0}, {0, 2}};
        const Cell destination = moth.point_a + orbit[static_cast<std::size_t>(moth.counter_a % 4)];
        if (distance(moth.cell, destination) <= 1) moth.counter_a = (moth.counter_a + 1) % 4;
        approach(game, slot, destination);
    }
}

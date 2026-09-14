#include "../items/catalog.hpp"
#include "../props/interaction.hpp"
#include "shove.hpp"

namespace {

int contact_damage(const Item& item, const Entity& target, Cell source, int base) {
    if (item.kind != ItemKind::FlintKnife) return base;
    const Cell incoming = cardinal_toward(target.cell, source, target.facing);
    const bool rear = incoming == Cell{-target.facing.x, -target.facing.y};
    return target.sleep_ticks > 0 || rear ? base * 2 : base;
}

} // namespace

bool strike_melee(Game& game, int user_slot, Cell direction, const Item& item) {
    const Cell origin = game.entities[static_cast<std::size_t>(user_slot)].cell;
    const Cell sideways{-direction.y, direction.x};
    const ItemPattern pattern = item_pattern(item);
    bool struck = false;
    std::array<bool, max_entities> hit_once{};
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
        for (int reach = pattern.minimum; reach <= pattern.maximum; ++reach) {
            const Cell cell = origin + Cell{direction.x * reach + sideways.x * lane,
                                            direction.y * reach + sideways.y * lane};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr) break;
            const bool blocked = !walkable(*tile);
            int prop_damage = pattern.damage;
            if (item.kind == ItemKind::Hatchet &&
                (tile->prop.kind == PropKind::Crate || tile->prop.kind == PropKind::RottenLog))
                prop_damage *= 3;
            struck |= hit_prop(game, cell, prop_damage, origin);
            const int hit = entity_at(game, cell, true);
            if (hit >= 0 && hit != user_slot && !hit_once[static_cast<std::size_t>(hit)]) {
                Entity& target = game.entities[static_cast<std::size_t>(hit)];
                hit_once[static_cast<std::size_t>(hit)] = true;
                const bool blocked_hit = blocks_facing(target, origin);
                emit_sound(game, SoundId::Punch1, cell);
                damage_entity(game, hit, contact_damage(item, target, origin, pattern.damage), origin);
                if (item.kind == ItemKind::WoodenMaul && !blocked_hit && target.health > 0)
                    shove_actor(game, hit, direction, origin);
                struck = true;
                if (!pattern.piercing) break;
            }
            struck |= hit_terrain(game, cell, origin, pattern.damage, item.dig_power);
            // CONTACT: An unsuccessful wall blow still costs its attack beat.
            struck |= blocked;
            if (blocked) break;
        }
    }
    return struck;
}

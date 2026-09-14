#include "dispatch.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"
#include "scavenging.hpp"
#include "bird_feeding.hpp"
#include "../item_pattern.hpp"

#include <algorithm>

namespace {

int food_slot(const Entity& carrier) {
    for (int slot = 0; slot < quick_slots; ++slot) {
        const Item& item = carrier.inventory.slots[static_cast<std::size_t>(slot)];
        if (item.count > 0 && scavenger_food(item.kind)) return slot;
    }
    return -1;
}

void call_murder(Game& game, int slot, Handle carrier) {
    const Cell from = game.entities[static_cast<std::size_t>(slot)].cell;
    emit_sound(game, SoundId::CrowCall, from);
    for (Entity& crow : game.entities) {
        if (crow.kind != EntityKind::CarrionCrow || crow.health <= 0 ||
            crow.inventory.held()->count > 0 || distance(crow.cell, from) > 8) continue;
        crow.entity_b = carrier; crow.timer_b = 180;
    }
}

void take_food(Game& game, Entity& crow) {
    Entity* carrier = get_entity(game, crow.entity_b);
    if (carrier != nullptr && carrier->health > 0 && carrier->cell == crow.point_b &&
        distance(crow.cell, carrier->cell) == 1) {
        const int slot = food_slot(*carrier);
        if (slot >= 0) {
            Item& food = carrier->inventory.slots[static_cast<std::size_t>(slot)];
            *crow.inventory.held() = food;
            crow.inventory.held()->count = 1;
            if (--food.count == 0) food = {};
            emit_sound(game, SoundId::CrowSnatch, crow.cell);
        }
    }
    crow.label_a = crow.inventory.held()->count > 0 ? 2 : 3;
    crow.timer_a = crow.label_a == 2 ? 180 : 60;
    crow.sprite = Sprite::CarrionCrow;
}

} // namespace

// SLOTS: point_a perch; point_b committed theft; entity_a loose food; entity_b carrier;
// label_a forage/tell/return/recovery; timer_a scan/eating/phase; timer_b shared call.
// Actual stolen food occupies held inventory until eaten or dropped on death.
void init_carrion_crow(Entity& crow) {
    crow.sprite = Sprite::CarrionCrow;
    crow.health = crow.max_health = 24;
    crow.move_interval = 10;
    crow.impassable = true;
    crow.point_a = crow.cell;
}

void step_carrion_crow(Game& game, int slot) {
    Entity& crow = game.entities[static_cast<std::size_t>(slot)];
    if (crow.label_a == 1) {
        if (crow.timer_a == 0) take_food(game, crow);
        return;
    }
    if (crow.label_a == 3) {
        if (crow.timer_a == 0) crow.label_a = 0;
        else {
            const Entity* threat = get_entity(game, crow.entity_b);
            if (threat != nullptr) flee(game, slot, threat->cell);
        }
        return;
    }
    if (step_hearing(game, slot)) return;
    if (crow.inventory.held()->count > 0) {
        crow.label_a = 2;
        if (crow.cell != crow.point_a) { pursue(game, slot, crow.point_a); crow.timer_a = 180; }
        else if (crow.timer_a == 0) {
            const Item& meal = *crow.inventory.held();
            const bool sleepy = meal.kind == ItemKind::FungalBread;
            crow.health = std::min(crow.max_health, crow.health + item_pattern(meal).heal);
            if (sleepy) apply_sleep(crow, 120);
            *crow.inventory.held() = {};
            crow.label_a = 0; crow.timer_a = 90;
            emit_sound(game, sleepy ? SoundId::BreadMunch : SoundId::MeatMunch, crow.cell);
        }
        return;
    }
    if (feed_on_bird_seed(game, slot)) return;
    if (crow.timer_a == 0) {
        crow.entity_a = find_scavenge(game, slot, true, 8);
        crow.timer_a = 45;
    }
    Entity* loose = get_entity(game, crow.entity_a);
    if (loose != nullptr && loose->kind == EntityKind::GroundItem && scavenger_food(loose->ground_item.kind)) {
        if (collect_scavenge(game, slot, crow.entity_a, true)) { crow.entity_a = {}; crow.timer_a = 180; return; }
        pursue(game, slot, loose->cell);
        return;
    }
    Entity* carrier = crow.timer_b > 0 ? get_entity(game, crow.entity_b) : nullptr;
    if (carrier == nullptr || carrier->health <= 0 || food_slot(*carrier) < 0) {
        const int nearest = nearest_player(game, crow.cell, 5);
        carrier = nearest < 0 ? nullptr : &game.entities[static_cast<std::size_t>(nearest)];
        if (carrier != nullptr && food_slot(*carrier) >= 0) {
            crow.entity_b = {nearest, carrier->generation};
            if (crow.timer_b == 0) call_murder(game, slot, crow.entity_b);
        } else carrier = nullptr;
    }
    if (carrier != nullptr && clear_sight(game, crow.cell, carrier->cell)) {
        if (distance(crow.cell, carrier->cell) == 1) {
            crow.point_b = carrier->cell; crow.facing = carrier->cell - crow.cell;
            crow.label_a = 1; crow.timer_a = 24; crow.sprite = Sprite::CarrionCrowSnatch;
            emit_sound(game, SoundId::CrowRattle, crow.cell);
        } else pursue(game, slot, carrier->cell);
    } else if (distance(crow.cell, crow.point_a) > 3) pursue(game, slot, crow.point_a);
    else wander(game, slot);
}

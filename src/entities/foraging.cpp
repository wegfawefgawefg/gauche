#include "foraging.hpp"
#include "behavior.hpp"
#include "../surfaces/scent.hpp"

#include <algorithm>
#include <vector>

bool eats_meat(EntityKind kind) {
    return kind == EntityKind::Wolf || kind == EntityKind::Bear ||
           kind == EntityKind::Boar || kind == EntityKind::Dog;
}

bool step_foraging(Game& game, int slot, bool committed_attack) {
    Entity& animal = game.entities[static_cast<std::size_t>(slot)];
    // SHARED SLOTS: counter_b = satiety, label_b = idle/eating/approaching food.
    animal.counter_b = std::max(0, animal.counter_b - 1);
    if (committed_attack || enemy_target(game, animal.cell, 1).has_value()) {
        animal.label_b = 0;
        return false;
    }
    if (animal.label_b == 1 && animal.counter_b > 270) return true;
    if (animal.counter_b > 0) { animal.label_b = 0; return false; }
    if (animal.move_wait > 0) return animal.label_b == 2 || step_scent(game, slot);
    animal.label_b = 0;
    std::vector<int> candidates;
    for (int i = 0; i < max_entities; ++i) {
        const Entity& food = game.entities[static_cast<std::size_t>(i)];
        if (food.kind != EntityKind::GroundItem || food.ground_item.count <= 0) continue;
        const ItemKind kind = food.ground_item.kind;
        const bool honey = kind == ItemKind::HoneyPot && animal.kind == EntityKind::Bear;
        if (!honey && kind != ItemKind::SmokedFish && kind != ItemKind::RawMeat && kind != ItemKind::CookedMeat) continue;
        if (distance(animal.cell, food.cell) > (kind == ItemKind::SmokedFish || kind == ItemKind::CookedMeat || honey ? 9 : 6)) continue;
        const int occupant = entity_at(game, food.cell, true);
        if (occupant >= 0 && occupant != slot) continue;
        candidates.push_back(i);
    }
    std::stable_sort(candidates.begin(), candidates.end(), [&](int a, int b) {
        return distance(animal.cell, game.entities[static_cast<std::size_t>(a)].cell) <
               distance(animal.cell, game.entities[static_cast<std::size_t>(b)].cell);
    });
    for (int food_slot : candidates) {
        Entity& food = game.entities[static_cast<std::size_t>(food_slot)];
        if (animal.cell == food.cell) {
            const bool fish = food.ground_item.kind == ItemKind::SmokedFish;
            --food.ground_item.count;
            if (food.ground_item.count == 0) remove_entity(game, {food_slot, food.generation});
            animal.health = std::min(animal.max_health, animal.health + 4);
            animal.counter_b = 360;
            animal.label_b = 1;
            emit_sound(game, fish ? SoundId::FishNibble : SoundId::MeatMunch, animal.cell);
            return true;
        }
        // SCENT: A bounded route check prevents food behind a sealed wall becoming a lure.
        if (const auto next = next_route_cell(game, slot, food.cell, 256)) {
            animal.label_b = 2;
            move_entity(game, slot, *next);
            return true;
        }
    }
    return step_scent(game, slot);
}

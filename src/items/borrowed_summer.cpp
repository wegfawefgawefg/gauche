#include "borrowed_summer.hpp"
#include "../surfaces/temperature.hpp"
#include "../entities/attacks.hpp"

namespace {
constexpr RegionalItem summer{"Borrowed Summer",
    "Carry a warm cross for 4s. Thaws nearby friends, foes and ice; ignites oil. No damage protection. Two uses.",
    Sprite::BorrowedSummer,{0,0,1,0,45,PatternEffect::Utility,false,0,0,false,false,true},
    ItemAction::Material,38,1,false,2,0,0,0,0,SoundId::SummerOpen};

void warm_around(Game& game, const Entity& source) {
    // Brief surface warmth shares all existing thaw, fuel and heat-seeking rules.
    // The trace stops at cover, and the previous position cools within two ticks.
    warm_surface(game,source.cell,2);
    for (Cell direction : {Cell{1,0},{-1,0},{0,1},{0,-1}})
        for (int reach=1; reach<=source.vitals.summer_radius; ++reach) {
            const Cell cell=source.cell+Cell{direction.x*reach,direction.y*reach};
            if (!clear_attack_sight(game,source.cell,cell,false)) break;
            warm_surface(game,cell,2);
        }
}
}

const RegionalItem* borrowed_summer_item(ItemKind kind) {
    return kind==ItemKind::BorrowedSummer ? &summer : nullptr;
}

bool use_borrowed_summer(Game& game, int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];
    if (user.health<=0 || user.vitals.summer_ticks>0) return false;
    user.vitals.summer_ticks=240;
    user.vitals.summer_radius=static_cast<std::uint8_t>(item_pattern(*user.inventory.held()).blast_radius);
    warm_around(game,user);
    return true;
}

void step_summer_auras(Game& game) {
    for (const Entity& source : game.entities) {
        if (source.kind==EntityKind::None || source.health<=0 || source.vitals.summer_ticks==0) continue;
        if (source.kind==EntityKind::Player && (source.owner<0 || !has_player(game, source.owner) ||
            !player_state(game, source.owner).online)) continue;
        warm_around(game,source);
    }
}

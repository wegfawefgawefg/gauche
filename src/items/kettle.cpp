#include "kettle.hpp"
#include "heated_water.hpp"
#include "../entities/attacks.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../world/water.hpp"

#include <algorithm>

namespace {
constexpr RegionalItem kettle{"Steam Kettle", "Fill at water; heat nearby for 1.5s. Hot spray scalds friends too. Cold spray douses. Reusable; one charge.",
    Sprite::SteamKettle,{1,2,0,12,45,PatternEffect::Damage,false,1,0,false,true},
    ItemAction::Material,22,1,false,0,0,0,0,0,SoundId::KettlePour};

}

const RegionalItem* kettle_item(ItemKind kind) { return kind == ItemKind::SteamKettle ? &kettle : nullptr; }

std::vector<Cell> kettle_cells(const Game& game, const Item& item, Cell source, Cell direction) {
    const ItemPattern pattern = item_pattern(item);
    const Cell side{-direction.y,direction.x};
    std::vector<Cell> cells;
    for (int reach=pattern.minimum;reach<=pattern.maximum;++reach) {
        const int width = pattern_half_width(pattern,reach);
        for (int lane=-width;lane<=width;++lane) {
            const Cell cell = source+Cell{direction.x*reach+side.x*lane,direction.y*reach+side.y*lane};
            const Tile* tile = game.stage.at(cell);
            const bool sight=item.kind==ItemKind::SteamLance ? clear_shot_sight(game,source,cell,false) : clear_attack_sight(game,source,cell,false);
            if (tile && walkable(tile->kind) && sight) cells.push_back(cell);
        }
    }
    return cells;
}

bool use_kettle(Game& game, int slot, Cell direction) {
    Entity& user = game.entities[static_cast<std::size_t>(slot)];
    Item& held = *user.inventory.held();
    const Cell source = user.cell;
    if (held.loaded == 0) {
        if (!fresh_water(game,source+direction) && !fresh_water(game,source)) return false;
        held.loaded = 1; held.spare = 0;
        emit_sound(game,SoundId::KettleFill,source);
        return true;
    }
    // COVER: Freeze the footprint before damage, melting or reflection changes the scene.
    const Item item = held;
    const auto cells = kettle_cells(game,item,source,direction);
    if (cells.empty()) return false;
    held.loaded = held.spare = 0;
    const int damage = item_pattern(item).damage;
    emit_sound(game,SoundId::KettlePour,source);
    for (Cell cell : cells) {
        if (damage > 0)
            for (int target=0;target<max_entities;++target) {
                const Entity& actor = game.entities[static_cast<std::size_t>(target)];
                if (target != slot && actor.kind != EntityKind::None && actor.health > 0 && actor.cell == cell)
                    damage_entity(game,target,damage,source);
            }
        pour_surface(game,cell,LiquidKind::Water,300);
        quench_cell(game,cell);
        emit_sound(game,item.loaded == 2 ? SoundId::KettleScald : SoundId::KettleSplash,cell);
    }
    return true;
}

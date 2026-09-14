#include "kettle.hpp"
#include "../entities/attacks.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../world/water.hpp"

#include <algorithm>

namespace {
constexpr RegionalItem kettle{"Steam Kettle", "Fill at water; heat nearby for 1.5s. Hot spray scalds friends too. Cold spray douses. Reusable; one charge.",
    Sprite::SteamKettle,{1,2,0,12,45,PatternEffect::Damage,false,1,0,false,true},
    ItemAction::Material,22,1,false,0,0,0,0,0,SoundId::KettlePour};

bool fresh_water(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (!tile) return false;
    const Surface& surface = tile->surface;
    // CONTENTS: Oil and brine do not become fresh water by passing through a kettle.
    if (surface.liquid_ticks > 0 && surface.liquid != LiquidKind::None &&
        surface.liquid != LiquidKind::Water) return false;
    return shallow_water(tile->kind) || tile->kind == TileKind::Water ||
        (surface.liquid == LiquidKind::Water && surface.liquid_ticks > 0);
}
}

const RegionalItem* kettle_item(ItemKind kind) { return kind == ItemKind::SteamKettle ? &kettle : nullptr; }

// STORAGE: loaded is empty/cold/hot (0/1/2); spare is heating progress or remaining heat.
// Every carried and loose item ages, so swapping slots never preserves boiling water.
void step_kettle(Game& game, Item& item, Cell cell, bool wet) {
    if (item.kind != ItemKind::SteamKettle || item.loaded == 0) return;
    if (wet) { item.loaded = 1; item.spare = 0; return; }
    const bool heated = warm_cell(game,cell);
    if (item.loaded == 2) {
        if (heated) item.spare = kettle_cool_ticks;
        else if (--item.spare <= 0) {
            item.loaded = 1; item.spare = 0;
            emit_sound(game,SoundId::KettleCool,cell);
        }
        return;
    }
    item.spare = heated ? item.spare+1 : std::max(0,item.spare-1);
    if (item.spare < kettle_heat_ticks) return;
    item.loaded = 2; item.spare = kettle_cool_ticks;
    emit_sound(game,SoundId::KettleReady,cell);
}

std::vector<Cell> kettle_cells(const Game& game, const Item& item, Cell source, Cell direction) {
    const ItemPattern pattern = item_pattern(item);
    const Cell side{-direction.y,direction.x};
    std::vector<Cell> cells;
    for (int reach=pattern.minimum;reach<=pattern.maximum;++reach) {
        const int width = pattern_half_width(pattern,reach);
        for (int lane=-width;lane<=width;++lane) {
            const Cell cell = source+Cell{direction.x*reach+side.x*lane,direction.y*reach+side.y*lane};
            const Tile* tile = game.stage.at(cell);
            if (tile && walkable(tile->kind) && clear_attack_sight(game,source,cell,false)) cells.push_back(cell);
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

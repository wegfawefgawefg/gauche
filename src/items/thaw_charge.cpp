#include "thaw_charge.hpp"
#include "../projectiles/projectile.hpp"
#include "../entities/attacks.hpp"
#include "../surfaces/interaction.hpp"

#include <vector>

namespace {
constexpr RegionalItem charge{"Thaw Charge",
    "2s fuse melts two ice walls. Steam hits you and neighbors. Place facing ice. Water won't stop it.",
    Sprite::ThawCharge,{0,0,1,10,30,PatternEffect::Damage,false,0,0,false,false,true},
    ItemAction::Material,19,3,true,0,0,0,0,0,SoundId::ThawPlace};

void burst(Game& game, int slot) {
    const Entity shot=game.entities[static_cast<std::size_t>(slot)];
    const ItemPattern pattern=item_pattern(shot.ground_item);
    std::vector<Cell> steam{shot.cell};
    // COVER: Freeze steam reach before a wall opens. A protected far side stays safe.
    for (Cell direction : {Cell{1,0},{-1,0},{0,1},{0,-1}})
        for (int reach=1;reach<=pattern.blast_radius;++reach) {
            const Cell cell=shot.cell+Cell{direction.x*reach,direction.y*reach};
            if (!clear_attack_sight(game,shot.cell,cell,false)) break;
            steam.push_back(cell);
        }
    for (int reach=1;reach<=2;++reach) {
        const Cell cell=shot.cell+Cell{shot.facing.x*reach,shot.facing.y*reach};
        Tile* tile=game.stage.at(cell);
        if (!tile) break;
        if (tile->kind==TileKind::Wall) {
            if (!thawable_wall(game.stage,cell)) break;
            const int hp=tile->hp;
            if (!damage_tile(game.stage,cell,hp,255)) break;
            tile->kind=TileKind::ShallowWater;
            if (game.impact_count<static_cast<int>(game.impacts.size()))
                game.impacts[static_cast<std::size_t>(game.impact_count++)]={cell,shot.cell,Sprite::IceWall,hp,true};
            emit_sound(game,SoundId::ThawSteam,cell);
        } else if (!walkable(*tile)) break;
        const int fixture=entity_at(game,cell,true);
        if (fixture>=0 && game.entities[static_cast<std::size_t>(fixture)].hard_blocker) break;
    }
    emit_sound(game,SoundId::ThawBurst,shot.cell);
    for (Cell cell : steam) {
        for (int victim=0;victim<max_entities;++victim) {
            const Entity& actor=game.entities[static_cast<std::size_t>(victim)];
            if (actor.kind!=EntityKind::None && actor.health>0 && actor.cell==cell)
                damage_entity(game,victim,pattern.damage,shot.cell,false);
        }
        pour_surface(game,cell,LiquidKind::Water,180);
        emit_sound(game,SoundId::ThawSteam,cell);
    }
    remove_entity(game,{slot,shot.generation});
}
}

const RegionalItem* thaw_charge_item(ItemKind kind) {
    return kind==ItemKind::ThawCharge ? &charge : nullptr;
}

bool thawable_wall(const Stage& stage, Cell cell) {
    const Tile* tile=stage.at(cell);
    return cell.x>0 && cell.y>0 && cell.x<stage.width-1 && cell.y<stage.height-1 &&
        tile && tile->kind==TileKind::Wall && tile->material==TileMaterial::Ice &&
        tile->hp>0 && tile->break_rule!=BreakRule::Unbreakable;
}

// SLOTS: timer_a sealed chemical fuse; cell/point_a fixed placement, facing bore axis.
// entity_a generation-checked placer, ground_item actual modifier-bearing charge.
bool place_thaw_charge(Game& game, int slot) {
    const Entity& user=game.entities[static_cast<std::size_t>(slot)];
    if (!thawable_wall(game.stage,user.cell+user.facing)) return false;
    for (const Entity& actor : game.entities)
        if (actor.kind==EntityKind::Projectile && actor.label_a==static_cast<int>(ProjectileKind::ThawCharge) &&
            actor.cell==user.cell) return false;
    Entity* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,user.cell));
    if (!shot) return false;
    shot->label_a=static_cast<int>(ProjectileKind::ThawCharge);
    shot->timer_a=thaw_fuse_ticks; shot->point_a=user.cell; shot->facing=user.facing;
    shot->entity_a={slot,user.generation}; shot->ground_item=*user.inventory.held(); shot->ground_item.count=1;
    shot->sprite=Sprite::ThawChargeLit; shot->light={2,220,{230,170,92}};
    return true;
}

void step_thaw_charge(Game& game, int slot) {
    Entity& shot=game.entities[static_cast<std::size_t>(slot)];
    if (shot.kind!=EntityKind::Projectile || shot.label_a!=static_cast<int>(ProjectileKind::ThawCharge)) return;
    if (shot.timer_a==0) { burst(game,slot); return; }
    const bool urgent=shot.timer_a<=30;
    shot.sprite=urgent ? Sprite::ThawChargeReady : Sprite::ThawChargeLit;
    shot.light.strength=urgent ? 420 : 220;
    if (shot.timer_a%(urgent ? 10 : 30)==0) emit_sound(game,SoundId::ThawFuse,shot.cell);
}

bool valid_thaw_charge(const Entity& actor) {
    if (actor.kind!=EntityKind::Projectile || actor.label_a!=static_cast<int>(ProjectileKind::ThawCharge)) return true;
    return actor.ground_item.kind==ItemKind::ThawCharge && actor.ground_item.count==1 &&
        actor.timer_a>=0 && actor.timer_a<=thaw_fuse_ticks && actor.counter_a==0 &&
        distance({},actor.facing)==1 && actor.cell==actor.point_a;
}

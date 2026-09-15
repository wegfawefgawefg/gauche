#include "quarry_charge.hpp"
#include "../projectiles/projectile.hpp"
#include "../entities/attacks.hpp"
#include "../entities/hearing.hpp"
#include "../props/interaction.hpp"
#include <array>

namespace {
constexpr RegionalItem charge{"Quarry Charge",
    "3s fuse cuts 3 stone cells ahead. Backblast hits here and one cell behind. Water quenches; cold slows.",
    Sprite::QuarryCharge,{0,3,0,24,30,PatternEffect::Damage,true},
    ItemAction::Material,19,3,true,0,0,0,0,1,SoundId::QuarryPlace};

void burst(Game& game,int slot) {
    const Entity shot=game.entities[static_cast<std::size_t>(slot)];
    remove_entity(game,{slot,shot.generation});
    // BACKBLAST: Snapshot its cover before the forward cut changes any terrain.
    const Cell back=shot.cell- shot.facing;
    const bool exposed=clear_attack_sight(game,shot.cell,back,false);
    std::array<Cell,5> cells{}; int count=0;
    cells[static_cast<std::size_t>(count++)]=shot.cell;
    if (exposed) cells[static_cast<std::size_t>(count++)]=back;
    for (int reach=1;reach<=3;++reach) {
        const Cell cell=shot.cell+Cell{shot.facing.x*reach,shot.facing.y*reach};
        const Tile* tile=game.stage.at(cell);
        if (!tile) break;
        const int blocker=entity_at(game,cell,true);
        if (blocker>=0 && game.entities[static_cast<std::size_t>(blocker)].hard_blocker) break;
        const bool prop_stop=prop_blocks(tile->prop);
        if (tile->kind==TileKind::Wall) {
            if (!quarry_wall(game.stage,cell)) break;
            if (!hit_terrain(game,cell,shot.cell,tile->hp,1,TileImpact::Blast)) break;
        } else if (!walkable(tile->kind)) break;
        cells[static_cast<std::size_t>(count++)]=cell;
        if (prop_stop) break;
    }
    emit_sound(game,SoundId::QuarryBurst,shot.cell); make_noise(game,shot.cell,12);
    for (int i=0;i<count;++i) {
        const Cell cell=cells[static_cast<std::size_t>(i)];
        hit_prop(game,cell,24,shot.cell);
        for (int victim=0;victim<max_entities;++victim) {
            const auto& actor=game.entities[static_cast<std::size_t>(victim)];
            if (actor.kind!=EntityKind::None && actor.kind!=EntityKind::GroundItem && actor.cell==cell && actor.health>0)
                damage_entity(game,victim,24,shot.cell,false);
        }
    }
}
}
const RegionalItem* quarry_charge_item(ItemKind kind) { return kind==ItemKind::QuarryCharge ? &charge : nullptr; }
bool quarry_wall(const Stage& stage,Cell cell) {
    const Tile* tile=stage.at(cell);
    return cell.x>0 && cell.y>0 && cell.x<stage.width-1 && cell.y<stage.height-1 &&
        tile && tile->kind==TileKind::Wall && tile->material==TileMaterial::Stone && tile->hp>0 &&
        tile->break_rule!=BreakRule::Unbreakable &&
        (tile->break_rule==BreakRule::Damageable || tile->required_dig_power<=1);
}
Handle arm_quarry_charge(Game& game,int slot,Item item,Cell direction,int fuse_ticks) {
    const auto& user=game.entities[static_cast<std::size_t>(slot)];
    if (distance({},direction)!=1 || item.kind!=ItemKind::QuarryCharge || item.count<=0 ||
        fuse_ticks<1 || fuse_ticks>180) return {};
    const Tile& cover=game.stage.at_or_border(user.cell+direction);
    if (!quarry_wall(game.stage,user.cell+direction) && !(prop_blocks(cover.prop) && cover.prop.hp>0)) return {};
    for (const auto& actor:game.entities)
        if (actor.kind==EntityKind::Projectile && actor.cell==user.cell &&
            actor.label_a==static_cast<int>(ProjectileKind::QuarryCharge)) return {};
    const Handle handle=spawn_entity(game,EntityKind::Projectile,user.cell);
    auto* shot=get_entity(game,handle);
    if (!shot) return {};
    shot->label_a=static_cast<int>(ProjectileKind::QuarryCharge);
    shot->timer_a=fuse_ticks; shot->point_a=user.cell; shot->facing=direction;
    shot->entity_a={slot,user.generation}; shot->ground_item=item; shot->ground_item.count=1;
    shot->sprite=Sprite::QuarryChargeLit; shot->light={2,180,{242,163,64}};
    return handle;
}
bool place_quarry_charge(Game& game,int slot) {
    const auto& user=game.entities[static_cast<std::size_t>(slot)];
    if (!quarry_wall(game.stage,user.cell+user.facing)) return false;
    return get_entity(game,arm_quarry_charge(game,slot,*user.inventory.held(),user.facing,180))!=nullptr;
}
void step_quarry_charge(Game& game,int slot) {
    auto& shot=game.entities[static_cast<std::size_t>(slot)];
    if (shot.kind!=EntityKind::Projectile || shot.label_a!=static_cast<int>(ProjectileKind::QuarryCharge)) return;
    if (shot.timer_a==0) { burst(game,slot); return; }
    const bool urgent=shot.timer_a<=30;
    shot.sprite=urgent ? Sprite::QuarryChargeReady : Sprite::QuarryChargeLit;
    shot.light.strength=urgent ? 360 : 180;
    if (shot.freeze_ticks==0 && shot.timer_a%(urgent ? 10 : 30)==0) emit_sound(game,SoundId::QuarryFuse,shot.cell);
}
bool valid_quarry_charge(const Entity& actor) {
    if (actor.kind!=EntityKind::Projectile || actor.label_a!=static_cast<int>(ProjectileKind::QuarryCharge)) return true;
    return actor.ground_item.kind==ItemKind::QuarryCharge && actor.ground_item.count==1 &&
        actor.timer_a>=0 && actor.timer_a<=180 && actor.counter_a==0 &&
        distance({},actor.facing)==1 && actor.cell==actor.point_a;
}

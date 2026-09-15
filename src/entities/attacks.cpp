#include "emergency_pump.hpp"
#include "counterweight.hpp"
#include "ash_sleeper.hpp"
#include "slag_snail.hpp"
#include "furnace_moth.hpp"
#include "walking_kiln.hpp"
#include "cable_crawler.hpp"
#include "pressure_rat.hpp"
#include "magnet_crane.hpp"
#include "arc_welder.hpp"
#include "yeti.hpp"
#include "strikebreaker.hpp"
#include "rivet_gunner.hpp"
#include "mine_crew.hpp"
#include "powder_monkey.hpp"
#include "attacks.hpp"
#include "ember.hpp"
#include "bell_diver.hpp"
#include "steam_leech.hpp"
#include "ice_mason.hpp"
#include "snow_burrower.hpp"
#include "mirror_knight.hpp"
#include "echo_hound.hpp"
#include "frozen_pilgrim.hpp"
#include "fishing_widow.hpp"
#include "seal_thief.hpp"
#include "avalanche_ram.hpp"
#include "snow_effigy.hpp"
#include "candle_keeper.hpp"
#include "icicle_spider.hpp"
#include "boiler_porter.hpp"
#include "boiler_tank.hpp"
#include "../props/interaction.hpp"

#include <algorithm>
#include <cstdlib>

EnemyAttack enemy_attack(const Entity& enemy) {
    EnemyAttack attack;
    const auto add = [&attack](Cell cell) {
        if (attack.count < static_cast<int>(attack.cells.size()))
            attack.cells[static_cast<std::size_t>(attack.count++)] = cell;
    };
    switch (enemy.kind) {
    case EntityKind::EmergencyPump:
        if (enemy.label_a==PumpWarn && enemy.cell==enemy.point_a)
            for (int i=1;i<=std::min(3,(enemy.counter_b+599)/600);++i)
                add(enemy.cell+Cell{enemy.point_b.x*i,enemy.point_b.y*i});
        break;
    case EntityKind::Counterweight:
        if ((enemy.label_a==WeightWarn || enemy.label_a==WeightDrop) && enemy.cell==enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::AshSleeper:
        if (enemy.label_a==AshSwipe && enemy.cell==enemy.point_a) add(enemy.point_a+enemy.point_b);
        break;
    case EntityKind::SlagSnail:
        if ((enemy.label_a==SlagTuck || enemy.label_a==SlagLunge) && enemy.cell==enemy.point_a)
            for (int i=1;i<=enemy.counter_a;++i) add(enemy.cell+Cell{enemy.point_b.x*i,enemy.point_b.y*i});
        break;
    case EntityKind::FurnaceMoth:
        if (enemy.cell==enemy.point_b && (enemy.label_a==FurnaceWarn || enemy.label_a==FurnaceDive))
            for (int i=1;i<=enemy.counter_b;++i) add(enemy.cell+Cell{enemy.facing.x*i,enemy.facing.y*i});
        break;
    case EntityKind::RivetGunner:
        if (enemy.cell==enemy.point_a && (enemy.label_a==GunnerBrace || enemy.label_a==GunnerBurst))
            for (int i=1;i<=8;++i) add(enemy.point_a+Cell{enemy.point_b.x*i,enemy.point_b.y*i});
        break;
    case EntityKind::Strikebreaker:
        if (enemy.cell==enemy.point_a && (enemy.label_a==BreakerPush ||
            enemy.label_a==BreakerHammer || enemy.label_a==BreakerCut)) add(enemy.point_b);
        break;
    case EntityKind::PowderMonkey:
        if (enemy.label_a==PowderStrike && enemy.cell==enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::Ember:
        if (enemy.cell==enemy.point_a && enemy.label_a==StokerStrike) add(enemy.point_b);
        if (enemy.cell==enemy.point_a && enemy.label_a==StokerPack)
            for (int i=1;i<=8;++i) add(enemy.cell+Cell{enemy.point_b.x*i,enemy.point_b.y*i});
        break;
    case EntityKind::Pickhand: case EntityKind::ShiftForeman:
        if (enemy.label_a==CrewStrike && enemy.cell==enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::BoilerPorter:
        if (enemy.label_a == PorterBite && enemy.cell == enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::BoilerTank:
        if (enemy.label_a == BoilerTell && enemy.cell == enemy.point_a)
            for (int i=1;i<=4;++i) add(enemy.cell+Cell{enemy.point_b.x*i,enemy.point_b.y*i});
        break;
    case EntityKind::IcicleSpider:
        if (enemy.label_a == SpiderBite && enemy.cell == enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::CandleKeeper:
        if (enemy.label_a == KeeperStrike && enemy.cell == Cell{enemy.counter_a,enemy.counter_b})
            for (int i=1;i<=2;++i) add(enemy.cell+Cell{enemy.facing.x*i,enemy.facing.y*i});
        break;
    case EntityKind::SnowEffigy:
        if (enemy.label_a == EffigyStrike && enemy.cell == enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::WalkingKiln:
        if ((enemy.label_a==KilnDoor || enemy.label_a==KilnBreath) && enemy.cell==enemy.point_a)
            for (int row=enemy.label_a==KilnDoor ? 1 : enemy.counter_b;row<=3;++row)
                for (int side=-1;side<=1;++side)
                    add(enemy.cell+Cell{enemy.point_b.x*row-enemy.point_b.y*side,enemy.point_b.y*row+enemy.point_b.x*side});
        break;
    case EntityKind::CableCrawler:
        if (enemy.label_a==CrawlerCharge && enemy.cell==enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::PressureRat:
        if ((enemy.label_a==RatInflate || enemy.label_a==RatDash) && enemy.cell==enemy.point_a)
            for (int i=1;i<=std::min(8,enemy.counter_a);++i) add(enemy.cell+Cell{enemy.point_b.x*i,enemy.point_b.y*i});
        break;
    case EntityKind::MagnetCrane:
        if (enemy.label_a==CraneLock && enemy.cell==enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::ArcWelder:
        if ((enemy.label_a==WelderMask || enemy.label_a==WelderSweep) && enemy.cell==enemy.point_a) {
            const Cell side{-enemy.point_b.y,enemy.point_b.x};
            for (int lane=enemy.counter_a-1;lane<=1;++lane) for (int reach=1;reach<=2;++reach)
                add(enemy.cell+Cell{enemy.point_b.x*reach+side.x*lane,enemy.point_b.y*reach+side.y*lane});
        }
        break;
    case EntityKind::Yeti:
        if (enemy.label_a==YetiGrab && enemy.cell==Cell{enemy.counter_a,enemy.counter_b}) add(enemy.point_b);
        break;
    case EntityKind::AvalancheRam:
        if ((enemy.label_a == RamPaw || enemy.label_a == RamLunge) && enemy.cell == enemy.point_a)
            for (int i=1;i<=enemy.counter_a;++i)
                add(enemy.cell + Cell{enemy.point_b.x*i,enemy.point_b.y*i});
        break;
    case EntityKind::SealThief:
        if (enemy.label_a == SealBark && enemy.cell == enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::FishingWidow:
        if (enemy.label_a == WidowWindup && enemy.cell == enemy.point_a)
            for (int i=1;i<=enemy.counter_a;++i)
                add(enemy.cell + Cell{enemy.facing.x*i,enemy.facing.y*i});
        break;
    case EntityKind::FrozenPilgrim:
        if (enemy.label_a == PilgrimStrike && enemy.cell == enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::EchoHound:
        if (enemy.label_a == EchoBite && enemy.cell == Cell{enemy.counter_a, enemy.counter_b}) add(enemy.point_b);
        break;
    case EntityKind::MirrorKnight:
        if (enemy.label_a == KnightSwing && enemy.cell == enemy.point_a) add(enemy.point_b);
        break;
    case EntityKind::SnowBurrower:
        if (enemy.label_a == SnowWarn) add(enemy.point_b);
        break;
    case EntityKind::IceMason:
        if (enemy.label_a == MasonSwing) add(enemy.point_b);
        break;
    case EntityKind::SteamLeech:
        if (enemy.label_a == LeechSwell)
            for (Cell side : {Cell{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}}) add(enemy.cell + side);
        break;
    case EntityKind::BellDiver:
        if (enemy.label_a == DiverSwing) add(enemy.point_b);
        break;
    case EntityKind::BurrowWorm:
        if (enemy.label_a == 0 && enemy.label_b == 1) add(enemy.point_a);
        break;
    case EntityKind::Wasp:
        if (enemy.label_a == 1) add(enemy.point_a);
        break;
    case EntityKind::ForagerGoblin:
        if (enemy.label_b == 1) add({enemy.counter_a, enemy.counter_b});
        break;
    case EntityKind::RootTurret:
        if (enemy.label_a == 1)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x*i, enemy.facing.y*i});
        break;
    case EntityKind::Woodpecker:
        if (enemy.label_a == 1 || enemy.label_a == 2)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x*i, enemy.facing.y*i});
        break;
    case EntityKind::Owl: case EntityKind::Mosquito:
        if (enemy.label_a == 1 || enemy.label_a == 2) add(enemy.point_b);
        break;
    case EntityKind::RimeSkater:
        if (enemy.label_a == 1 || enemy.label_a == 2)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.point_b.x * i, enemy.point_b.y * i});
        break;
    case EntityKind::Boar:
        if (enemy.label_a == 1 || enemy.label_a == 2)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x * i, enemy.facing.y * i});
        break;
    case EntityKind::Bear:
        if (enemy.label_a == 1) {
            const Cell side{-enemy.facing.y, enemy.facing.x};
            for (int reach = 1; reach <= 2; ++reach)
                for (int lane = -1; lane <= 1; ++lane)
                    add(enemy.cell + Cell{enemy.facing.x * reach + side.x * lane,
                                          enemy.facing.y * reach + side.y * lane});
        }
        break;
    case EntityKind::SporeToad: case EntityKind::LanternMoth:
        if (enemy.label_a == 1) {
            const int radius = enemy.kind == EntityKind::SporeToad ? 2 : 1;
            for (int y = -radius; y <= radius; ++y)
                for (int x = -radius; x <= radius; ++x)
                    if (std::abs(x) + std::abs(y) <= radius) add(enemy.cell + Cell{x, y});
            attack.sleep = true;
        }
        break;
    case EntityKind::BrambleGuard: case EntityKind::Wolf: case EntityKind::CrateMimic:
        if (enemy.label_a == 1) add(enemy.point_b);
        break;
    case EntityKind::FrostBat:
        if (enemy.label_a == 1)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x * i, enemy.facing.y * i});
        break;
    case EntityKind::Bat:
        if (enemy.label_a == 1 || enemy.label_a == 2)
            for (int i = 1; i <= enemy.counter_a; ++i)
                add(enemy.cell + Cell{enemy.facing.x * i, enemy.facing.y * i});
        break;
    default: break;
    }
    return attack;
}

static bool trace_sight(const Game& game, Cell from, Cell to, bool smoke_blocks, bool solid_target,bool through_grates=false,bool solid_terrain_target=false) {
    if (smoke_blocks && obscures_sight(game.stage.at_or_border(from).surface)) return false;
    // GRID RAY: Corner gaps must not leak dust or a creature's line of sight.
    const int dx = to.x - from.x, dy = to.y - from.y;
    const int nx = std::abs(dx), ny = std::abs(dy);
    const Cell sx{dx > 0 ? 1 : -1, 0}, sy{0, dy > 0 ? 1 : -1};
    int ix = 0, iy = 0;
    Cell cell = from;
    const auto open = [&game, smoke_blocks, solid_target, through_grates, solid_terrain_target, to](Cell at) {
        const Tile* tile = game.stage.at(at);
        if (tile == nullptr || (!walkable(tile->kind) && !(solid_terrain_target && at==to)) ||
            (prop_blocks(tile->prop) && !(through_grates && prop_shoot_through(tile->prop)) && !(solid_target && at == to)) ||
            (smoke_blocks && obscures_sight(tile->surface))) return false;
        // FIXTURES: Closed doors and anchored blockers interrupt sight through a corridor.
        if (at != to) {
            const int actor = entity_at(game, at, true);
            if (actor >= 0 && game.entities[static_cast<std::size_t>(actor)].hard_blocker) return false;
        }
        return true;
    };
    while (ix < nx || iy < ny) {
        const int horizontal = (1 + 2 * ix) * ny, vertical = (1 + 2 * iy) * nx;
        if (horizontal == vertical) {
            if (!open(cell + sx) || !open(cell + sy)) return false;
            cell = cell + sx + sy; ++ix; ++iy;
        } else if (horizontal < vertical) { cell = cell + sx; ++ix; }
        else { cell = cell + sy; ++iy; }
        if (!open(cell)) return false;
    }
    return true;
}

bool clear_sight(const Game& game, Cell from, Cell to, bool smoke_blocks) {
    return trace_sight(game, from, to, smoke_blocks, false,true);
}

bool clear_shot_sight(const Game& game, Cell from, Cell to, bool smoke_blocks) {
    return trace_sight(game,from,to,smoke_blocks,true,true);
}

bool clear_attack_sight(const Game& game, Cell from, Cell to, bool smoke_blocks) {
    return trace_sight(game, from, to, smoke_blocks, true);
}

bool clear_heat_sight(const Game& game, Cell from, Cell to) {
    return trace_sight(game,from,to,false,true,false,true);
}

void resolve_enemy_attack(Game& game, int slot, int damage, SoundId sound, int sleep) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    const EnemyAttack attack = enemy_attack(enemy);
    emit_sound(game, sound, enemy.cell);
    enemy.use_flash = 10;
    // COVER: Decide visibility before damage. A broken front prop still shelters
    // cells behind it from the remainder of this same committed attack.
    std::array<bool, 64> exposed{};
    for (int i = 0; i < attack.count; ++i)
        exposed[static_cast<std::size_t>(i)] = clear_attack_sight(game, enemy.cell,
            attack.cells[static_cast<std::size_t>(i)], false);
    for (int i = 0; i < attack.count; ++i) {
        const Cell cell = attack.cells[static_cast<std::size_t>(i)];
        if (!exposed[static_cast<std::size_t>(i)]) continue;
        if (damage > 0) hit_prop(game, cell, damage, enemy.cell);
        for (int target_slot = 0; target_slot < max_entities; ++target_slot) {
            Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
            if (target_slot == slot || target.health <= 0 || target.cell != cell) continue;
            if (damage > 0) damage_entity(game, target_slot, damage, enemy.cell);
            if (sleep > 0 && target.health > 0 && target.impassable && !target.hard_blocker)
                apply_sleep(target, sleep);
        }
    }
}

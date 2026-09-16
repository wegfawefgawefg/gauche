#include "boiler_feed.hpp"
#include "boiler_tank.hpp"
#include "../items/sled.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

namespace {
Cell toward(Cell from,Cell to) {return {(to.x-from.x)/3,(to.y-from.y)/3};}
bool source_ready(const Game& game,const BoilerFeed& feed) {
    return game.stage.at_or_border(feed.source).kind==TileKind::Spring;
}
bool live_pipe(const Tile& tile) {
    return walkable(tile.kind) && tile.prop.kind==PropKind::WaterPipe && !tile.prop.broken && tile.prop.hp>0;
}
void load_coal(Game& game,Entity& tank,const BoilerFeed& feed) {
    if (game.tick%30!=0 || feed.water==0 || feed.dry_ticks>0 || tank.cell!=feed.mount ||
        tank.counter_b>boiler_fuel_limit-1200) return;
    for (int i=0;i<max_entities;++i) {
        Entity& cargo=game.entities[static_cast<std::size_t>(i)];
        if (cargo.kind!=EntityKind::GroundItem || cargo.cell!=feed.delivery ||
            cargo.ground_item.kind!=ItemKind::CoalLump || cargo.ground_item.count<=0 ||
            cargo.ground_item.flight.slot>=0 || cargo.ground_item.anchor.slot>=0 || cargo.toss.ticks>0 || sled_cargo(game,cargo)) continue;
        tank.counter_b+=1200;
        if (--cargo.ground_item.count==0) remove_entity(game,{i,cargo.generation});
        emit_sound(game,SoundId::CoalFeed,tank.cell);return;
    }
}
void draw_water(Game& game,BoilerFeed& feed) {
    if (!source_ready(game,feed) || game.tick%30!=0) return;
    const Cell direction=toward(feed.source,feed.mount);
    for (int i=1;i<=2;++i) {
        const Cell cell=feed.source+Cell{direction.x*i,direction.y*i};
        const Tile& tile=game.stage.at_or_border(cell);
        if (!live_pipe(tile) || tile.prop.hp<prop_spec(PropKind::WaterPipe).health) {
            // A real spring supplies the leak; downstream broken pipes stay dry.
            pour_surface(game,cell,LiquidKind::Water,90);
            if (game.tick%90==0) emit_sound(game,SoundId::PipeLeak,cell);
            if (!live_pipe(tile)) return;
        }
    }
    const Entity* tank=get_entity(game,feed.tank);
    if (tank && tank->cell==feed.mount)
        feed.water=static_cast<std::uint16_t>(std::min(boiler_water_limit,static_cast<int>(feed.water)+60));
}
void dry_fire(Game& game,Entity& tank,BoilerFeed& feed) {
    tank.counter_a=0;tank.label_a=BoilerIdle;tank.timer_a=0;
    if (tank.counter_b==0 || surface_wet(game.stage.at_or_border(tank.cell))) {feed.dry_ticks=0;return;}
    if (feed.dry_ticks==0) emit_sound(game,SoundId::BoilerDryWarn,tank.cell);
    ++feed.dry_ticks;
    if (feed.dry_ticks<180) return;
    // The remaining fuel is lost in one warned firebox failure. Water/coolant
    // or a repaired intake during the warning can prevent it; no free fuel loop.
    tank.counter_b=0;feed.dry_ticks=0;
    pour_surface(game,tank.cell,LiquidKind::Oil,240);ignite_surface(game,tank.cell);
    emit_sound(game,SoundId::BoilerDryFire,tank.cell);
}
}

const BoilerFeed* boiler_feed(const Game& game,const Entity& tank) {
    if (tank.kind!=EntityKind::BoilerTank) return nullptr;
    for (const auto& feed:game.boiler_feeds) if (get_entity(game,feed.tank)==&tank) return &feed;
    return nullptr;
}
BoilerFeed* boiler_feed(Game& game,const Entity& tank) {
    for (auto& feed:game.boiler_feeds) if (get_entity(game,feed.tank)==&tank) return &feed;
    return nullptr;
}
bool boiler_feed_connected(const Game& game,const BoilerFeed& feed) {
    const Entity* tank=get_entity(game,feed.tank);
    if (!tank || tank->health<=0 || tank->cell!=feed.mount || !source_ready(game,feed)) return false;
    const Cell direction=toward(feed.source,feed.mount);
    for (int i=1;i<=2;++i)
        if (!live_pipe(game.stage.at_or_border(feed.source+Cell{direction.x*i,direction.y*i}))) return false;
    return true;
}
bool step_boiler_feed(Game& game,Entity& tank) {
    BoilerFeed* feed=boiler_feed(game,tank);if (!feed) return false;
    draw_water(game,*feed);load_coal(game,tank,*feed);
    if (tank.counter_b>0) {
        --tank.counter_b;
        if (feed->water>0) {
            const int loss=tank.health<tank.max_health && tank.timer_b==0 ? 3 : 1;
            feed->water=static_cast<std::uint16_t>(std::max(0,static_cast<int>(feed->water)-loss));
            if (loss>1 && game.tick%60==0)
                pour_surface(game,tank.cell+toward(feed->mount,feed->source),LiquidKind::Water,90);
        }
        if (feed->water>0) {
            feed->dry_ticks=0;
            if (game.tick%3==0) tank.counter_a=std::min(100,tank.counter_a+1);
        } else dry_fire(game,tank,*feed);
    } else {
        feed->dry_ticks=0;
        if (game.tick%6==0) tank.counter_a=std::max(0,tank.counter_a-1);
    }
    // A dry vessel cannot produce a free water jet from old pressure.
    if (feed->water==0) {tank.counter_a=0;tank.label_a=BoilerIdle;tank.timer_a=0;}
    return true;
}
bool consume_boiler_water(Game& game,const Entity& tank,bool rupture) {
    BoilerFeed* feed=boiler_feed(game,tank);if (!feed) return true;
    const int amount=std::min(static_cast<int>(feed->water),rupture ? boiler_water_limit : 120);
    feed->water=static_cast<std::uint16_t>(feed->water-amount);return amount>0;
}
bool repair_water_pipe(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(tile->kind) || tile->prop.kind!=PropKind::WaterPipe ||
        tile->prop.hp>=prop_spec(PropKind::WaterPipe).health) return false;
    tile->prop.hp=static_cast<std::uint8_t>(std::min(prop_spec(PropKind::WaterPipe).health,static_cast<int>(tile->prop.hp)+20));
    tile->prop.broken=false;return true;
}
bool valid_boiler_feeds(const Game& game) {
    if (game.boiler_feeds.size()>4) return false;
    for (std::size_t i=0;i<game.boiler_feeds.size();++i) {
        const auto& feed=game.boiler_feeds[i];
        if (feed.tank.slot<0 || feed.tank.slot>=max_entities || feed.tank.generation==0 ||
            !game.stage.at(feed.mount) || !game.stage.at(feed.source) || !game.stage.at(feed.delivery) ||
            distance(feed.mount,feed.source)!=3 || (feed.mount.x!=feed.source.x && feed.mount.y!=feed.source.y) ||
            distance(feed.delivery,feed.mount)!=1 || feed.water>boiler_water_limit || feed.dry_ticks>=180) return false;
        const Entity* tank=get_entity(game,feed.tank);
        if (tank && tank->kind!=EntityKind::BoilerTank) return false;
        for (std::size_t j=0;j<i;++j) if (game.boiler_feeds[j].tank==feed.tank) return false;
    }
    return true;
}

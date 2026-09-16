#include "boiler_drive.hpp"
#include "boiler_tank.hpp"
#include "../props/conveyor.hpp"
#include <algorithm>

const BoilerFeed* belt_supply(const Game& game,Cell cell) {
    for (const auto& feed:game.boiler_feeds)
        if (std::find(feed.belts.begin(),feed.belts.end(),cell)!=feed.belts.end()) return &feed;
    return nullptr;
}
bool live_steam_drive(const Game& game,const BoilerFeed& feed) {
    if (feed.belts.empty()) return false;
    const auto& tile=game.stage.at_or_border(feed.drive);
    return walkable(tile.kind) && tile.prop.kind==PropKind::SteamDrive && tile.prop.hp>0 && !tile.prop.broken;
}
bool steam_drive_powered(const Game& game,const BoilerFeed& feed) {
    const Entity* tank=get_entity(game,feed.tank);
    return live_steam_drive(game,feed) && tank && tank->kind==EntityKind::BoilerTank && tank->health>0 &&
        tank->cell==feed.mount && tank->counter_b>0 && tank->counter_a>=25 && tank->timer_b==0 &&
        tank->label_a==BoilerIdle && feed.water>0 && feed.dry_ticks==0;
}
bool driven_belt_powered(const Game& game,const BoilerFeed& feed,Cell cell) {
    if (!steam_drive_powered(game,feed)) return false;
    const auto found=std::find(feed.belts.begin(),feed.belts.end(),cell);
    if (found==feed.belts.end()) return false;
    // The drive is at the unloading end. A broken or redirected section cuts
    // transmission upstream; its intact downstream section can still clear cargo.
    for (auto it=found;it!=feed.belts.end();++it) {
        const auto& tile=game.stage.at_or_border(*it);
        const Cell next=it+1==feed.belts.end() ? feed.delivery : *(it+1);
        if (!walkable(tile.kind) || !live_belt(tile.prop) || belt_direction(tile.prop)!=next-*it) return false;
    }
    return true;
}
bool cutter_powered(const Game& game,const Entity& cutter) {
    for (const auto& feed:game.boiler_feeds) if (get_entity(game,feed.cutter)==&cutter)
        return !feed.belts.empty() && cutter.cell-cutter.facing==feed.belts.front() &&
            driven_belt_powered(game,feed,feed.belts.front());
    return true; // Unwired debug/standalone cutters retain their own drive.
}
bool valid_boiler_drives(const Game& game) {
    for (std::size_t i=0;i<game.boiler_feeds.size();++i) {
        const auto& feed=game.boiler_feeds[i];
        if (feed.belts.empty()) {
            if (feed.cutter.slot>=0) return false;
            continue;
        }
        if (feed.belts.size()>max_driven_belts || !game.stage.at(feed.drive) ||
            distance(feed.drive,feed.mount)!=1 || feed.drive==feed.delivery ||
            feed.cutter.slot<0 || feed.cutter.slot>=max_entities || feed.cutter.generation==0) return false;
        const Entity* cutter=get_entity(game,feed.cutter);
        if (cutter && cutter->kind!=EntityKind::CoalCutter) return false;
        for (std::size_t j=0;j<i;++j)
            if (game.boiler_feeds[j].drive==feed.drive || game.boiler_feeds[j].cutter==feed.cutter) return false;
        for (std::size_t j=0;j<feed.belts.size();++j) {
            const Cell cell=feed.belts[j],next=j+1==feed.belts.size() ? feed.delivery : feed.belts[j+1];
            if (!game.stage.at(cell) || cell==feed.drive || cell==feed.mount || cell==feed.delivery || distance(cell,next)!=1) return false;
            for (std::size_t k=0;k<j;++k) if (feed.belts[k]==cell) return false;
            for (std::size_t k=0;k<i;++k)
                if (std::find(game.boiler_feeds[k].belts.begin(),game.boiler_feeds[k].belts.end(),cell)!=game.boiler_feeds[k].belts.end()) return false;
        }
    }
    return true;
}

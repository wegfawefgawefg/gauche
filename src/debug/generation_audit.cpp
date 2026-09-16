#include "generation_audit.hpp"
#include "../items/supply.hpp"
#include "../world/population_report.hpp"
#include "../net_codec.hpp"
#include <cstdio>
#include <memory>

namespace {
void add(PlacementCount& total,const PlacementCount& count) {
    total.attempted+=count.attempted;total.placed+=count.placed;
    total.budget_blocked+=count.budget_blocked;total.rejected+=count.rejected;
}
void row(int biome,const char* category,const char* name,const PlacementCount& count,
         int planned=0,int fallbacks=0,int actual=0) {
    std::printf("%d,%s,%s,%d,%d,%d,%d,%d,%d,%d\n",biome,category,name,planned,
        count.attempted,count.placed,count.budget_blocked,count.rejected,fallbacks,actual);
}
}

// Generate ordinary floors only. No gameplay ticks, SDL host, input, or networking.
// Entity IDs refer to EntityKind in game.hpp; actual counts include inhabitants
// created inside scenes, while attempt counters cover budgeted room spawns.
int run_generation_audit() {
    auto game=std::make_unique<Game>(),copy=std::make_unique<Game>();
    std::puts("biome,category,name_or_kind_id,planned,attempted,placed,budget_blocked,rejected,fallbacks,actual");
    for (int biome=1;biome<=2;++biome) {
        PopulationReport totals;
        int shelf_floors=0,shelf_rewards=0,thaw_channels=0,work_halls=0,excavations=0,industrial_links=0,belt_cells=0;
        std::array<int,static_cast<std::size_t>(EntityKind::Count)> bodies{};
        std::array<int,static_cast<std::size_t>(ItemKind::Count)> items{};
        for (int seed=1;seed<=64;++seed) {
            *game={};game->rng=static_cast<std::uint64_t>(seed);game->run.seed=game->rng;
            game->run.floor=biome*4+1+seed%4;*copy=*game;
            PopulationReport report;
            generate_world_floor(*game,FloorLayout::Generated,&report);
            generate_world_floor(*copy,FloorLayout::Generated);
            std::string error;
            if (game_hash(*game)!=game_hash(*copy) || !floor_reachable(*game) ||
                !floor_lock_required(*game) || !decode_game(encode_game(*game),*copy,error) ||
                game_hash(*game)!=game_hash(*copy)) {
                std::fprintf(stderr,"Generation audit failed: biome=%d seed=%d floor=%d %s\n",
                    biome,seed,game->run.floor,error.c_str());return 1;
            }
            work_halls+=report.industry_profile==1;excavations+=report.industry_profile==2;
            industrial_links+=static_cast<int>(report.industrial_links.size());
            for (const auto& link:report.industrial_links) belt_cells+=static_cast<int>(link.belt.size())-1;
            thaw_channels+=static_cast<int>(report.thaw_channels.size());
            shelf_floors+=report.shelf_links>0;shelf_rewards+=static_cast<int>(report.shelf_rewards.size());
            totals.shelf_rooms+=report.shelf_rooms;totals.shelf_links+=report.shelf_links;
            for (const Entity& entity:game->entities) {
                ++bodies[static_cast<std::size_t>(entity.kind)];
                if (entity.kind==EntityKind::GroundItem) ++items[static_cast<std::size_t>(entity.ground_item.kind)];
            }
            for (std::size_t i=0;i<totals.scenes.size();++i) {
                add(totals.scenes[i],report.scenes[i]);
                totals.scenes[i].planned+=report.scenes[i].planned;
                totals.scenes[i].fallbacks+=report.scenes[i].fallbacks;
            }
            for (std::size_t i=0;i<totals.enemies.size();++i) add(totals.enemies[i],report.enemies[i]);
            for (std::size_t i=0;i<totals.supplies.size();++i) add(totals.supplies[i],report.supplies[i]);
        }
        std::array<int,static_cast<std::size_t>(ItemRole::Count)> role_counts{};
        for (std::size_t i=1;i<items.size();++i)
            role_counts[static_cast<std::size_t>(item_supply(static_cast<ItemKind>(i)).role)]+=items[i];
        for (std::size_t i=1;i<role_counts.size();++i)
            row(biome,"loot_role",item_role_name(static_cast<ItemRole>(i)),{},0,0,role_counts[i]);
        row(biome,"geometry","work_hall_floors",{},0,0,work_halls);
        row(biome,"geometry","excavation_floors",{},0,0,excavations);
        row(biome,"geometry","industrial_links",{},0,0,industrial_links);
        row(biome,"geometry","linked_belt_cells",{},0,0,belt_cells);
        row(biome,"geometry","thaw_channels",{},0,0,thaw_channels);
        row(biome,"geometry","shelf_floors",{},0,0,shelf_floors);
        row(biome,"geometry","shelf_rooms",{},0,0,totals.shelf_rooms);
        row(biome,"geometry","shelf_connections",{},0,0,totals.shelf_links);
        row(biome,"geometry","crossing_rewards",{},0,0,shelf_rewards);
        for (std::size_t i=0;i<totals.scenes.size();++i) {
            const auto& count=totals.scenes[i];
            if (count.planned) row(biome,"room",room_name(static_cast<RoomRole>(i)),count,count.planned,count.fallbacks);
        }
        for (std::size_t i=1;i<totals.enemies.size();++i) {
            const auto& count=totals.enemies[i];
            if (!bodies[i] && !count.attempted && !count.budget_blocked) continue;
            char id[24];std::snprintf(id,sizeof(id),"%zu",i);
            row(biome,"entity",id,count,0,0,bodies[i]);
        }
        for (std::size_t i=1;i<totals.supplies.size();++i) {
            const auto& count=totals.supplies[i];
            if (items[i] || count.attempted || count.budget_blocked)
                row(biome,"ground_item",item_name(static_cast<ItemKind>(i)),count,0,0,items[i]);
        }
    }
    std::fprintf(stderr,"128 ordinary floors: repeatability, report neutrality, routes, locks and snapshots passed.\n");
    return 0;
}

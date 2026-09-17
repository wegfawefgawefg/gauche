#include "game.hpp"
#include "items/supply.hpp"
#include "items/catalog.hpp"
#include "world/population_report.hpp"
#include <cstdio>
#include <memory>

int main() {
    FILE* catalog=std::fopen("item-catalog.tsv","w");
    std::fprintf(catalog,"item\trole\tstage\tbundle\tdamage\tcooldown_ticks\tdurability\tuses\tloaded\tspare\tdescription\n");
    for(unsigned i=1;i<static_cast<unsigned>(ItemKind::Count);++i) {
        const auto kind=static_cast<ItemKind>(i);const auto item=make_item(kind);const auto p=item_pattern(kind);const auto* def=regional_item(kind);
        std::fprintf(catalog,"%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n",item_name(kind),item_role_name(item_supply(kind).role),item_supply(kind).stage,supply_count(kind),p.damage,p.cooldown,item.durability,item.uses,item.loaded,item.spare,def ? def->description : "");
    }
    std::fclose(catalog);
    auto g=std::make_unique<Game>();
    std::puts("floor,seed,source,item,quantity,durability,uses,loaded,spare,x,y");
    for(int floor=1;floor<=4;++floor)for(unsigned seed=1;seed<=256;++seed) {
        *g={};g->run.floor=floor;g->rng=g->run.seed=seed;player_state(*g,0).online=true;
        generate_world_floor(*g,FloorLayout::Generated);
        const auto emit=[&](const char* source,const Item& item,Cell c) {
            if(item.kind==ItemKind::None || item.count<=0)return;
            std::printf("%d,%u,%s,%s,%d,%d,%d,%d,%d,%d,%d\n",floor,seed,source,item_name(item.kind),item.count,item.durability,item.uses,item.loaded,item.spare,c.x,c.y);
        };
        for(const auto& e:g->entities) {
            if(e.kind==EntityKind::None || e.kind==EntityKind::Player)continue;
            emit(e.kind==EntityKind::GroundItem ? "ground" : "stored",e.ground_item,e.cell);
            for(const auto& item:e.inventory.slots)emit("carried",item,e.cell);
        }
        for(int y=0;y<g->stage.height;++y)for(int x=0;x<g->stage.width;++x) {
            const auto& tile=g->stage.at_or_border({x,y});
            if(tile.contents!=ItemKind::None)emit("wall",make_item(tile.contents,tile.content_count),{x,y});
        }
    }
}

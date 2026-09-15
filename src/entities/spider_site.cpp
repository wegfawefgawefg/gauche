#include "icicle_spider.hpp"
#include "behavior.hpp"

bool find_spider_site(Game& game, int slot) {
    Entity& spider = game.entities[static_cast<std::size_t>(slot)];
    // PERCH: Nearest reachable wall-to-wall crossing, at most five steps from here.
    for (int radius=0;radius<=5;++radius)
        for (int y=-radius;y<=radius;++y)
            for (int x=-radius;x<=radius;++x) {
                const Cell stand = spider.cell+Cell{x,y};
                if (distance(stand,spider.cell) != radius) continue;
                for (Cell direction : {Cell{1,0},{-1,0},{0,1},{0,-1}}) {
                    const Cell anchor = stand-direction;
                    const Tile* wall = game.stage.at(anchor);
                    if (!wall || wall->kind != TileKind::Wall) continue;
                    for (int reach=2;reach<=6;++reach) {
                        const Cell end = stand+Cell{direction.x*reach,direction.y*reach};
                        const Tile* far = game.stage.at(end);
                        if (!far) break;
                        if (far->kind != TileKind::Wall) continue;
                        const Cell old_a = spider.point_a, old_b = spider.point_b;
                        spider.point_a = anchor; spider.point_b = end;
                        const bool valid = spider_span_clear(game,spider,false) &&
                            (stand == spider.cell || next_route_cell(game,slot,stand,192).has_value());
                        if (valid) return true;
                        spider.point_a = old_a; spider.point_b = old_b;
                        break;
                    }
                }
            }
    return false;
}

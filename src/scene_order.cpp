#include "scene_order.hpp"
#include "scenery/roof.hpp"
#include "entities/bell_diver.hpp"
#include <algorithm>
#include <cmath>

namespace {
int entity_priority(const Entity& entity) {
    // Flat fixtures belong below ground flames and all standing bodies.
    if (diver_submerged(entity) || entity.kind==EntityKind::Campfire ||
        entity.kind==EntityKind::Sled || entity.kind==EntityKind::RiverRaft || entity.kind==EntityKind::IceAnchor ||
        entity.kind==EntityKind::PocketDoor || entity.kind==EntityKind::Trap ||
        entity.kind==EntityKind::Exit || entity.kind==EntityKind::Switch ||
        entity.kind==EntityKind::Encounter || entity.kind==EntityKind::WaveVent ||
        entity.kind==EntityKind::EncounterGate) return 0;
    if (entity.kind==EntityKind::GroundItem || entity.kind==EntityKind::Key ||
        entity.kind==EntityKind::Coins) return 1;
    return 3;
}
}

std::vector<BodyDraw> body_draw_order(const Game& game,ViewCamera camera,float zoom,ScenePass pass) {
    std::vector<BodyDraw> order;
    for (std::size_t slot=0;slot<game.entities.size();++slot) {
        const auto& entity=game.entities[slot];
        if (entity.kind==EntityKind::None || entity.kind==EntityKind::RailLayer ||
            (entity.kind==EntityKind::Door && entity.fixture_open)) continue;
        const int priority=entity_priority(entity);
        if ((priority==0)!=(pass==ScenePass::Ground)) continue;
        order.push_back({slot,entity.cell,BodyKind::Entity,priority});
    }
    if (pass==ScenePass::Bodies) {
        const int rx=static_cast<int>(320/tile_pixels(zoom))+6,ry=static_cast<int>(180/tile_pixels(zoom))+7;
        const int cx=static_cast<int>(std::floor(camera.x)),cy=static_cast<int>(std::floor(camera.y));
        for (int y=std::max(0,cy-ry);y<std::min(game.stage.height,cy+ry);++y)
            for (int x=std::max(0,cx-rx);x<std::min(game.stage.width,cx+rx);++x) {
                const auto& prop=game.stage.at({x,y})->prop;
                if (!prop.broken && (prop.kind==PropKind::LightTower || prop.kind==PropKind::TallTree ||
                    prop.kind==PropKind::TallMushroom || prop.kind==PropKind::StreetLamp || prop.kind==PropKind::IcePillar))
                    order.push_back({0,{x,y},BodyKind::Prop,2});
            }
        for (std::size_t slot=0;slot<game.stage.roofs.size();++slot) {
            const auto& roof=game.stage.roofs[slot];
            if (!roof.hp) continue;
            const int rows=roof_rows(roof);
            for (int row=0;row<rows;++row) {
                const Cell cell=roof.start+Cell{0,row};
                if (cell.y<cy-ry || cell.y>cy+ry+roof.height) continue;
                // A long roof is a sequence of ground rows. Its front cannot
                // hide a pillar rooted farther south; occupants at a row are
                // still covered by that row and use the normal group reveal.
                order.push_back({slot,cell,BodyKind::RoofRow,4});
            }
        }
    }
    std::stable_sort(order.begin(),order.end(),[](const BodyDraw& a,const BodyDraw& b) {
        return a.cell.y!=b.cell.y ? a.cell.y<b.cell.y : a.priority<b.priority;
    });
    return order;
}

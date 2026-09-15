#include "nail_board.hpp"
#include "action.hpp"
#include "../traps/nail_board.hpp"
#include "../surfaces/interaction.hpp"
#include "../world/water.hpp"

namespace {
constexpr RegionalItem board{"Nail Board",
    "Swing for 10. Secondary lays a 16-damage foot trap ahead; arms in 0.3s. Recover with pickup. Shares condition: swings cost 1, traps 4. Friends count. Burns.",
    Sprite::NailBoard,{1,1,0,10,18,PatternEffect::Damage},
    ItemAction::Melee,11,1,false,0,0,0,0,0,SoundId::NailSwing,35};
bool lay_board(Game& game,int slot) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];Item& item=*user.inventory.held();
    const Cell cell=user.cell+user.facing;const Tile* tile=game.stage.at(cell);
    if (item.cooldown>0 || item.durability<=0 || !wading_actor(user) || !tile || !walkable(*tile) ||
        tile->kind==TileKind::Lava || surface_wet(*tile) || tile->surface.fire_ticks>0 ||
        (tile->prop.kind!=PropKind::None && !tile->prop.broken) || entity_at(game,cell,false)>=0) return false;
    const Handle handle=spawn_entity(game,EntityKind::Trap,cell);Entity* trap=get_entity(game,handle);
    if (!trap) return false;
    trap->ground_item=item;trap->ground_item.cooldown=0;trap->ground_item.opened=false;
    trap->health=item.durability;trap->max_health=item.max_durability;
    trap->sprite=Sprite::NailBoard;trap->timer_a=18;trap->facing=user.facing;
    trap->entity_a={slot,user.generation};trap->owner=user.owner;
    item={};cancel_item_action(user);emit_sound(game,SoundId::NailSet,cell);return true;
}
}
const RegionalItem* nail_board_item(ItemKind kind) {return kind==ItemKind::NailBoard ? &board : nullptr;}
bool step_nail_board_action(Game& game,int slot,const Input& input) {
    Entity& user=game.entities[static_cast<std::size_t>(slot)];Item& item=*user.inventory.held();
    if (item.kind!=ItemKind::NailBoard) return false;
    const bool pressed=input.reload && !item.opened;item.opened=input.reload;
    if (!input.reload) return false; // Primary uses the shared committed melee action.
    cancel_item_action(user);
    if (pressed && !input.cancel_use && !input.drop && !input.interact) lay_board(game,slot);
    return true;
}

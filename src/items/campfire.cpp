#include "campfire.hpp"
#include "cooking.hpp"

bool use_campfire(Game& game,Entity& player,const Entity& fire,bool held_use) {
    return fire.fire_tramples<5 && use_cooking_fire(game,player,fire.cell,held_use);
}

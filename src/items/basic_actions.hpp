#pragma once
#include "catalog.hpp"
inline constexpr std::array basic_actions{ItemKind::Fist,ItemKind::Slap,ItemKind::ParryPan,
    ItemKind::Jump,ItemKind::Grapple,ItemKind::Shove,ItemKind::Kick,ItemKind::Elbow,ItemKind::GodFist};
bool is_basic_action(ItemKind kind);
const RegionalItem* basic_action_item(ItemKind kind);
void set_basic_action(Entity& player, ItemKind kind);
bool use_basic_action(Game& game,int slot,Cell direction);
void basic_contact(Game& game,Entity& user,Cell cell);
void step_basic_state(Game& game,int slot);
bool basic_airborne(const Entity& actor);
float basic_jump_height(const Entity& actor);
bool balloon_floating(const Entity& actor);
void release_grapple(Game& game,Entity& user,bool throwing=false,Cell direction={});
bool use_grapple(Game& game,int slot,Cell direction);
void step_grapple(Game& game,int slot);

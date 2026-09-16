#pragma once
#include "../game.hpp"
enum GnomeRole { GnomeStick, GnomeBow, GnomeCrossbow, GnomeRider };
enum GnomePhase { GnomeCalm, GnomeFlee, GnomeHidden, GnomeTell, GnomeRest };
void init_gnome(Entity& gnome);
void set_gnome_role(Entity& gnome,GnomeRole role);
void init_gnome_house(Entity& house);
void step_gnome(Game& game,int slot);
void step_gnome_house(Game& game,int slot);
void hurt_gnome_settlement(Game& game,int slot,Cell from);
void gnome_timers(Entity& gnome);
Sprite gnome_pose(const Entity& gnome,int phase=0);
bool hidden_gnome(const Entity& gnome);

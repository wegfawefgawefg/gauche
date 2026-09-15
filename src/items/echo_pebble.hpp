#pragma once
#include "catalog.hpp"
#include <array>

struct EchoVoice { SoundId sound; const char* name; int radius; };
inline constexpr std::array<EchoVoice,20> echo_voices{{
    EchoVoice{SoundId::PistolShot,"Pistol",10}, {SoundId::MusketShot,"Musket",10},
    {SoundId::ShotgunShot,"Shotgun",10}, {SoundId::SmgShot,"SMG",10},
    {SoundId::BlunderShot,"Blunderbuss",10}, {SoundId::RocketLaunch,"Rocket",10},
    {SoundId::LensFire,"Lens carbine",10}, {SoundId::CrossbowShot,"Crossbow",7},
    {SoundId::BowRelease,"Bow",7}, {SoundId::FistWindup,"Fist",4},
    {SoundId::StickWindup,"Stick",4}, {SoundId::PickaxeWindup,"Pickaxe",4},
    {SoundId::AxeSwing,"Hatchet",4}, {SoundId::SpearThrust,"Spear",4},
    {SoundId::MaulSwing,"Maul",4}, {SoundId::RakeSweep,"Rake",4},
    {SoundId::KnifeStab,"Knife",4}, {SoundId::SkateSlash,"Skate blade",4},
    {SoundId::TorchSwing,"Torch",4}, {SoundId::ChiselJab,"Chisel",4},
}};
const EchoVoice* echo_voice(const Item& item);
const RegionalItem* echo_pebble_item(ItemKind kind);
void record_echo_pebbles(Game& game, SoundId sound, Cell source);
bool launch_echo_pebble(Game& game, int owner, const Item& item, Cell facing);
void step_echo_pebble(Game& game, int slot);

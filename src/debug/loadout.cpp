#include "../items/basic_actions.hpp"
#include "../artifacts/powers.hpp"
#include "../items/heated_water.hpp"
#include "../items/pocket_pump.hpp"
#include "../items/muffling.hpp"
#include "../items/echo_pebble.hpp"
#include "playtest.hpp"
#include "../item_attribute.hpp"
#include "../items/storm_lantern.hpp"
#include "../props/candle.hpp"
#include "../items/kettle.hpp"
#include "../items/action.hpp"
#include "../artifacts/catalog.hpp"
#include <algorithm>

// TEMPLATES: Keep real item limits and supported modifiers; never persist entity handles.
void normalize_test_item(Item& item) {
    if (item.kind <= ItemKind::None || item.kind >= ItemKind::Count) { item = {}; return; }
    if (!item_accepts_attribute(item.kind, item.attribute)) item.attribute = ItemAttribute::None;
    Item fresh = make_item(item.kind, 1, item.attribute);
    Entity technician;technician.powers[static_cast<std::size_t>(ArtifactKind::Technical)]=std::min(item.technical_level,max_power_stacks);
    improve_pickup(technician,fresh);
    fresh.count = std::clamp(item.count, 1, std::max(1, fresh.max_count));
    if (fresh.max_durability) fresh.durability = std::clamp(item.durability, 1, fresh.max_durability);
    if (fresh.max_uses) fresh.uses = std::clamp(item.uses, 1, fresh.max_uses);
    if (item_is_gun(item.kind)) {
        fresh.loaded = std::clamp(item.loaded, 0, make_item(item.kind).loaded);
        fresh.spare = std::clamp(item.spare, 0, 999);
    }
    if (item.kind==ItemKind::Balloon) fresh.loaded=std::clamp(item.loaded,0,3600);
    if (item.kind==ItemKind::LunchTin) fresh.loaded=std::clamp(item.loaded,0,2);
    if (item.kind==ItemKind::GlowSlag) fresh.loaded=std::clamp(item.loaded,0,1200);
    if (item.kind==ItemKind::NozzleElbow) fresh.loaded=std::clamp(item.loaded,0,1);
    if (item.kind==ItemKind::PocketPump) {
        fresh.loaded=valid_pocket_pump(item) ? item.loaded : 0;
        fresh.spare=valid_pocket_pump(item) ? item.spare : 0;
    }
    if (item.kind == ItemKind::HeatSiphon) fresh.loaded=std::clamp(item.loaded,0,1800);
    if (item.kind == ItemKind::StormLantern) {
        fresh.loaded = std::clamp(item.loaded, 0, lantern_fuel_ticks);
        fresh.opened = item.opened && fresh.loaded > 0;
        fresh.light.shape = item.light.shape == LightShape::Beam ? LightShape::Beam : LightShape::Cone;
    }
    if (item.kind == ItemKind::EchoPebble) {
        if (const EchoVoice* voice = echo_voice(item)) {
            fresh.loaded = static_cast<int>(voice->sound)+1; fresh.spare = voice->radius;
        }
    }
    if (item.kind == ItemKind::CandleStub) {
        fresh.loaded = std::clamp(item.loaded, 0, candle_fuel_ticks);
        fresh.opened = item.opened || fresh.loaded < candle_fuel_ticks;
        if (fresh.opened) fresh.count = 1;
    }
    if (heated_water_item(item.kind)) {
        fresh.loaded = std::clamp(item.loaded, 0, 2);
        fresh.spare = fresh.loaded == 2 ? std::clamp(item.spare, 1, kettle_cool_ticks) : 0;
    }
    if (item.kind == ItemKind::BearTrap) fresh.opened = item.opened;
    if (item.kind == ItemKind::Stick) fresh.flame_ticks = std::clamp(item.flame_ticks, 0, 1800);
    fresh.muffled_uses = muffleable_item(fresh) ? std::min<std::uint8_t>(item.muffled_uses, 6) : 0;
    item = fresh;
}

// PRESETS: Representative test kits, not guaranteed progression rewards.
void set_loadout_preset(TestLoadout& kit, int preset) {
    kit = {};
    constexpr ItemKind kits[][6]{
        {ItemKind::Fist, ItemKind::Bandage, ItemKind::Stick},
        {ItemKind::Bow, ItemKind::Buckler, ItemKind::Pickaxe, ItemKind::CookedMeat},
        {ItemKind::Bow, ItemKind::Hatchet, ItemKind::Torch, ItemKind::WoolWrap, ItemKind::HotBroth, ItemKind::Ammo},
        {ItemKind::Shotgun, ItemKind::SkateBlade, ItemKind::ColdFlask, ItemKind::StormLantern, ItemKind::Medkit, ItemKind::Ammo},
        {ItemKind::RocketLauncher, ItemKind::Bomb, ItemKind::Bow, ItemKind::Ammo, ItemKind::Medkit, ItemKind::Pickaxe},
    };
    preset = std::clamp(preset, 0, 4);
    for (int slot = 0; slot < quick_slots; ++slot) {
        const auto kind = kits[preset][slot];
        kit.inventory.slots[static_cast<std::size_t>(slot)] = make_item(kind);
        Item& item = kit.inventory.slots[static_cast<std::size_t>(slot)];
        if (item.max_count > 1) item.count = std::min(3, item.max_count);
    }
    kit.health = 100 + preset * 20;
    kit.step_ticks = preset >= 2 ? 6 : 7;
    kit.gold = preset * 25;
    if (preset >= 1) kit.artifacts |= 1U << static_cast<unsigned int>(ArtifactKind::Hearth);
    if (preset >= 3) kit.artifacts |= 1U << static_cast<unsigned int>(ArtifactKind::Reflector);
}

void apply_test_loadout(Game& game, int owner) {
    Entity* player = get_entity(game, player_state(game, owner).controlled);
    if (player == nullptr || player->health <= 0) return;
    const auto& kit = playtest_tools().loadout;
    // REPLACE: Remove owned in-flight/placed tools before discarding their references.
    cancel_item_action(*player);
    for (const Item& item : player->inventory.slots) {
        if (get_entity(game, item.flight)) remove_entity(game, item.flight);
        if (get_entity(game, item.anchor)) remove_entity(game, item.anchor);
    }
    player->inventory = kit.inventory;
    for (Item& item : player->inventory.slots) normalize_test_item(item);
    release_grapple(game,*player,false,player->facing);
    player->basic={};player->powers=kit.powers;player->artifacts = kit.artifacts;
    player->action_fraction=player->move_fraction=player->regen_progress=0;player->action_steps=1;
    set_basic_action(*player,kit.basic_action);
    for (auto& item:player->inventory.slots) improve_pickup(*player,item);
    player->health = player->max_health = kit.health+20*static_cast<int>(artifact_count(*player,ArtifactKind::Vitality));
    player->move_interval = kit.step_ticks;
    player->move_wait = player->attack_wait = player->block_ticks = 0;
    player_state(game, owner).coins = kit.gold;
}

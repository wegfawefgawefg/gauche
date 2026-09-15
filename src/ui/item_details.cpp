#include "../items/echo_pebble.hpp"
#include "../combat/parry.hpp"
#include "../items/fire_render.hpp"
#include "item_details.hpp"
#include "../items/action.hpp"
#include "../items/catalog.hpp"
#include "../items/fish.hpp"
#include "../items/muffling.hpp"
#include "../items/woodland_tools.hpp"
#include "../item_pattern.hpp"
#include "../item_attribute.hpp"
#include "pattern_diagram.hpp"
#include "item_meter.hpp"
#include "text.hpp"
#include "prompts.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>

namespace {

void plate(SDL_Renderer* renderer, float x, float y, float width, float height,
           SDL_FColor color) {
    const SDL_Vertex vertices[4]{
        {{x + 4.0F, y}, color, {}},
        {{x + width + 4.0F, y}, color, {}},
        {{x + width, y + height}, color, {}},
        {{x, y + height}, color, {}},
    };
    constexpr int indices[]{0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
}

void text(SDL_Renderer* renderer, float x, float y, std::string_view words,
          std::uint8_t red = 235, std::uint8_t green = 230,
          std::uint8_t blue = 214) {
    small_ui_text(renderer, x, y, words, red, green, blue);
}

void wrapped(SDL_Renderer* renderer, float x, float y, int columns,
             int lines, std::string_view words) {
    for (int line = 0; line < lines && !words.empty(); ++line) {
        std::size_t count = std::min(words.size(), static_cast<std::size_t>(columns));
        if (count < words.size()) {
            const std::size_t gap = words.rfind(' ', count);
            if (gap != std::string_view::npos && gap > 0) count = gap;
        }
        text(renderer, x, y + static_cast<float>(line) * 9.0F,
             words.substr(0, count), 194, 192, 180);
        words.remove_prefix(count);
        while (!words.empty() && words.front() == ' ') words.remove_prefix(1);
    }
}

} // namespace

const char* item_description(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->description;
    switch (kind) {
    case ItemKind::Wall: return "Build a wall on nearby ground. It blocks paths and fire.";
    case ItemKind::Medkit: return "Restore up to 100 health. One use consumes one kit.";
    case ItemKind::Bandage: return "Restore 10 health. Quick, modest field medicine.";
    case ItemKind::Bandaid: return "Restore 1 health when there is time to spare.";
    case ItemKind::Fist: return "A direct punch into the next tile. Always available.";
    case ItemKind::ConductorHat: return "Lay track and call a train that tears through walls.";
    case ItemKind::Buckler: return "Block a hit, then shove the actor in front of you.";
    case ItemKind::Pistol: return "A reliable short-cooldown shot along a straight line.";
    case ItemKind::Musket: return "A loud, powerful single shot. Slow to reload.";
    case ItemKind::Bow: return "Hold to draw; release an arrow. No reload. Damage arrives with the arrow.";
    case ItemKind::RocketLauncher: return "A rocket travels straight, then blasts the impact area.";
    case ItemKind::Ammo: return "Supply each gun and bow separately, including the held weapon.";
    case ItemKind::Bomb: return "Throw forward. A 2.5s fuse starts on use, then it explodes. Get clear!";
    case ItemKind::SleepMeds: return "Put a nearby target to sleep for a short time.";
    case ItemKind::Stick: return "Hit harder than a fist. Light at a campfire for 30s of fire strikes; water puts it out.";
    case ItemKind::Shotgun: return "Powerful close-range shot with a slow recovery.";
    case ItemKind::SMG: return "Rapid straight shots with a large magazine.";
    case ItemKind::BearTrap: return "Open the jaws first. Then place it; a victim takes 100 damage.";
    case ItemKind::Mine: return "Place an explosive trap on the next clear tile.";
    case ItemKind::Pickaxe: return "Strike the next tile and break weak rock quickly.";
    case ItemKind::RawMeat: return "Eat for 4 health, or cook it at a campfire.";
    case ItemKind::CookedMeat: return "Eat for 18 health after cooking it at a fire.";
    case ItemKind::None: return "Empty slot.";
    default: break;
    }
    return "";
}

void draw_item_details(SDL_Renderer* renderer, const GameGraphics& graphics,
                       const Entity& player, const Item& item, float x, float y,
                       float width, float height, const char* label, bool highlight) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    plate(renderer, x + 6.0F, y + 7.0F, width, height,
          {0.01F, 0.01F, 0.01F, 0.72F});
    plate(renderer, x, y, width, height,
          {0.075F, 0.085F, 0.09F, 0.96F});
    draw_item_banner(renderer, x, y, width, label, highlight);
    if (item.kind == ItemKind::None) return;
    SDL_FRect icon{x + 9.0F, y + 22.0F, 24.0F, 24.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item)), nullptr, &icon);
    draw_item_flame(renderer, graphics, item, icon, {1, 0}, static_cast<std::uint64_t>(item.flame_ticks));
    draw_muffled_count(renderer, item, x + 18, y + 39);
    text(renderer, x + 39.0F, y + 22.0F, item.flame_ticks > 0 ? "Lit Stick" : item_name(item.kind));
    char line[80];
    if (item_stackable(item))
        std::snprintf(line, sizeof(line), "x%d  %s", item.count, item.cooldown > 0 ? "COOLING" : "READY");
    else std::snprintf(line, sizeof(line), "%s", item.cooldown > 0 ? "COOLING" : "READY");
    if (item.kind==ItemKind::StormLantern) std::snprintf(line,sizeof(line),"%s",item_state_text(item,false).c_str());
    if (item.flight.slot >= 0) std::snprintf(line, sizeof(line), item.kind == ItemKind::HarpoonGun ? "LINE OUT" : "IN FLIGHT");
    if (item.attribute != ItemAttribute::None) {
        text(renderer, x + 39.0F, y + 34.0F,
             item_attribute_name(item.attribute), 218, 169, 94);
        text(renderer, x + 10.0F, y + 48.0F,
             item_attribute_effect(item.attribute), 218, 169, 94);
    } else text(renderer, x + 39.0F, y + 34.0F, line, 186, 189, 174);
    wrapped(renderer, x + 10.0F, y + 59.0F,
            static_cast<int>((width - 20.0F) / 6.0F), 4,
            item_description(item.kind));
    const ItemPattern pattern = item_pattern(item);
    if (pattern.damage > 0 && item.dig_power > 0)
        std::snprintf(line, sizeof(line), "DAMAGE %d  DIG %d", pattern.damage, item.dig_power);
    else if (pattern.damage > 0)
        std::snprintf(line, sizeof(line), "DAMAGE %d", pattern.damage);
    else if (pattern.heal > 0)
        std::snprintf(line, sizeof(line), "HEAL +%d   HP %d/%d", pattern.heal,
                      player.health, player.max_health);
    else std::snprintf(line, sizeof(line), "%s", item.opened ? "OPEN" : "UTILITY");
    if (item.kind == ItemKind::SaltedKelp) {
        if (player.vitals.nausea > 0)
            std::snprintf(line, sizeof(line), "CURE | HP %d -> %d", player.health,
                std::max(0, player.health - kelp_health_cost));
        else std::snprintf(line, sizeof(line), "NO NAUSEA | COST %d HP", kelp_health_cost);
    }
    if (item.kind == ItemKind::BrineFlask)
        std::snprintf(line, sizeof(line), "WOUNDED DMG %d | BRINE 8s", pattern.damage);
    if (pattern.chain)
        std::snprintf(line, sizeof(line), "HITS %d / %d / %d / %d", pattern.damage,
            (pattern.damage * 3 + 3) / 4, (pattern.damage * 2 + 3) / 4, (pattern.damage + 3) / 4);
    if (item.kind == ItemKind::Scarecrow)
        std::snprintf(line, sizeof(line), "WARD %d TILES | PROP HP %d", pattern.blast_radius, prop_spec(PropKind::Scarecrow).health);
    if (item.kind == ItemKind::PocketDoor) {
        if (item.anchor.slot >= 0)
            std::snprintf(line, sizeof(line), "FIRST SET | PLACE SECOND");
        else std::snprintf(line, sizeof(line), "PLACE FIRST | THIS FLOOR");
    }
    if (item.kind == ItemKind::StrawDecoy)
        std::snprintf(line, sizeof(line), "LURE UP TO %d | PROP HP %d", pattern.blast_radius, prop_spec(PropKind::StrawDecoy).health);
    if (item.kind == ItemKind::ReflectingPan)
        std::snprintf(line, sizeof(line), "PARRY %.2fs | COST %d CONDITION",
            static_cast<double>(parry_ticks) / 60, parry_wear);
    if (item.kind == ItemKind::ShieldLantern)
        std::snprintf(line, sizeof(line), "FRONT GUARD 0.25s | LIGHT %d", item.light.radius);
    if (item.kind == ItemKind::StinkBomb)
        std::snprintf(line, sizeof(line), "SCENT 10s | NO DAMAGE");
    if (item.kind == ItemKind::RottenFruit)
        std::snprintf(line, sizeof(line), "NAUSEA 1 HP/s | WATER CURES");
    if (item.kind == ItemKind::WolfWhistle)
        std::snprintf(line, sizeof(line), "DISTRACT 5s | LONE WOLVES ONLY");
    if (item.kind == ItemKind::HandBell)
        std::snprintf(line, sizeof(line), "WAKE + INVESTIGATE 5s");
    if (item.kind == ItemKind::Firecracker)
        std::snprintf(line, sizeof(line), "STUN 0.5s | HEARD 10");
    if (item.kind == ItemKind::ResinGlue) {
        const int repair = resin_repair_slot(player.inventory);
        if (repair < 0) std::snprintf(line, sizeof(line), "NOTHING NEEDS REPAIR");
        else {
            const Item& target = player.inventory.slots[static_cast<std::size_t>(repair)];
            std::snprintf(line, sizeof(line), "%s %d -> %d", item_name(target.kind),
                target.durability, target.max_durability);
        }
    }
    if (item.kind == ItemKind::AirBladder)
        std::snprintf(line, sizeof(line), "SHOVE 1 | FLOAT UP TO 16");
    if (item.kind == ItemKind::HeatCapsule)
        std::snprintf(line, sizeof(line), "WARMTH 4s | THAW + CLEAR CHILL");
    if (item.kind == ItemKind::ColdFlask)
        std::snprintf(line, sizeof(line), "CHILL 3s | FREEZE WATER 8s");
    if (item.kind == ItemKind::IceNeedle)
        std::snprintf(line, sizeof(line), "DMG %d | CHILL 1s", pattern.damage);
    else if (item.kind == ItemKind::GritPouch)
        std::snprintf(line, sizeof(line), "TRACTION | %d ICE CELLS", pattern.half_width * 2 + 1);
    if (item.kind == ItemKind::SnowScoop)
        std::snprintf(line, sizeof(line), "CLEAR %d | SNOW %d/12", pattern.half_width * 2 + 1, item.loaded);
    if (item.kind == ItemKind::MufflingFelt) {
        Inventory order = player.inventory;
        bool carried = false;
        for (int slot = 0; slot < quick_slots; ++slot)
            if (&player.inventory.slots[static_cast<std::size_t>(slot)] == &item) { order.selected = slot; carried = true; }
        const int target = muffling_target_slot(order);
        if (!carried) std::snprintf(line, sizeof(line), "EQUIP TO PREVIEW TARGET");
        else if (target < 0) std::snprintf(line, sizeof(line), "NO UNWRAPPED WEAPON");
        else std::snprintf(line, sizeof(line), "WRAP #%d: %s", target + 1,
            item_name(order.slots[static_cast<std::size_t>(target)].kind));
    }
    if (item.kind == ItemKind::HarpoonGun)
        std::snprintf(line,sizeof(line),"DMG %d | REEL 1 / 0.17s",pattern.damage);
    if (item.kind == ItemKind::EchoPebble) {
        const EchoVoice* voice = echo_voice(item);
        std::snprintf(line,sizeof(line),"RECORD: %s | 3 ECHOES",voice ? voice->name : "Knock");
    }
    if (item.kind==ItemKind::StormLantern) std::snprintf(line,sizeof(line),"FUEL %ds | %s",(item.loaded+59)/60,item.light.shape==LightShape::Beam ? "FOCUSED" : "WIDE");
    if (item.kind == ItemKind::CandleStub)
        std::snprintf(line, sizeof(line), "FUEL %.1fs | PLACED HP %d", static_cast<double>(item.loaded)/60, item.durability);
    if (item.kind == ItemKind::SteamKettle)
        std::snprintf(line,sizeof(line),item.loaded == 0 ? "FILL AT WATER | HEAT 1.5s" :
            item.loaded == 1 ? "COLD: DOUSE | WATER 5s" : "SCALD %d | WATER 5s",pattern.damage);
    if (item.kind == ItemKind::PressureValve) std::snprintf(line,sizeof(line),"AIM LOCK | RECOVER BELOW 25");
    if (item.kind == ItemKind::SignalFlare) std::snprintf(line,sizeof(line),"LIGHT 15s | CONTACT BURNS 5s");
    if (item.kind == ItemKind::SkateBlade)
        std::snprintf(line,sizeof(line),"DMG %d | TIP %s",pattern.damage,player.vitals.slide_momentum>0 ? "ACTIVE" : "AFTER SLIP");
    if (item.kind == ItemKind::Crampons) {
        Entity preview=player; preview.vitals.traction=300;
        std::snprintf(line,sizeof(line),"STEP %d -> %d TICKS | 5s",movement_beat(player,player.move_interval),
            movement_beat(preview,preview.move_interval));
    }
    if (item.kind == ItemKind::Sealant) std::snprintf(line,sizeof(line),"REPAIR 20 HP | PLUG 10s");
    if (item.kind == ItemKind::WickSpool)
        std::snprintf(line, sizeof(line), "CANDLE +30s | CAPACITY 80s");
    if (item.kind == ItemKind::FishingLine)
        std::snprintf(line, sizeof(line), "REELS LOOSE ITEMS | NO DAMAGE");
    if (item.kind == ItemKind::AlarmClock)
        std::snprintf(line, sizeof(line), "PLACED HP %d | DELAY 3s", item.durability);
    if (item.kind == ItemKind::BlackFelt)
        std::snprintf(line, sizeof(line), "BLOCKS LIGHT | BURNS / TEARS");
    if (item.kind == ItemKind::PrismBomb)
        std::snprintf(line, sizeof(line), "DMG %d | FUSE 1.5s | BEAM %d", pattern.damage, pattern.blast_radius);
    if (item.kind == ItemKind::MirrorShard)
        std::snprintf(line, sizeof(line), "PLACED HP 8 | TURNS BEAMS");
    if (item.kind == ItemKind::CrystalLens)
        std::snprintf(line, sizeof(line), "PLACED HP 18 | SPLIT 50%%/50%%");
    if (item.kind == ItemKind::Snowball)
        std::snprintf(line, sizeof(line), "DMG %d | WET PATCH 2s", pattern.damage);
    if (item.kind == ItemKind::CopperWire) std::snprintf(line,sizeof(line),"LINKS WET / WIRED CELLS | 4 HP");
    if (item.kind == ItemKind::GroundingSpike) std::snprintf(line,sizeof(line),"ABSORBS 1 PULSE | COOL 3s");
    if (item.kind == ItemKind::EelBattery)
        std::snprintf(line, sizeof(line), "DMG %d | CIRCUIT %d", pattern.damage, pattern.blast_radius);
    if (item.kind == ItemKind::Chisel)
        std::snprintf(line, sizeof(line), "DMG %d | ICE %d | DIG 1", pattern.damage, pattern.damage * 2);
    if (item.kind == ItemKind::IceBrick)
        std::snprintf(line, sizeof(line), "THROW %d | COVER 35 HP", pattern.damage);
    if (item.kind == ItemKind::WoolWrap)
        std::snprintf(line, sizeof(line), "CHILL GUARD 8s | FLAMMABLE");
    if (item.kind == ItemKind::HotBroth || item.kind == ItemKind::IcePoultice) {
        const double rate = item.kind == ItemKind::HotBroth ? 3.0 : 2.0;
        std::snprintf(line, sizeof(line), "REGEN +%d OVER %.1fs", pattern.heal, pattern.heal / rate);
    }
    if (item.kind == ItemKind::HerbBag)
        std::snprintf(line, sizeof(line), "REGEN +%d OVER %.1fs", pattern.heal,
            static_cast<double>(pattern.heal) / 3);
    if (item.kind == ItemKind::BitterRoot) {
        if (player.health <= 3) std::snprintf(line, sizeof(line), "NEEDS MORE THAN 3 HP");
        else std::snprintf(line, sizeof(line), "HP %d -> %d | GUARD 10s", player.health, player.health - 3);
    }
    if (item.kind == ItemKind::StickyBoots) {
        Entity gripped = player;
        gripped.vitals.grip = 360;
        std::snprintf(line, sizeof(line), "STEP %d -> %d TICKS", movement_beat(player, player.move_interval),
            movement_beat(gripped, gripped.move_interval));
    }
    if (item.kind == ItemKind::Chili) {
        Entity faster = player;
        faster.vitals.haste = 240;
        std::snprintf(line, sizeof(line), "STEP %d -> %d TICKS", movement_beat(player, player.move_interval),
            movement_beat(faster, faster.move_interval));
    }
    text(renderer, x + 10.0F, y + 96.0F, line);
    if (item.flight.slot >= 0) std::snprintf(line, sizeof(line), item.kind == ItemKind::HarpoonGun ? "SECONDARY CUTS THE LINE" : "IN FLIGHT - SLOT RESERVED");
    else std::snprintf(line, sizeof(line), "COOLDOWN %.2f / %.2fs",
                  static_cast<double>(item.cooldown) / 60.0,
                  static_cast<double>(pattern.cooldown) / 60.0);
    text(renderer, x + 10.0F, y + 107.0F, line, 188, 187, 176);
    if (item.kind == ItemKind::SteamKettle)
        std::snprintf(line,sizeof(line),"REUSABLE | NOT STACKABLE");
    else if (item.kind == ItemKind::PressureValve)
        std::snprintf(line,sizeof(line),"ATTACHES | RECOVERABLE");
    else if (item.kind == ItemKind::Bow)
        std::snprintf(line, sizeof(line), "QUIVER %d ARROWS", item.loaded);
    else if (item_is_gun(item.kind))
        std::snprintf(line, sizeof(line), "MAG %d / %d   SPARE %d",
                      item.loaded, item_meter_capacity(item), item.spare);
    else if (item.max_durability > 0)
        std::snprintf(line, sizeof(line), "CONDITION %d / %d",
                      item.durability, item.max_durability);
    else if (item.kind == ItemKind::PocketDoor)
        std::snprintf(line, sizeof(line), "PLACEMENTS %d / %d", item.uses, item.max_uses);
    else if (item.max_uses > 0)
        std::snprintf(line, sizeof(line), "USES %d / %d",
                      item.uses, item.max_uses);
    else if (item_stackable(item))
        std::snprintf(line, sizeof(line), "STACK %d / %d   %s",
                      item.count, item.max_count,
                      item.consume_on_use ? "CONSUMES" : "PERSISTS");
    else std::snprintf(line, sizeof(line), "%s",
                       item.consume_on_use ? "ONE USE - CONSUMES" : "PERSISTENT");
    text(renderer, x + 10.0F, y + 118.0F, line, 194, 192, 180);
    if (item.kind == ItemKind::BorrowedSummer) {
        text(renderer,x+10,y+129,"4s MOVING CROSS | NO IMMUNITY",216,186,117);
    } else if (item.kind == ItemKind::EmergencyDoorstop) {
        text(renderer,x+10,y+129,"PLACED | RECOVERABLE",167,197,199);
    } else if (item.kind==ItemKind::StormLantern) {
        draw_action_hint(renderer,x+10,y+127,Action::Reload,"TOGGLE SHUTTER");
    } else if (item.kind == ItemKind::SteamKettle) {
        text(renderer,x + 10,y + 129,item_state_text(item,false),167,197,199);
    } else if (item.kind == ItemKind::CandleStub) {
        text(renderer, x + 10, y + 129, item_stackable(item) ? "UNUSED: STACKS UP TO 4" : "USED: DOES NOT STACK", 167, 197, 199);
    } else if (item.kind == ItemKind::FishingLine) {
        text(renderer, x + 10, y + 129, "STAY STILL | BLOCKERS CUT LINE", 167, 197, 199);
    } else if (item.kind == ItemKind::AlarmClock) {
        text(renderer, x + 10, y + 129, "RINGS 5s | RECOVER AFTER", 167, 197, 199);
    } else if (item.kind == ItemKind::BlackFelt) {
        draw_action_hint(renderer, x + 10, y + 127, Action::Reload, "TEAR OFF COVER AHEAD");
    } else if (item.kind == ItemKind::MirrorShard) {
        draw_action_hint(renderer, x + 10, y + 127, Action::Reload, "TURN MIRROR AHEAD");
    } else if (item.kind == ItemKind::RottenFruit) {
        draw_action_hint(renderer, x + 10, y + 127, Action::Reload, "EAT +3 HP / NAUSEA 6s");
    } else if (item.kind == ItemKind::SnowScoop) {
        draw_action_hint(renderer, x + 10, y + 127, Action::Reload, item.loaded > 0 ? "PACK SNOWBALL" : "COLLECT SNOW FIRST");
    } else if (item.muffled_uses > 0) {
        std::snprintf(line, sizeof(line), "QUIET USES %u / 6", static_cast<unsigned int>(item.muffled_uses));
        text(renderer, x + 10, y + 129, line, 167, 197, 199);
    } else if (item.flame_ticks > 0) {
        std::snprintf(line, sizeof(line), "FIRE %.1fs  BURN 20 / 5s", static_cast<double>(item.flame_ticks)/60);
        text(renderer, x + 10.0F, y + 129.0F, line, 235, 167, 80);
    } else if (pattern.chain) {
        std::snprintf(line, sizeof(line), "4 HITS | JUMP %d | CIRCUIT 6", pattern.blast_radius);
        text(renderer, x + 10, y + 129, line, 162, 196, 213);
    } else text(renderer, x + 10.0F, y + 129.0F, item_stackable(item) ? "STACKABLE" : "NOT STACKABLE", 162, 171, 159);
    if (item_windup(item) > 0)
        std::snprintf(line, sizeof(line), "WINDUP %.2fs  RANGE %d-%d",
            static_cast<double>(item_windup(item)) / 60.0, pattern.minimum, pattern.maximum);
    else if (item.dig_power > 0)
        std::snprintf(line, sizeof(line), "RANGE %d-%d   DIG %d", pattern.minimum,
                      pattern.maximum, item.dig_power);
    else if (item.kind == ItemKind::AirBladder)
        std::snprintf(line, sizeof(line), "PUSH REACH %d", pattern.blast_radius);
    else if (item.kind == ItemKind::HeatCapsule)
        std::snprintf(line, sizeof(line), "WARM REACH %d", pattern.blast_radius);
    else std::snprintf(line, sizeof(line), "RANGE %d-%d", pattern.minimum, pattern.maximum);
    if (item.kind == ItemKind::PitchBomb)
        std::snprintf(line, sizeof(line), "FUSE 2s | FIRE 6s + RESIN");
    else if (item.kind == ItemKind::RottenFruit)
        std::snprintf(line, sizeof(line), "THROW %d | ROT PATCH 10s", pattern.maximum);
    else if (item.kind == ItemKind::HandBell)
        std::snprintf(line, sizeof(line), "HEARD UP TO %d CELLS", pattern.blast_radius);
    else if (item.kind == ItemKind::Firecracker)
        std::snprintf(line, sizeof(line), "FUSE 1.5s | STARTLE %d", pattern.blast_radius);
    else if (item.kind == ItemKind::ThrowingNet)
        std::snprintf(line, sizeof(line), "RANGE %d | ROOT 2.0s", pattern.maximum);
    else if (item.kind == ItemKind::StickyBoots)
        std::snprintf(line, sizeof(line), "GRIP 6.0s | SLOW STEPS");
    else if (item.kind == ItemKind::RabbitCharm)
        std::snprintf(line, sizeof(line), "RETREAT UP TO 3 CELLS");
    else if (item.kind == ItemKind::AcornMine)
        std::snprintf(line, sizeof(line), "PLACE 1 | SPLINTERS %d", pattern.blast_radius);
    else if (item.kind == ItemKind::RopeSnare)
        std::snprintf(line, sizeof(line), "PLACE 1 | ROOT 3.0s");
    else if (item.kind == ItemKind::SpringTrap)
        std::snprintf(line, sizeof(line), "PLACE 1 | SHOVE 2");
    text(renderer, x + 10.0F, y + 140.0F, line, 194, 192, 180);
    if (height >= 176.0F) {
        text(renderer, x + 10.0F, y + 151.0F, item.kind == ItemKind::IceBrick ? "THROW PATTERN" : item.kind == ItemKind::EelBattery ? "CONTACT + CIRCUIT" : "PATTERN", 185, 185, 172);
        draw_pattern_diagram(renderer, item, x + 10.0F, y + 159.0F,
                             width - 20.0F, height - 169.0F, &player);
    }
}

void draw_compact_item_details(SDL_Renderer* renderer, const GameGraphics& graphics,
                               const Item& item, float x, float y,
                               float width, const char* label) {
    if (item.kind == ItemKind::None) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    plate(renderer, x + 4.0F, y + 4.0F, width, 31.0F,
          {0.01F, 0.01F, 0.01F, 0.72F});
    plate(renderer, x, y, width, 31.0F,
          {0.075F, 0.085F, 0.09F, 0.96F});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_FRect icon{x + 5.0F, y + 7.0F, 17.0F, 17.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item)), nullptr, &icon);
    draw_item_flame(renderer, graphics, item, icon, {1, 0}, static_cast<std::uint64_t>(item.flame_ticks));
    draw_muffled_count(renderer, item, x + 8, y + 22);
    if (item.flame_ticks > 0) {
        char status[64];
        std::snprintf(status, sizeof(status), "%s  FIRE %ds", label, (item.flame_ticks+59)/60);
        text(renderer, x + 28.0F, y + 3.0F, status, 235, 167, 80);
    } else text(renderer, x + 28.0F, y + 3.0F, label, 178, 164, 151);
    text(renderer, x + 28.0F, y + 14.0F, item.flame_ticks > 0 ? "Lit Stick" : item_name(item.kind));
    text(renderer, x + width - 54.0F, y + 14.0F,
         item_state_text(item, true), 200, 207, 189);
    text(renderer, x + width - 54.0F, y + 23.0F,
         item_cooldown_text(item), 217, 183, 128);
}

void draw_item_banner(SDL_Renderer* renderer, float x, float y, float width,
                      const char* label, bool highlight) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    plate(renderer, x-6, y-8, width+8, 18, highlight ?
        SDL_FColor{.56F, .16F, .14F, .98F} : SDL_FColor{.14F, .17F, .15F, .98F});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    text(renderer, x+3, y-5, label);
}

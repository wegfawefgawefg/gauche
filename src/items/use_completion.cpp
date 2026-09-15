#include "use_completion.hpp"
#include "catalog.hpp"
#include "../item_attribute.hpp"
#include "muffling.hpp"
#include "../artifacts/hearth.hpp"

void finish_item_use(Game& game, Entity& user, Item& item, ItemKind used_kind, Cell target,
                     Cell direction, int cooldown) {
    share_hearth_meal(game,user,used_kind);
    if (item_is_melee(used_kind)) finish_muffled_use(game, item, user.cell);
    item.cooldown = cooldown;
    user.use_flash = 8;
    if (const RegionalItem* spec = regional_item(used_kind)) {
        if (used_kind == ItemKind::CandleStub)
            emit_sound(game, item.loaded > 0 ? SoundId::CandleLight : SoundId::Drop, user.cell + direction);
        else if (!item_is_melee(used_kind) && used_kind != ItemKind::CoalLump && used_kind != ItemKind::SteamKettle && used_kind != ItemKind::SteamLance) emit_sound(game, spec->sound,
            used_kind == ItemKind::SnowGlobe ? user.cell + direction : user.cell);
    }
    else switch (used_kind) {
    case ItemKind::Wall: emit_sound(game, SoundId::BlockLand, target); break;
    case ItemKind::Medkit: case ItemKind::Bandage: case ItemKind::Bandaid:
        emit_sound(game, SoundId::ClothRip, user.cell); break;
    case ItemKind::RawMeat: case ItemKind::CookedMeat:
        emit_sound(game, SoundId::MeatMunch, user.cell); break;

    case ItemKind::ConductorHat:
        emit_sound(game, SoundId::DistantTrainSound, user.cell); break;
    case ItemKind::Buckler: emit_sound(game, SoundId::HitBlock1, user.cell); break;
    case ItemKind::SleepMeds: emit_sound(game, SoundId::ClothRip, target); break;
    case ItemKind::BearTrap: case ItemKind::Mine:
        emit_sound(game, SoundId::BlockLand, target); break;
    default: break;
    }
    if (used_kind==ItemKind::NailBoard && --item.durability<=0) {emit_sound(game,SoundId::NailBreak,user.cell);item={};return;}
    if ((used_kind == ItemKind::PressHammer || used_kind == ItemKind::RubberMallet) && --item.durability <= 0) {
        emit_sound(game,SoundId::WoodCrack,user.cell);
        item = {};
        return;
    }
    if ((used_kind == ItemKind::SkateBlade || used_kind == ItemKind::Chisel || used_kind == ItemKind::SnowScoop) && --item.durability <= 0) {
        emit_sound(game, used_kind == ItemKind::SkateBlade ? SoundId::SkateBreak : used_kind == ItemKind::SnowScoop ? SoundId::ScoopBreak : SoundId::ChiselBreak, user.cell);
        item = {};
        return;
    }
    if (item.max_uses > 0 && --item.uses <= 0) {
        if (used_kind == ItemKind::RailSwitchKey) emit_sound(game,SoundId::SwitchKeySpent,user.cell);
        else if (used_kind == ItemKind::MoldKey) emit_sound(game,SoundId::MoldKeySpent,user.cell);
        else if (used_kind == ItemKind::HandBellows) emit_sound(game,SoundId::BellowsSpent,user.cell);
        else if (used_kind == ItemKind::CoolantCan) emit_sound(game,SoundId::CoolantEmpty,user.cell);
        else if (used_kind == ItemKind::HorseshoeMagnet) emit_sound(game,SoundId::MagnetSpent,user.cell);
        else if (used_kind == ItemKind::SnowShelter) emit_sound(game,SoundId::ShelterEmpty,user.cell);
        else if (used_kind == ItemKind::FuseScissors) emit_sound(game,SoundId::ScissorsSpent,user.cell);
        else if (used_kind == ItemKind::ForemanWhistle) emit_sound(game,SoundId::WorkSpent,user.cell);
        else if (used_kind == ItemKind::StillwaterBell) emit_sound(game,SoundId::StillwaterSpent,user.cell);
        else if (used_kind == ItemKind::TuningFork) emit_sound(game,SoundId::ForkSpent,user.cell);
        else if (used_kind == ItemKind::CopperWire) emit_sound(game,SoundId::WireEmpty,user.cell);
        else if (used_kind == ItemKind::Crampons) emit_sound(game,SoundId::CramponsSpent,user.cell);
        else if (used_kind == ItemKind::Sealant) emit_sound(game,SoundId::SealantEmpty,user.cell);
        else if (used_kind == ItemKind::WickSpool) emit_sound(game, SoundId::WickEmpty, user.cell);
        else if (used_kind == ItemKind::FishingLine) emit_sound(game, SoundId::FishingEmpty, user.cell);
        else if (used_kind == ItemKind::MufflingFelt) emit_sound(game, SoundId::MuffleEmpty, user.cell);
        else if (used_kind == ItemKind::EelBattery) emit_sound(game, SoundId::BatteryEmpty, user.cell);
        else if (used_kind == ItemKind::AirBladder) emit_sound(game, SoundId::AirEmpty, user.cell);
        else if (used_kind == ItemKind::GritPouch) emit_sound(game, SoundId::GritEmpty, user.cell);
        else if (used_kind != ItemKind::PocketDoor && used_kind != ItemKind::BorrowedSummer)
            emit_sound(game, SoundId::BoxBreak, user.cell);
        item = {};
        return;
    }
    if (item.consume_on_use && --item.count <= 0) item = {};
}

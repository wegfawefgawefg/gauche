#include "echo_pebble.hpp"
#include "../entities/hearing.hpp"
#include "../projectiles/projectile.hpp"
#include <algorithm>

namespace {
constexpr RegionalItem pebble{"Echo Pebble",
    "Records nearby weapon sounds. Throw to replay three times and lure listeners. Copies sound, not attacks.",
    Sprite::EchoPebble,{1,7,0,0,36,PatternEffect::Utility,true},
    ItemAction::Throw,16,3,true,0,0,0,0,0,SoundId::PebbleThrow};

void record(Item& item, const EchoVoice& voice) {
    if (item.kind != ItemKind::EchoPebble) return;
    item.loaded = static_cast<int>(voice.sound)+1;
    item.spare = voice.radius;
}

void land(Game& game, Entity& shot) {
    shot.counter_a = 0;
    shot.label_b = 1;
    shot.timer_a = 30;
    shot.timer_b = 0;
    shot.sprite = Sprite::EchoPebbleReady;
    emit_sound(game,SoundId::PebbleLand,shot.cell);
}

void replay(Game& game, Entity& shot) {
    const EchoVoice* voice = echo_voice(shot.ground_item);
    make_noise(game,shot.cell,voice ? voice->radius : 4);
    // REPLAY: Append only the cosmetic sound. Re-entering hear_world_action would record echoes
    // into other pebbles and overwrite listeners' destination with a second hearing event.
    if (game.sound_count < static_cast<int>(game.sounds.size())) {
        const auto sequence = static_cast<std::uint8_t>(game.sound_count);
        game.sounds[static_cast<std::size_t>(game.sound_count++)] =
            {voice ? voice->sound : SoundId::PebbleTone,shot.cell,game.tick,sequence,true,true};
    }
    shot.use_flash = 12;
    shot.light = {2,240,{139,198,211}};
    shot.timer_a = 60;
    ++shot.counter_b;
}
}

const EchoVoice* echo_voice(const Item& item) {
    if (item.kind != ItemKind::EchoPebble) return nullptr;
    for (const EchoVoice& voice : echo_voices)
        if (item.loaded == static_cast<int>(voice.sound)+1) return &voice;
    return nullptr;
}
const RegionalItem* echo_pebble_item(ItemKind kind) {
    return kind == ItemKind::EchoPebble ? &pebble : nullptr;
}

// MEMORY: Item loaded is sound ID+1 (zero is blank), spare is audible radius.
// Only carried/loose stones record; a thrown stone retains its launch-time recording.
void record_echo_pebbles(Game& game, SoundId sound, Cell source) {
    const auto voice = std::find_if(echo_voices.begin(),echo_voices.end(),
        [sound](const EchoVoice& entry) { return entry.sound == sound; });
    if (voice == echo_voices.end()) return;
    const auto carries_pebble = [source](const Entity& entity) {
        if (entity.kind == EntityKind::None || distance(entity.cell,source)>6) return false;
        if (entity.kind == EntityKind::GroundItem) return entity.ground_item.kind == ItemKind::EchoPebble;
        return entity.health>0 && std::any_of(entity.inventory.slots.begin(),entity.inventory.slots.end(),
            [](const Item& item) { return item.kind == ItemKind::EchoPebble; });
    };
    if (std::none_of(game.entities.begin(),game.entities.end(),carries_pebble)) return;
    const auto cells = audible_cells(game,source,6);
    for (Entity& entity : game.entities) {
        if (!carries_pebble(entity) || std::find(cells.begin(),cells.end(),entity.cell)==cells.end()) continue;
        if (entity.kind == EntityKind::GroundItem) {
            record(entity.ground_item,*voice); entity.sprite = Sprite::EchoPebbleReady;
        } else for (Item& item : entity.inventory.slots) record(item,*voice);
    }
}

bool launch_echo_pebble(Game& game, int owner, const Item& item, Cell facing) {
    const Entity& user = game.entities[static_cast<std::size_t>(owner)];
    Entity* shot = get_entity(game,spawn_entity(game,EntityKind::Projectile,user.cell));
    if (!shot) return false;
    shot->label_a = static_cast<int>(ProjectileKind::EchoPebble);
    shot->ground_item = item; shot->ground_item.count = 1;
    shot->counter_a = shot->attack_interval = item_pattern(item).maximum;
    shot->timer_b = 6; shot->timer_a = 240;
    shot->facing = facing; shot->point_a = user.cell;
    shot->entity_a = {owner,user.generation};
    shot->sprite = item.loaded>0 ? Sprite::EchoPebbleReady : Sprite::EchoPebble;
    return true;
}

// SHOT SLOTS: label_b 0 flight / 1 playback; counter_a remaining distance,
// counter_b completed echoes; timer_b travel beat; timer_a next echo / cleanup.
void step_echo_pebble(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (shot.kind!=EntityKind::Projectile || shot.label_a!=static_cast<int>(ProjectileKind::EchoPebble)) return;
    if (shot.health<=0) return;
    if (shot.use_flash == 0) shot.light = {};
    if (shot.label_b == 1) {
        if (shot.timer_a>0) return;
        if (shot.counter_b<3) replay(game,shot);
        else {
            emit_sound(game,SoundId::PebbleSpent,shot.cell);
            remove_entity(game,{slot,shot.generation});
        }
        return;
    }
    if (shot.timer_a==0) { land(game,shot); return; }
    if (shot.timer_b>0) return;
    const Cell next = shot.cell+shot.facing;
    if (projectile_blocked(game,next)) { land(game,shot); return; }
    shot.cell = next; --shot.counter_a; shot.timer_b = 6;
    if (shot.counter_a<=0 || entity_at(game,next,true)>=0) land(game,shot);
}

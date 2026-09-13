#include "behavior.hpp"
#include "dispatch.hpp"

void init_chicken(Game& game, Entity& entity) {
    const int variant = static_cast<int>(random_u32(game) % 4);
    entity.sprite = variant < 2 ? Sprite::Chick :
                    (variant == 2 ? Sprite::Hen : Sprite::Rooster);
    entity.health = entity.max_health = variant < 2 ? 1 : (variant == 2 ? 3 : 30);
    entity.move_interval = variant < 2 ? 18 : (variant == 2 ? 30 : 42);
    entity.move_wait = static_cast<int>(random_u32(game) %
                                        static_cast<std::uint32_t>(entity.move_interval));
    entity.impassable = true;
}

void step_chicken(Game& game, int slot) {
    Entity& chicken = game.entities[static_cast<std::size_t>(slot)];
    wander(game, slot);
    const SoundId call = chicken.sprite == Sprite::Chick ? SoundId::Chick :
                         (chicken.sprite == Sprite::Hen ? SoundId::Hen : SoundId::Rooster);
    maybe_growl(game, slot, call);
}

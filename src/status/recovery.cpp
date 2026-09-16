#include "effects.hpp"
#include "../game.hpp"

#include <algorithm>

int recovery_interval(const VitalEffects& effects) {
    return effects.recovery==RecoveryKind::Meal ? 18 : effects.recovery == RecoveryKind::Poultice ? 30 : 20;
}

bool begin_recovery(Entity& actor, RecoveryKind kind, int health) {
    VitalEffects& effects = actor.vitals;
    if (actor.health <= 0 || health <= 0 || effects.healing_left > 0) return false;
    effects.recovery = kind;
    effects.healing_left = static_cast<std::uint16_t>(std::clamp(health, 1, 1000));
    effects.healing_wait = static_cast<std::uint16_t>(recovery_interval(effects));
    return true;
}

void step_recovery(Entity& actor) {
    VitalEffects& effects = actor.vitals;
    if (effects.healing_left == 0) return;
    if (effects.healing_wait > 0) --effects.healing_wait;
    if (effects.healing_wait > 0) return;
    // PORTIONS: Full health still spends the pending point; storing future healing is not a reserve.
    actor.health = std::min(actor.max_health, actor.health + 1);
    --effects.healing_left;
    effects.healing_wait = effects.healing_left > 0 ?
        static_cast<std::uint16_t>(recovery_interval(effects)) : 0;
    if (effects.healing_left == 0) effects.recovery = RecoveryKind::Herbs;
}

bool interrupt_recovery(Entity& actor) {
    VitalEffects& effects = actor.vitals;
    if ((effects.recovery != RecoveryKind::Broth && effects.recovery != RecoveryKind::Meal) || effects.healing_left == 0) return false;
    effects.healing_left = effects.healing_wait = 0;
    effects.recovery = RecoveryKind::Herbs;
    return true;
}

#pragma once
#include "../game.hpp"
inline bool live_pay_cage(const Prop& prop) {
    return prop.kind==PropKind::PayCage && !prop.broken && prop.hp>0;
}

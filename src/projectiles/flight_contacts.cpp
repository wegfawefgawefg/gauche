#include "flight_contacts.hpp"
#include <algorithm>

bool first_flight_contact(Game& game,Handle shot,Handle victim) {
    for (const FlightContact& hit:game.flight_contacts)
        if (hit.projectile==shot && hit.victim==victim) return false;
    game.flight_contacts.push_back({shot,victim});
    return true;
}
void forget_flight_contacts(Game& game,Handle shot) {
    std::erase_if(game.flight_contacts,[shot](const FlightContact& hit){return hit.projectile==shot;});
}

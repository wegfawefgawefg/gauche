#include "contact.hpp"
#include "woodland.hpp"
#include "nail_board.hpp"

bool hit_ground_traps(Game& game,Cell cell,int damage,Cell source) {
    const bool woodland=hit_woodland_traps(game,cell,damage,source);
    const bool boards=hit_nail_boards(game,cell,damage,source);
    return woodland || boards;
}

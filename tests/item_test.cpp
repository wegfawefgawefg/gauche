#include "../src/game.hpp"
#include "../src/item_pattern.hpp"
#include "../src/ui/pattern_diagram.hpp"

#include <cstdio>

namespace {

bool check(bool condition, const char* message) {
    if (!condition) std::fprintf(stderr, "FAIL: %s\n", message);
    return condition;
}

Game small_game() {
    Game game;
    game.stage.width = 8;
    game.stage.height = 5;
    game.stage.tiles.resize(40);
    player_state(game, 0).controlled = spawn_entity(game, EntityKind::Player, {2, 2});
    game.started = true;
    return game;
}

bool buckler_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, player_state(game, 0).controlled);
    player->inventory.slots[0] = make_item(ItemKind::Buckler);
    *game.stage.at({4, 2}) = {TileKind::Wall, 100, 0};
    const Handle zombie = spawn_entity(game, EntityKind::Zombie, {3, 2});
    const bool shoved = use_held_item(game, player_state(game, 0).controlled.slot, {3, 2});
    if (!check(shoved && get_entity(game, zombie)->health == 0,
               "wall did not crush shoved actor")) return false;
    damage_entity(game, player_state(game, 0).controlled.slot, 5, {3, 2});
    if (!check(player->health == 100 && player->inventory.held()->durability == 25,
               "front block did not absorb hit and lose durability")) return false;
    for (int hit = 0; hit < 5; ++hit) damage_entity(game, player_state(game, 0).controlled.slot, 5, {3, 2});
    if (!check(player->inventory.held()->kind == ItemKind::None, "buckler did not break")) return false;

    Game teammates = small_game();
    Entity* blocker = get_entity(teammates, player_state(teammates, 0).controlled);
    blocker->inventory.slots[0] = make_item(ItemKind::Buckler);
    const Handle pushed = spawn_entity(teammates, EntityKind::Zombie, {3, 2});
    spawn_entity(teammates, EntityKind::Player, {4, 2});
    use_held_item(teammates, player_state(teammates, 0).controlled.slot, {3, 2});
    if (!check(get_entity(teammates, pushed)->health == 40,
               "teammate incorrectly became a crush surface")) return false;

    Game item_game = small_game();
    get_entity(item_game, player_state(item_game, 0).controlled)->inventory.slots[0] =
        make_item(ItemKind::Buckler);
    *item_game.stage.at({4, 2}) = {TileKind::Wall, 100, 0};
    const Handle item = spawn_entity(item_game, EntityKind::GroundItem, {3, 2});
    get_entity(item_game, item)->ground_item = make_item(ItemKind::Bandage);
    use_held_item(item_game, player_state(item_game, 0).controlled.slot, {3, 2});
    if (!check(get_entity(item_game, item) == nullptr,
               "item shoved into a wall did not break")) return false;

    Game hard = small_game();
    get_entity(hard, player_state(hard, 0).controlled)->inventory.slots[0] = make_item(ItemKind::Buckler);
    const Handle den = spawn_entity(hard, EntityKind::Den, {3, 2});
    use_held_item(hard, player_state(hard, 0).controlled.slot, {3, 2});
    return check(get_entity(hard, den)->cell == Cell{3, 2},
                 "buckler shoved a hard fixture");
}
bool equipment_rules() {
    Game game = small_game();
    Entity* player = get_entity(game, player_state(game, 0).controlled);
    player->inventory.slots[0] = make_item(ItemKind::Shotgun);
    player->inventory.slots[1] = make_item(ItemKind::SMG);
    player->inventory.slots[2] = make_item(ItemKind::Ammo);
    player->inventory.selected = 0;
    if (!check(use_held_item(game, player_state(game, 0).controlled.slot, {3, 2}),
               "shotgun failed to fire")) return false;
    if (!check(player->inventory.slots[0].loaded == 5 &&
               player->inventory.slots[1].loaded == 30,
               "weapons shared a magazine")) return false;
    player->inventory.selected = 2;
    use_held_item(game, player_state(game, 0).controlled.slot, player->cell);
    if (!check(player->inventory.slots[0].spare == 48 &&
               player->inventory.slots[1].spare == 210,
               "ammo did not refill each gun independently")) return false;

    Game trap_game = small_game();
    Entity* trapper = get_entity(trap_game, player_state(trap_game, 0).controlled);
    trapper->inventory.slots[0] = make_item(ItemKind::BearTrap);
    if (!check(use_held_item(trap_game, player_state(trap_game, 0).controlled.slot, {3, 2}) &&
               trapper->inventory.held()->opened &&
               item_sprite(*trapper->inventory.held()) == Sprite::BearTrapOpen &&
               trap_game.sounds[static_cast<std::size_t>(trap_game.sound_count - 1)].sound ==
                   SoundId::SturdyBlockBouncedOn,
               "bear trap did not open before placement")) return false;
    trapper->inventory.held()->cooldown = 0;
    if (!check(use_held_item(trap_game, player_state(trap_game, 0).controlled.slot, {3, 2}),
               "open bear trap placement failed")) return false;
    const Handle wolf = spawn_entity(trap_game, EntityKind::Wolf, {4, 2});
    move_entity(trap_game, wolf.slot, {3, 2});
    step_game(trap_game, {});
    bool recovered = false;
    for (const Entity& entity : trap_game.entities)
        recovered |= entity.kind == EntityKind::GroundItem &&
            entity.cell == Cell{3, 2} && entity.ground_item.kind == ItemKind::BearTrap &&
            !entity.ground_item.opened;
    return check(get_entity(trap_game, wolf) == nullptr && recovered,
                 "open bear trap did not deal 100 and remain recoverable");
}
bool item_attribute_rules() {
    Game heavy = small_game();
    Entity* shooter = get_entity(heavy, player_state(heavy, 0).controlled);
    shooter->inventory.slots[0] = make_item(ItemKind::Pistol, 1, ItemAttribute::Heavy);
    const Handle target = spawn_entity(heavy, EntityKind::Wolf, {4, 2});
    if (!check(use_held_item(heavy, player_state(heavy, 0).controlled.slot, {4, 2}) &&
               get_entity(heavy, target)->health == 23 &&
               shooter->inventory.held()->cooldown == 18,
               "heavy attribute did not raise damage and cooldown")) return false;

    Game big = small_game();
    Entity* striker = get_entity(big, player_state(big, 0).controlled);
    striker->inventory.slots[0] = make_item(ItemKind::Stick, 1, ItemAttribute::Big);
    const Handle distant = spawn_entity(big, EntityKind::Wolf, {4, 2});
    if (!check(item_pattern(*striker->inventory.held()).half_width == 1 &&
               use_held_item(big, player_state(big, 0).controlled.slot, {4, 2}) &&
               get_entity(big, distant)->health == 28,
               "big attribute changed the preview but not the melee reach")) return false;
    striker->inventory.held()->cooldown = 0;
    striker->inventory.held()->uses = 1;
    use_held_item(big, player_state(big, 0).controlled.slot, {4, 2});
    if (!check(striker->inventory.held()->kind == ItemKind::None,
               "limited-use stick did not break")) return false;

    const Item durable = make_item(ItemKind::Buckler, 1, ItemAttribute::Durable);
    const Item restorative = make_item(ItemKind::Bandage, 1, ItemAttribute::Restorative);
    return check(durable.durability == 60 && durable.max_durability == 60 &&
                 item_pattern(restorative).heal == 15,
                 "condition and healing attributes did not alter item rules");
}
bool item_stack_rules() {
    Inventory pack;
    if (!check(insert_item(pack, make_item(ItemKind::Bandage, 8)) &&
               insert_item(pack, make_item(ItemKind::Bandage, 5)) &&
               pack.slots[0].count == 10 && pack.slots[1].count == 3 &&
               pack.slots[0].max_count == 10,
               "bandages did not split at their stack capacity")) return false;
    if (!check(!insert_item(pack, make_item(ItemKind::Bandage, 100)) &&
               pack.slots[0].count == 10 && pack.slots[1].count == 3,
               "failed stack insertion did not roll back")) return false;

    Game game = small_game();
    Entity* player = get_entity(game, player_state(game, 0).controlled);
    player->inventory.slots[0] = make_item(ItemKind::Bandage, 2);
    player->health = 60;
    if (!check(player->inventory.held()->consume_on_use &&
               use_held_item(game, player_state(game, 0).controlled.slot, player->cell) &&
               player->inventory.held()->count == 1 &&
               player->inventory.held()->cooldown == 120 &&
               player->health == 70,
               "consumable count and cooldown were coupled incorrectly")) return false;
    player->inventory.slots[0] = make_item(ItemKind::Fist);
    spawn_entity(game, EntityKind::Zombie, {3, 2});
    return check(!player->inventory.held()->consume_on_use &&
                 use_held_item(game, player_state(game, 0).controlled.slot, {3, 2}) &&
                 player->inventory.held()->count == 1,
                 "persistent fist consumed its stack on use");
}

bool pattern_layout_rules() {
    const PatternDiagramLayout fist = pattern_diagram_layout(
        item_pattern(ItemKind::Fist), 10.0F, 20.0F, 160.0F, 50.0F);
    if (!check(fist.min_x == -1 && fist.max_x == 2 &&
               fist.min_y == -1 && fist.max_y == 1 &&
               fist.columns == 4 && fist.rows == 3,
               "short pattern was not padded by one cell")) return false;
    ItemPattern large;
    large.maximum = 49;
    large.half_width = 24;
    const PatternDiagramLayout fitted = pattern_diagram_layout(
        large, 10.0F, 20.0F, 160.0F, 50.0F);
    return check(fitted.x >= 10.0F && fitted.y >= 20.0F &&
                 fitted.x + static_cast<float>(fitted.columns) * fitted.cell_size <= 170.01F &&
                 fitted.y + static_cast<float>(fitted.rows) * fitted.cell_size <= 70.01F,
                 "large pattern escaped its allotted card area");
}
} // namespace

int main() {
    if (!buckler_rules() || !equipment_rules() ||
        !item_attribute_rules() || !item_stack_rules() ||
        !pattern_layout_rules()) return 1;
    std::puts("item rules passed");
    return 0;
}

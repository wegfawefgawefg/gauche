#include "../src/items/basic_actions.hpp"
#include "../src/items/ground_interaction.hpp"
#include "../src/items/action.hpp"
#include "../src/artifacts/powers.hpp"
#include "../src/artifacts/catalog.hpp"
#include "../src/combat/ranged.hpp"
#include "../src/combat/parry.hpp"
#include "../src/entities/dispatch.hpp"
#include "../src/world/chasm.hpp"
#include "../src/world/water.hpp"
#include "../src/world/loot.hpp"
#include "../src/items/supply.hpp"
#include <cstdio>
#include <memory>

namespace {
bool check(bool okay,const char* why) {if (!okay) std::fprintf(stderr,"FAIL new actions/powers: %s\n",why);return okay;}
std::unique_ptr<Game> arena() {
    auto game=std::make_unique<Game>();
    game->stage.width=16;game->stage.height=10;game->stage.tiles.resize(160);
    game->rng=12903;game->started=true;game->run.phase=RunPhase::Playing;
    game->run.death_policy=DeathPolicy::NoRespawn;
    auto& member=player_state(*game,0);member.online=true;
    member.controlled=spawn_entity(*game,EntityKind::Player,{3,4});
    get_entity(*game,member.controlled)->owner=0;
    return game;
}
Entity& player(Game& game) {return *get_entity(game,player_state(game,0).controlled);}
bool act(Game& game,ItemKind kind,Cell direction={1,0}) {
    auto& actor=player(game);set_basic_action(actor,kind);
    for (int i=0;i<quick_slots;++i) if (actor.inventory.slots[static_cast<std::size_t>(i)].kind==kind) {actor.inventory.selected=i;break;}
    return use_held_item(game,player_state(game,0).controlled.slot,actor.cell+direction);
}
bool contact_rules() {
    auto world=arena();auto& game=*world;auto& user=player(game);
    const Handle victim=spawn_entity(game,EntityKind::Zombie,{4,4});
    auto& target=*get_entity(game,victim);target.facing={-1,0};
    if (!check(act(game,ItemKind::Slap) && target.health==39 && target.facing==Cell{0,-1},"slap damages one and rotates")) return false;
    *game.stage.at({5,4})={TileKind::Wall,100,0};
    if (!check(act(game,ItemKind::Kick) && target.health==31 && target.cell==Cell{4,4},"kick stops at wall without crushing")) return false;
    target.health=20000000;target.max_health=target.health;
    if (!check(act(game,ItemKind::Shove) && target.health==0,"shove crushes even a pinned creature with extreme HP")) return false;
    remove_entity(game,victim);*game.stage.at({5,4})={};
    const auto rear=spawn_entity(game,EntityKind::Zombie,{2,4});
    const auto front=spawn_entity(game,EntityKind::Zombie,{4,4});
    if (!check(act(game,ItemKind::Elbow) && get_entity(game,rear)->health==28 && get_entity(game,front)->health==40,"elbow hits rear only")) return false;
    const auto blocker=spawn_entity(game,EntityKind::Zombie,{5,4});
    act(game,ItemKind::Shove);
    if (!check(get_entity(game,front)->health==36 && get_entity(game,blocker)->health==36,"body collision is an impact, not a crush")) return false;
    remove_entity(game,blocker);
    *game.stage.at({6,4})={TileKind::Wall,100,0};
    act(game,ItemKind::Shove);
    if (!check(get_entity(game,front)->health==36 && get_entity(game,front)->cell==Cell{5,4},"shove stops at a wall after crossing one empty cell")) return false;
    remove_entity(game,front);*game.stage.at({6,4})={};
    game.stage.at({4,4})->prop={PropKind::ClayPot,5};
    act(game,ItemKind::Kick);
    if (!check(game.stage.at({4,4})->prop.broken && game.stage.at({5,4})->prop.kind==PropKind::None,"kick breaks a pot instead of pushing it")) return false;
    game.stage.at({4,4})->prop={};
    user.inventory.slots[0]=make_item(ItemKind::CrushShield);
    game.stage.at({4,4})->prop={PropKind::Crate,20};
    use_held_item(game,0,{4,4});
    if (!check(game.stage.at({5,4})->prop.kind==PropKind::Crate,"crushing shield pushes portable props")) return false;
    game.stage.at({5,4})->prop={};
    user.powers[static_cast<std::size_t>(ArtifactKind::Chef)]=1;
    const auto food=spawn_entity(game,EntityKind::GroundItem,{4,4});
    get_entity(game,food)->ground_item=make_item(ItemKind::RawMeat,2);
    act(game,ItemKind::Fist);
    return check(get_entity(game,food)->ground_item.kind==ItemKind::CookedMeat && get_entity(game,food)->ground_item.count==2,"Chef cooks the contacted raw food stack");
}
bool parry_rules() {
    auto world=arena();auto& game=*world;auto& user=player(game);
    const auto foe=spawn_entity(game,EntityKind::Zombie,{4,4});
    const int slot=player_state(game,0).controlled.slot;
    act(game,ItemKind::ParryPan);
    damage_entity(game,slot,9,{4,4},true,foe);
    if (!check(user.health==100 && get_entity(game,foe)->health==13,"tight pan returns melee damage")) return false;
    damage_entity(game,slot,5,{2,4},true);
    if (!check(user.health==95,"rear hit bypasses pan")) return false;
    user.block_ticks=0;damage_entity(game,slot,5,{4,4},true,foe);
    if (!check(user.health==90,"missed parry takes damage")) return false;
    act(game,ItemKind::ParryPan);
    fire_bullet(game,foe.slot,{4,4},{-1,0},make_item(ItemKind::Pistol),true);
    return check(user.health==90 && get_entity(game,foe)->health==0 && user.inventory.held()->kind==ItemKind::ParryPan,"pan reflects a shot and never wears out");
}
bool jump_obstacle_rules() {
    for (const auto kind:{EntityKind::Zombie,EntityKind::Bat}) {
        auto world=arena();auto& game=*world;auto& user=player(game);
        const auto foe=spawn_entity(game,kind,{4,4});const int hp=get_entity(game,foe)->health;
        if (!check(act(game,ItemKind::Jump),"jump can start over a creature")) return false;
        for (int i=0;i<18;++i) step_basic_state(game,0);
        if (!check(user.cell==Cell{5,4} && user.health==100 && get_entity(game,foe)->health==hp,"jump clears zombie or bat without collision damage")) return false;
    }
    auto world=arena();auto& game=*world;auto& user=player(game);
    const auto foe=spawn_entity(game,EntityKind::Zombie,{4,4});
    act(game,ItemKind::Jump);
    const auto late=spawn_entity(game,EntityKind::Zombie,{5,4});
    for (int i=0;i<18;++i) step_basic_state(game,0);
    if (!check(user.cell==Cell{3,4},"occupied landing safely returns to reserved takeoff cell")) return false;
    remove_entity(game,foe);remove_entity(game,late);*game.stage.at({4,4})={TileKind::Wall,100,0};
    set_basic_action(user,ItemKind::Jump);game.sound_count=0;
    for (int i=0;i<60;++i) use_held_item(game,0,{4,4});
    return check(user.basic.jump_ticks==0 && game.sound_count==0 && user.inventory.held()->cooldown>0,"blocked jump has retry delay and no repeated buzz");
}
bool jump_balloon_rules() {
    auto world=arena();auto& game=*world;auto& user=player(game);const int slot=player_state(game,0).controlled.slot;
    *game.stage.at({4,4})={TileKind::Chasm};
    if (!check(act(game,ItemKind::Jump),"start animated two-cell jump")) return false;
    for (int i=0;i<18;++i) {step_basic_state(game,slot);chasm_contact(game,slot);}
    if (!check(user.cell==Cell{5,4} && user.health==100 && user.basic.jump_ticks==0,"jump crosses gap and lands on bank")) return false;
    user.cell={3,4};*game.stage.at({5,4})={TileKind::Chasm};
    act(game,ItemKind::Jump);
    for (int i=0;i<18;++i) {step_basic_state(game,slot);chasm_contact(game,slot);}
    if (!check(user.health==0,"landing in a pit still kills")) return false;
    world=arena();auto& water=*world;auto& floater=player(water);
    floater.inventory.slots[0]=make_item(ItemKind::Balloon);*water.stage.at(floater.cell)={TileKind::DeepRiver};
    for (int i=0;i<3599;++i) {step_basic_state(water,slot);deep_river_contact(water,slot);}
    if (!check(floater.health==100 && floater.inventory.held()->loaded==1,"balloon supports deep water for its full minute")) return false;
    step_basic_state(water,slot);deep_river_contact(water,slot);
    if (!check(floater.health==0 && floater.inventory.held()->kind==ItemKind::Fist,"expiry over deep water kills and loses cargo")) return false;
    world=arena();auto& legacy=*world;auto& flyer=player(legacy);
    flyer.inventory.slots[0]=make_item(ItemKind::Balloon);*legacy.stage.at({4,4})={TileKind::Water};
    if (!check(move_entity(legacy,slot,{4,4}) && flyer.health==100,"balloon also crosses older deep-water tiles")) return false;
    flyer.inventory.held()->loaded=1;step_basic_state(legacy,slot);deep_river_contact(legacy,slot);
    return check(flyer.health==0,"old water is fatal when balloon expires");
}
bool grapple_rules() {
    auto world=arena();auto& game=*world;auto& user=player(game);
    const int slot=player_state(game,0).controlled.slot;
    const auto body=spawn_entity(game,EntityKind::Zombie,{4,4});
    if (!check(act(game,ItemKind::Grapple) && user.basic.grabbed==body && entity_at(game,user.cell,true)==slot,"grapple lifts a creature without blocking its carrier")) return false;
    user.inventory.held()->cooldown=0;
    use_held_item(game,slot,{4,4});
    if (!check(user.basic.grabbed.slot<0 && get_entity(game,body)->toss.ticks>0,"second use throws held creature through the air")) return false;
    remove_entity(game,body);
    const auto bear=spawn_entity(game,EntityKind::Bear,{4,4});
    user.inventory.held()->cooldown=0;
    if (!check(!use_held_item(game,slot,{4,4}),"heavy creatures resist grapple")) return false;
    remove_entity(game,bear);
    game.stage.at({4,4})->prop={PropKind::Crate,20,0,false,0,false};
    user.inventory.held()->cooldown=0;
    if (!check(use_held_item(game,slot,{4,4}) && user.basic.held_prop.kind==PropKind::Crate && game.stage.at({4,4})->prop.kind==PropKind::None,"grapple lifts actual crate prop")) return false;
    user.inventory.held()->cooldown=0;use_held_item(game,slot,{4,4});
    for (int i=0;i<24;++i) step_basic_state(game,slot);
    if (!check(user.basic.held_prop.kind==PropKind::None && game.stage.at({7,4})->prop.kind==PropKind::Crate,"thrown crate flies and lands as the same prop")) return false;
    const auto another=spawn_entity(game,EntityKind::Zombie,{4,4});
    user.inventory.held()->cooldown=0;use_held_item(game,slot,{4,4});
    user.inventory.selected=1;step_basic_state(game,slot);
    return check(user.basic.grabbed.slot<0 && get_entity(game,another)->basic.carried_by.slot<0,"changing hands releases held creature");
}
bool stack_rules() {
    auto world=arena();auto& game=*world;auto& user=player(game);
    grant_artifact(user,ArtifactKind::StrongArms);grant_artifact(user,ArtifactKind::StrongArms);
    if (!check(artifact_count(user,ArtifactKind::StrongArms)==2 && power_damage(user,20)==30,"Strong Arms stacks on damage")) return false;
    const auto victim=spawn_entity(game,EntityKind::Zombie,{5,4});
    user.inventory.slots[0]=make_item(ItemKind::Pistol);
    use_held_item(game,0,{4,4});
    if (!check(get_entity(game,victim)->health==16,"Strong Arms affects guns too")) return false;
    grant_artifact(user,ArtifactKind::QuickHands);
    if (!check(power_damage(user,20)<30,"Quick Hands trades damage for speed")) return false;
    if (!check(!artifact_eligible(user,ArtifactKind::CriticalPower),"crit power needs crit chance")) return false;
    grant_artifact(user,ArtifactKind::CriticalChance);
    if (!check(artifact_eligible(user,ArtifactKind::CriticalPower),"crit chance enables crit power")) return false;
    grant_artifact(user,ArtifactKind::Vitality);grant_artifact(user,ArtifactKind::Vitality);
    if (!check(user.max_health==140,"HP Up stacks")) return false;
    grant_artifact(user,ArtifactKind::FleetFeet);
    int ticks=0;for (int i=0;i<100;++i) ticks+=power_move_interval(user);
    if (!check(ticks>=665 && ticks<=667,"first 5 percent speed stack survives tick rounding")) return false;
    user.health=50;grant_artifact(user,ArtifactKind::Regeneration);
    for (int i=0;i<1199;++i) step_player_powers(game,user);
    if (!check(user.health==50,"regen is slow")) return false;
    step_player_powers(game,user);
    if (!check(user.health==51,"regen gives one HP per twenty seconds")) return false;
    grant_artifact(user,ArtifactKind::Medic);
    if (!check(power_healing(user,20)==25,"Medic increases healing")) return false;
    grant_artifact(user,ArtifactKind::Technical);
    Item tool=make_item(ItemKind::Hatchet);improve_pickup(user,tool);
    const int cap=tool.max_durability;tool.durability-=12;
    improve_pickup(user,tool);
    if (!check(cap>35 && tool.durability==cap-12,"repeated pickup cannot refill Technical condition")) return false;
    grant_artifact(user,ArtifactKind::Technical);improve_pickup(user,tool);
    if (!check(tool.max_durability==70 && tool.durability==58,"higher Technical level preserves accumulated wear")) return false;
    user.powers[static_cast<std::size_t>(ArtifactKind::QuickHands)]=max_power_stacks;
    user.powers[static_cast<std::size_t>(ArtifactKind::FleetFeet)]=max_power_stacks;
    user.powers[static_cast<std::size_t>(ArtifactKind::Dodge)]=max_power_stacks;
    step_player_powers(game,user);
    if (!check(user.action_steps>1000 && power_move_interval(user)==1 && power_damage(user,100)>=1,"extreme stacks remain bounded and useful")) return false;
    for (int i=0;i<100;++i) {
        user.health=user.max_health;game.run.phase=RunPhase::Playing;finish_floor(game);
        const auto offers=player_state(game,0).offers;
        for (int j=0;j<3;++j) {
            if (!check(offers[static_cast<std::size_t>(j)].kind==RewardKind::Artifact && artifact_eligible(user,offers[static_cast<std::size_t>(j)].artifact),"every reward is an eligible permanent upgrade")) return false;
            for (int k=0;k<j;++k) if (!check(offers[static_cast<std::size_t>(j)].artifact!=offers[static_cast<std::size_t>(k)].artifact,"three offers differ")) return false;
        }
    }
    // Corpse coins were already valued at first pickup; dying cannot multiply them.
    user.powers[static_cast<std::size_t>(ArtifactKind::Golddigger)]=4;
    player_state(game,0).coins=0;place_coins(game,user.cell,10,true);collect_coins(game,user);
    if (!check(player_state(game,0).coins==10,"Golddigger cannot multiply dropped player gold")) return false;
    if (!check(supply_count(ItemKind::ThrowingRock)==6 && item_pattern(ItemKind::ThrowingRock).damage==28,"throwing rocks have useful damage and quantities")) return false;
    set_basic_action(user,ItemKind::Kick);game.run.floor=1;
    choose_reward(game,0,0);
    if (!check(game.run.floor==2 && player(game).basic_action==ItemKind::Kick &&
        artifact_count(player(game),ArtifactKind::QuickHands)==max_power_stacks,"floor transitions preserve basic action and large power stacks")) return false;
    game.run.death_policy=DeathPolicy::NextFloor;
    damage_entity(game,player_state(game,0).controlled.slot,100000000,player(game).cell,false);
    game.run.floor=3;generate_world_floor(game);
    return check(player(game).health==player(game).max_health && player(game).basic_action==ItemKind::Kick &&
        player(game).inventory.slots[0].kind==ItemKind::Kick && artifact_count(player(game),ArtifactKind::QuickHands)==max_power_stacks,
        "next-floor revival keeps permanent action and powers");
}
}
bool basic_actions_and_powers_tests() {
    return contact_rules() && parry_rules() && jump_obstacle_rules() && jump_balloon_rules() && grapple_rules() && stack_rules();
}

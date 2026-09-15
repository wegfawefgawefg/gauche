#include "../entities/rail_cart.hpp"
#include "belt_tools.hpp"
#include "../props/conveyor.hpp"
namespace {
constexpr RegionalItem crank{"Belt Crank", "Hold to turn an adjacent unpowered belt run. Forty turns; brakes and broken sections stop the drive.",
    Sprite::BeltCrank,{1,1,0,0,12,PatternEffect::Utility},ItemAction::Material,11,1,false,40,0,0,0,0,SoundId::BeltCrank};
constexpr RegionalItem shoe{"Brake Shoe", "Stop an adjacent cart or belt for 6s. Four shoes. A 16-damage hit breaks the brake. Trains ignore it.",
    Sprite::BrakeShoe,{1,1,0,0,24,PatternEffect::Utility},ItemAction::Material,10,1,false,4,0,0,0,0,SoundId::BrakeFit};
}
const RegionalItem* belt_tool_item(ItemKind kind) {
    if (kind==ItemKind::BeltCrank) return &crank;
    if (kind==ItemKind::BrakeShoe) return &shoe;
    return nullptr;
}
bool use_belt_tool(Game& game,int slot,Cell direction) {
    const auto& user=game.entities[static_cast<std::size_t>(slot)];
    const Cell target=user.cell+direction;
    return user.inventory.held()->kind==ItemKind::BeltCrank ? crank_belt(game,target) : (brake_rail_cart(game,target) || brake_belt(game,target));
}

#pragma once

enum class Action : int {
    MoveUp, MoveDown, MoveLeft, MoveRight,
    AimUp, AimDown, AimLeft, AimRight,
    Use, Pickup, Drop, Reload, Interact, Confirm,
    Slot1, Slot2, Slot3, Slot4, Slot5, Slot6,
    PreviousSlot, NextSlot, Inventory, Compare, CompactDetails,
};

constexpr int action_id(Action action) { return static_cast<int>(action); }

"""Restrained material bonks for first walking contact, without impact debris."""
from forest_enemies import impact

if __name__ == "__main__":
    impact("bump_wood", .12, 170, 543, True)
    impact("bump_stone", .10, 310, 75)

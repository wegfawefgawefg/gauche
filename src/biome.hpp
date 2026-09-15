#pragma once

// RUN ORDER: Regional rules follow biome identity, not an old positional index.
// The fourth zone is still in design; it must be integrated before extending runs.
enum class Biome { Forest, Ice, Industrial };
inline constexpr int floors_per_biome=4;
inline constexpr int implemented_biomes=3;
inline constexpr int run_floor_count=floors_per_biome*implemented_biomes;

constexpr int first_floor(Biome biome) {
    return static_cast<int>(biome)*floors_per_biome+1;
}
constexpr bool biome_floor(int floor,Biome biome) {
    return floor>=first_floor(biome) && floor<first_floor(biome)+floors_per_biome;
}
constexpr bool forest_floor(int floor) { return biome_floor(floor,Biome::Forest); }
constexpr bool ice_floor(int floor) { return biome_floor(floor,Biome::Ice); }
constexpr bool industrial_floor(int floor) { return biome_floor(floor,Biome::Industrial); }
constexpr Biome floor_biome(int floor) {
    return ice_floor(floor) ? Biome::Ice : industrial_floor(floor) ? Biome::Industrial : Biome::Forest;
}
constexpr int biome_stage(int floor) { return (floor-1)%floors_per_biome+1; }
constexpr const char* biome_name(Biome biome) {
    switch (biome) {
    case Biome::Forest: return "FOREST";
    case Biome::Ice: return "ICE";
    case Biome::Industrial: return "INDUSTRIAL";
    }
    return "";
}

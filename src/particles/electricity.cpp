#include "electricity.hpp"

void spawn_water_arc(Cosmetics& cosmetics, Cell from, Cell to, std::uint64_t seed) {
    // SURFACE: A thin traveling branch; no repeated impact forks or a light per wet tile.
    RibbonParticle bolt;
    bolt.count = 5; bolt.life = bolt.span = 7;
    bolt.red = 117; bolt.green = 191; bolt.blue = 218;
    const float dx = static_cast<float>(to.x - from.x), dy = static_cast<float>(to.y - from.y);
    for (int i = 0; i < bolt.count; ++i) {
        seed ^= seed << 13; seed ^= seed >> 7; seed ^= seed << 17;
        const float along = static_cast<float>(i) / 4;
        const float wobble = i == 0 || i == 4 ? 0 :
            static_cast<float>(static_cast<int>((seed >> 24) & 7U) - 3) * .035F;
        bolt.points[static_cast<std::size_t>(i)] = {
            static_cast<float>(from.x) + .5F + dx * along - dy * wobble,
            static_cast<float>(from.y) + .5F + dy * along + dx * wobble};
    }
    if (cosmetics.ribbons.size() < 256) cosmetics.ribbons.push_back(bolt);
}

void spawn_electric_arc(Cosmetics& cosmetics, Cell from, Cell to, std::uint64_t seed) {
    // PRESENTATION: Jaggedness is local; the simulation already chose both endpoints.
    RibbonParticle bolt;
    bolt.count = 8; bolt.life = bolt.span = 9;
    bolt.red = 163; bolt.green = 221; bolt.blue = 255;
    const float dx = static_cast<float>(to.x - from.x), dy = static_cast<float>(to.y - from.y);
    for (int index = 0; index < bolt.count; ++index) {
        seed ^= seed << 13; seed ^= seed >> 7; seed ^= seed << 17;
        const float along = static_cast<float>(index) / 7;
        const float wobble = index == 0 || index == 7 ? 0 :
            static_cast<float>(static_cast<int>((seed >> 24) & 7U) - 3) * .045F;
        bolt.points[static_cast<std::size_t>(index)] = {
            static_cast<float>(from.x) + .5F + dx * along + (dy == 0 ? 0 : wobble),
            static_cast<float>(from.y) + .5F + dy * along + wobble};
    }
    if (cosmetics.ribbons.size() < 256) cosmetics.ribbons.push_back(bolt);
    if (cosmetics.flashes.size() < 128)
        cosmetics.flashes.push_back({{to, 3, 1.0F, {.55F, .8F, 1.0F}}, 8, 8});
    // SPARKS: Short forks also make a wall impact visible when both endpoints coincide.
    for (int index = 0; index < 3 && cosmetics.ribbons.size() < 256; ++index) {
        RibbonParticle spark;
        spark.count = 3; spark.life = spark.span = 7 + index;
        spark.red = 211; spark.green = 237; spark.blue = 255;
        const float x = static_cast<float>(to.x) + .5F, y = static_cast<float>(to.y) + .5F;
        const float side = index == 1 ? -.3F : .2F;
        spark.points[0] = {x, y};
        spark.points[1] = {x + side, y - .14F * static_cast<float>(index + 1)};
        spark.points[2] = {x + side * .5F, y - .28F * static_cast<float>(index + 1)};
        cosmetics.ribbons.push_back(spark);
    }
    push_debris(cosmetics.debris, to, .65F, .04F);
}

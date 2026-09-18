#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include <string>

struct StageAnnouncement {
    std::uint64_t seed = 0;
    int floor = 0;
    unsigned int revision = 0;
    float age = 10;
    std::string title, subtitle;
};
void update_stage_announcement(StageAnnouncement& banner, const Game& game,
                               float dt, bool playing, unsigned int revision);
void draw_stage_announcement(tr::Renderer* renderer, const StageAnnouncement& banner);

#pragma once
#include "renderer/device.hpp"
#include <SDL3/SDL.h>
#include <cstdint>
int effective_frame_cap(SDL_Window* window,tr::Renderer* renderer,int configured,bool wants_vsync,bool bot);
void sleep_frame_remainder(std::uint64_t frame_start,int cap);

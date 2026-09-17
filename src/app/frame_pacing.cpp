#include "frame_pacing.hpp"
#include <algorithm>
#include <cmath>

int effective_frame_cap(SDL_Window* window,SDL_Renderer* renderer,int configured,bool wants_vsync,bool bot) {
    if(bot)return 30;
    if(configured>0)return configured;
    int actual=0;
    if(!wants_vsync || (SDL_GetRenderVSync(renderer,&actual) && actual!=0))return 0;
    // A backend may reject V-sync. Match the display in software instead of
    // silently running uncapped when the user requested synchronized rendering.
    const auto* mode=SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(window));
    return mode && mode->refresh_rate>0 ? std::clamp(static_cast<int>(std::lround(mode->refresh_rate)),30,1000) : 60;
}
void sleep_frame_remainder(std::uint64_t frame_start,int cap) {
    if(cap<=0)return;
    const auto target=std::uint64_t{1'000'000'000}/static_cast<std::uint64_t>(cap);
    const auto elapsed=SDL_GetTicksNS()-frame_start;
    // This yields to the OS for the whole remainder; DelayPrecise busy-spins at
    // its tail. Oversleep is preferable to burning laptop CPU on that tail.
    if(elapsed<target)SDL_DelayNS(target-elapsed);
}

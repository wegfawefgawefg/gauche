#include "input.hpp"

#include <array>
#include <cmath>
#include <unordered_map>

namespace {

bool using_mouse = true;
std::unordered_map<SDL_JoystickID, std::array<int, SDL_GAMEPAD_AXIS_COUNT>> axis_anchors;

} // namespace

bool pointer_device_active() { return using_mouse; }

void observe_input_device(const SDL_Event& event) {
    if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) using_mouse = false;
    else if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION &&
             event.gaxis.axis < SDL_GAMEPAD_AXIS_COUNT) {
        int& anchor = axis_anchors[event.gaxis.which][event.gaxis.axis];
        const int value = event.gaxis.value;
        const int threshold = event.gaxis.axis >= SDL_GAMEPAD_AXIS_LEFT_TRIGGER ? 4000 : 10000;
        if (std::abs(value) < threshold) anchor = 0;
        else if (std::abs(value - anchor) > 3000) {
            using_mouse = false;
            anchor = value;
        }
    } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED)
        axis_anchors.erase(event.gdevice.which);
    else if (event.type == SDL_EVENT_MOUSE_MOTION && event.motion.which != SDL_TOUCH_MOUSEID &&
             (std::abs(event.motion.xrel) >= .5F || std::abs(event.motion.yrel) >= .5F))
        using_mouse = true;
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.which != SDL_TOUCH_MOUSEID)
        using_mouse = true;
}

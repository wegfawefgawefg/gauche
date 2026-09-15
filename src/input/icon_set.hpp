#pragma once

enum class ControllerIcons { Auto, Xbox, PlayStation, Nintendo };
ControllerIcons controller_icons();
const char* controller_icon_name();
void load_controller_icons();
bool set_controller_icons(ControllerIcons icons);

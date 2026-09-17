#include "../app/options.hpp"
#include "icon_set.hpp"
#include <SDL3/SDL.h>
#include <filesystem>
#include <fstream>

namespace {
ControllerIcons icons = ControllerIcons::Auto;
std::filesystem::path icon_path() {
    return user_data_root() / "controller-icons.cfg";
}
}
ControllerIcons controller_icons() { return icons; }
const char* controller_icon_name() {
    constexpr const char* names[]{"Auto", "Xbox", "PlayStation", "Nintendo"};
    return names[static_cast<int>(icons)];
}
void load_controller_icons() {
    std::ifstream input(icon_path());
    int value = 0;
    if (input >> value && value >= 0 && value <= 3) icons = static_cast<ControllerIcons>(value);
}
bool set_controller_icons(ControllerIcons choice) {
    const auto path = icon_path();
    if (path.empty()) return false;
    auto temporary = path; temporary += ".tmp";
    std::ofstream output(temporary);
    output << static_cast<int>(choice) << '\n'; output.close();
    if (!output) return false;
    std::error_code error;
    std::filesystem::rename(temporary, path, error);
    if (error) return false;
    icons = choice;
    return true;
}

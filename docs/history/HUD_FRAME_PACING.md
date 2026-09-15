# HUD, cursor and high refresh rendering

2026-09-15 feedback implemented.

Quick slots, inventory rows and HP now share the same angled panel, gradient and offset shadow. Selected slots use the inventory's warm highlight. Health fill follows the angled silhouette; information density and footprint remain compact.

ImGui's SDL3 backend calls its mouse-cursor updater every NewFrame, even when no debug windows are open. Gauche now sets ImGuiConfigFlags_NoMouseCursorChange. The game owns OS cursor visibility and draws its pointer over menus/ImGui. Controller use still hides the game pointer through the existing input-device tracking.

Splonks/splonks-cpp-gview-menu/src/main.cpp caps only when ConfiguredFrameCapFps returns a positive value. Gauche erroneously converted the unlimited value zero to 60. That fallback is removed. Frame elapsed time uses nanoseconds instead of milliseconds, and menu animations receive real elapsed time. Simulation remains 60 Hz; camera presentation uses the fractional step accumulator, and actors remain snapped to their actual tiles.

This user's local frame cap is 144; VSync remains enabled. Settings > Display retains Unlimited/60/120/144. Actual achieved refresh still depends on display/VSync and rendering cost; no desktop performance or flicker playtest was performed.

Strict game and capture builds passed. Static HUD and inventory renders inspected at 1920x1080. No new test suite or live playthrough.

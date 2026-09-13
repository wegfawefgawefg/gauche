# Gauche

Planned C++20 successor to the original Rust Gauche prototype in the
neighboring `gauche-rs` repository. The first step is a close port of its
mechanics and look. The target game is a fast, rectilinear co-op dungeon run:
spawn together, improvise through generated rooms and guarded objectives,
then reach an exit. Keep Gauche's fast tile-step movement; barricades,
pickaxes, bombs, and the existing rail-laying train can change a route.
Gunplay and lighting shape combat. Use direct C++ game code with the SDL3/Gubsy
host and asset patterns of `splonks-cpp`. Co-op includes reconnects and a
configurable death rule. Floors lead to a pick-one-of-three reward, with
occasional shops and run-long artifacts; themed four-floor worlds start with
forest caves and can later branch into fire and ice.

No game code has been ported yet. See [the port plan](docs/PORT_PLAN.md) and
[forest content sketch](docs/FOREST_CONTENT_IDEAS.md).

# Storm Lantern

Implemented 2026-09-15. Ice item 37/50.

A sealed directional lamp with 120 seconds of fuel. Hold Use for a longer focused beam; Secondary toggles its shutter. Stowing covers the lamp and pauses fuel use. Dropped lamps keep their direction and fuel. No fire damage or free refills from swaps/ammo. Ice rewards, shops, weather stations and candle cabinets can provide one.

Light emitters now support Omni, Cone and Beam. The renderer clips directional light against physical cover; Lantern Moths use the same integer angular attenuation. Existing omnidirectional lights preserve their behavior. Fuel, shutter and light shape survive snapshots; protocol layout 36.

Three original 16px sprites and five offline synthesized sounds. Strict game/capture/codec builds passed; existing codec fixture passed with a partially fueled focused lamp. Static wide-beam and inventory captures inspected; audio decoded finite with headroom. No live playtest.

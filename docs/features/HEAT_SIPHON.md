# Heat Siphon

Implemented 2026-09-15. Ice regional item 42/50.

Primary use draws up to five seconds of heat from the cell directly ahead.
Secondary discharges a short flame cone. Thirty seconds of storage is six full
shots; partial draws accumulate exactly, and partial shots scale damage and burn
duration. A full shot deals eight immediate damage and applies the existing
five-second weak burn. Friends are exposed too. Heat melts ice/snow/props and
ignites suitable fuel through the existing temperature rules.

Sources are lit candles/stoves, burning floor fuel, landed flares, fueled boiler
tanks, burning actors and exposed burning sticks. Sources lose exactly the
stored amount; depleted lamps go dark. A campfire instead yields five discrete
portions, sharing its existing trample budget. Already-built boiler pressure
remains dangerous. Lava, permanent torches and living embers are not fuel reserves.
Full storage and absent sources reject use without spending heat or cooldown.

The cone reaches two cells, with cover captured before any melting. Big adds
reach and width; Long adds reach. Strong, Agile and Heavy use the usual effective
damage/cooldown definitions. Empty tools remain reusable; ammo does not refill
them. No automatic decay while stowed. Stored heat is part of the normal item
snapshot/hash and persists when dropped, traded or saved in a debug loadout.

Price 34. Ice rewards/shops, occasional boiler-gallery equipment and a 3%
maintenance-locker roll. HUD/inventory show fractional charges and the actual
secondary binding; debug loadouts can set zero to thirty stored seconds. Sparse
brass empty/charged icons and three offline-generated cues accompany local flame
sprites. Nearby echo hounds hear the draw/discharge through shared sound rules.

Snapshot shape remains 37. Gameplay compatibility is `0x2026091511`; multiplayer
peers need matching builds. Validation: strict game/static-render builds;
direct calls checking partial transfer, capacity, five-portion campfire exhaustion,
partial/full shots, cover, secondary priority, Big geometry, snapshot round trip
and rejection of excess heat. Inspected dummy-driver inventory/flame captures.
No autonomous playthrough; balance remains for user playtesting.

# Hand Bellows — 2026-09-15

Industry gains another reusable tool with uses across existing encounters:
eight directional puffs, a three-tile cone, and a 0.75-second cooldown. Native
stage-one stock, containers and treasure can supply it; half of ash lofts offer
bellows instead of coolant. Price 10. Big widens the cone, Long extends reach,
and Durable doubles capacity. This raises implemented Industry regional items
from eighteen to nineteen; the fifty-item target remains unfinished.

## Shared interactions

Living movable actors and loose ground items move one cell. Targets are captured
with generation-safe handles and expected cells, sorted farthest first, and
rechecked before each push. Each target moves at most once. Hard blockers,
rooted/gripped/tossed actors, trains and sled riders/cargo are excluded. Walls,
occupied landings and props stop movement without crush damage. Real floor
contacts still apply, so hazards matter; flying enemies retain their flight
rules. Loose gear retains its actual state and stops floating before displacement.

The gust moves smoke, sleeping spores and whiteout one cell, preserving remaining
duration and merging overlapping clouds. Closed doors and walls stop air; grates
allow it through. Existing flames can ignite real fuel one cell farther forward.
Sources are captured before ignition, preventing an instantaneous chain through
the whole cone, and old fires receive no free lifetime. A nearby campfire with
bare floor ahead cannot manufacture burning floor.

Affected Ash Sleepers wake even if their landing is blocked. Existing outward
sweep events draw gust ribbons and push local debris. Cosmetic buffer limits,
sound settings and debris counts do not change item consumption or gameplay.
An empty puff still spends a use. This is shared surface wind logic, available
to later fan fixtures; those fixtures are not yet implemented.

## Assets and validation

Original transparent bellows sprite retained at
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-0f2fe4eb-89b3-47fb-b319-34d94ad16b4b.png`.
The prompt requested muted brown wooden paddles, red leather, a brass nozzle,
isolated pixel clusters and transparent alpha, without hands or scenery.
Two original leather/air OGG cues come from `tools/sound/hand_bellows.py`.
The sprite and an SDL dummy static render were inspected; both sound files loaded.

Strict release build passed. Temporary direct checks covered one-step pushes,
wall safety, cargo preservation, flyers/riders, ash waking, gas movement and door
occlusion, finite fire spread, last-use consumption, snapshot/hash consistency,
full cosmetic buffers and local debris independence. Sixty-four generated
Industry floors retained reachable routes and required locks, and supplied three
bellows. No autonomous playthrough; live balance still needs human feedback.

Gameplay compatibility version is `0x2026091556`; no new serialized fields.

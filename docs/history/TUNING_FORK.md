# Tuning Fork and crystal veins

Implemented Ice item 45/50. A 0.5-second committed windup rings a straight
vibration through touching crystal, up to eight cells. The first non-crystal cell
receives the pulse and ends the chain; an empty gap cannot relay it to another
vein. Stone, deep water, blocking furniture, closed gates and felt-covered optics
stop it. It does not dig ordinary walls or transmit through ice alone.

Each reached creature takes eight damage through ordinary defenses, including
friendly players. Crystal growths have six HP and shatter; mirror shards and crystal lenses
also conduct and take damage. Shard-colony nodes conduct and receive a 90-tick
endpoint interruption. A damaged surviving lens or node can still carry the
current pulse. Material is sampled before destruction, so breaking the first
crystal does not prevent the rest of that same strike from traveling.

The fork has 24 uses and a 0.8-second cooldown after release. Durable doubles uses;
Long extends the chain cap to twelve. Strong, Heavy, Agile and Fragile follow the
existing damage, timing and use-count modifiers. Big and Piercing are unsupported.
Canceling a windup spends nothing. Facing is committed, while movement remains
available. The shared saved player action slots carry pending windup state.

The ring wakes and attracts existing listeners within the shared bounded hearing
rules; Echo Pebbles can record/replay it. Replay copies only the noise. New metal
windup/ring/spent tones, quiet per-cell vibration tones and glass fracture audio
are generated offline by `tools/sound/tuning_fork.py`. The fork and crystal growth
sprites come from `tools/art/tuning_fork.py`; broken growths use local crystal
splinter debris and the pulse uses local rings.

Crystal galleries, echo tunnels and ice quarries can grow one three- or five-cell
vein in an available alcove. Placement avoids protected routes, wet cells and
occupied cells. Growths are ordinary blocking props, nonflammable and breakable by
fists; progression never requires finding a fork. Crystal-gallery supply rolls,
10% of broken lens cases, Ice rewards and shops can supply the fork, value 22.

No new snapshot fields. Pending action and prop state use existing saved/hashed
fields; gameplay compatibility advances to `0x2026091514` (snapshot version 37).
Strict game/render builds and focused direct checks passed: delayed release,
cancellation, use count, restored windup determinism, crystal destruction, node
interruption, friendly damage, gap/range limits, stone/felt/closed-gate blocking,
nonflammable growths, variants and snapshot round trips. Four statically generated
Ice floors at seed 1701 contained fourteen crystal cells. World, pulse and item
renders were inspected using dummy SDL drivers. No autonomous playthrough or new
permanent test suite.

# Level features: geography, variants, infrastructure and events

2026-09-15 clarification of the user's layered-generation ideas. Implementation
status belongs in [MASTER_TASKS.md](MASTER_TASKS.md). These are descriptive
categories that can combine, not a requirement for five separate frameworks.
Existing context: [world review](WORLD_INTERACTION_REVIEW.md),
[scene candidates](content/SCENE_CANDIDATES.md),
[objective/disaster rules](CONDITIONS_AND_WORLD_FEEDBACK.md#objectives-escape-pressure-and-floor-modifiers).

## What the examples are

| Category | Meaning | Examples |
| --- | --- | --- |
| Landmark | A recognizable composed place | Station building, spider cave, haunted mansion |
| Cross-room feature | Geography laid across existing boundaries | Buried trunk, root seam, blocked watercourse |
| Infrastructure | A route/network laid through the floor, with fixtures and users | Railway/station/trains, two-way road/streetlights/occasional traffic |
| Floor variant or modifier | A change to initial geography or persistent floor rules | Timber Forest with unusually continuous burnable material |
| Triggered floor event | A state change that unfolds during play | Exit crank starts reactor meltdown, forest wildfire, escalating structural collapse |

The nuclear reactor example combines an objective, its machinery/room, and a
triggered floor event. The Timber Forest variant creates conditions for a wildfire;
it need not start burning on arrival. Fire from the player, enemy or another source
can then develop into a floor-wide consequence through shared fire rules. An
authored already-burning forest can instead begin with the event active.

A station is a landmark; the tracks across surrounding rooms are infrastructure;
the trains are active world participants. One design can contribute to several
systems without pretending those are several independently completed encounters.
Infrastructure placement can share cross-room fitting/reservation tools. No extra
per-biome quota is imposed by naming this category. The earlier roughly ten floor
modifiers per biome remains exploratory, not a new forty-feature commitment.

## Industrial exit reactor

Concrete request: player turns a crank to open the exit, destabilizing a reactor.
Candidate consequences include spreading radioactive fire, bursts that destroy
walls across the floor, and a final explosion. Select/tune coherent variants;
not every reactor must do all of them. This elaborates the existing reactor task.

Use explicit states such as stable, crank/activation, warning, disaster and finished.
Define whether cranking takes sustained input, whether it can be interrupted,
and exactly when the gate opens. Show the bargain before activation: escape access
in exchange for a worsening floor. An alarm, changed lights, obvious reactor state
and readable map effects announce what happened. If there is a terminal deadline,
communicate it; do not invent a hidden instant-death timer for gradual spread.

Radioactive fire is gameplay, not just green particles. Define its damage/status,
spread, extinguishing/containment, immunity/resistance and persistence deliberately.
Reuse appropriate fire/hazard mechanisms; a distinct mechanic needs several useful
ways to interact across the content. Walls may carry the disaster faster than
floors as originally requested. Structural bursts can open routes as well as make
them dangerous; distinguish collapse, clearable rubble and impassable remnants.
Keep the usable exit, warning time and viable escape policy intentional.

Propagation and scheduled blasts use a deterministic bounded frontier/queue with
saved progress and no per-render-frame decisions. Bound active work and cosmetic
effects independently. Include actor damage, breakables, fuel, loot, lighting,
navigation changes and snapshot/reconnect in the same authoritative consequences.
Co-op activation happens once; team pause pauses the event, local menus do not.
Resolve peers crossing the exit and late arrivals according to encounter policy.

## Railways and roads across generated rooms

Concrete additions: a station fitted onto a generated floor with actual trains
crossing it, and a rare Forest two-way road with an occasional car/truck and
streetlights. A quiet forestry/service road gives that rare intrusion context.
Vehicles should be occasional events, not a constant traffic blanket.

Fit routes after the base room/landmark geometry, then integrate associated
station/hut/platform pieces, lights, crossings, stock and population. Placement
can follow available corridors or deliberately cut through selected rooms/walls.
Reserve the route before ordinary population; do not silently erase previously
placed keys, exits, actors or shops. Reroute, reject, or explicitly redesign an
intersection that conflicts with protected progression. Revalidate the final map.

Distinguish two kinds of destruction: the generation pass may cut a route into
the initial map; a moving train may later break permitted obstacles in play.
Both can exist, with visible rail/road continuity and defined crossing behavior.
Don't randomly choose between blocking forever and destroying everything.
Some fixtures/terrain may be rammed; structural boundaries need explicit stopping,
turnaround or route endpoint/despawn rules. A vehicle leaving the map is not a
general permission for actors to walk through an unbreakable border.

Road lanes define travel direction, appearance cadence, speed, braking/obstruction
rules, warning distance and actor impact. Train crossings need whistle/lights or
other readable advance notice and somewhere to get clear. Use existing single-cell
actors/linked train segments where fitting; do not make general multi-cell enemy
support a prerequisite. Visible moving parts and authoritative impact space must
agree. Breaking a streetlight extinguishes its actual source and leaves appropriate
debris, sharing the tall-prop system.

## Layering and rarity

Compose when the combination is interesting: a road can interrupt fuel in Timber
Forest and become a firebreak; a fallen tree can obstruct traffic; reactor damage
can sever power to signals or open a train route. These are design candidates,
not claims of implemented interactions. Define overlap through material rules and
small explicit combination handling instead of assuming every pairing works.

Keep placement rare using floor/run budgets shared with other large features,
not many independent per-tile rolls. Permit authored exceptions and debug forcing.
A feature's spatial footprint, chance of appearing, event trigger and runtime
state are separate choices. This lets a station be quiet, active or ruined without
requiring three unrelated generators or three permanent special-case game modes.

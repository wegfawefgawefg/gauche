# Level features: geography, variants, infrastructure and events

2026-09-15 clarification of the user's layered-generation ideas. Implementation
status belongs in [MASTER_TASKS.md](../MASTER_TASKS.md). These are descriptive
categories that can combine, not a requirement for five separate frameworks.
Existing context: [world review](WORLD_INTERACTION_REVIEW.md),
[scene candidates](../content/SCENE_CANDIDATES.md),
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

## Rare fire-response crew

User candidate: a fire engine enters a major burning area with flashing lights
and a siren, parks, and deploys firefighters. They run to fires, throw finite
buckets of temporary water, then scurry back to refill and repeat. This is an
event with participating actors, not a cosmetic truck or immediate floor reset.

Stage the response: eligible incident -> audible approach -> vehicle enters a
legal route -> parks -> crew disembarks -> obtains water -> fights fires/refills
-> finishes, withdraws or becomes stranded according to visible circumstances.
Eligibility should be rare at the floor/run level, possibly tied to a substantial
fire; don't roll on every burning tile or every frame. A rare pre-authored visit
can also be valid. Its entry point must connect to usable terrain and leave a
readable crossing opportunity for players. Reuse infrastructure fitting where
available; no free invisible road should appear across occupied rooms at runtime.

Water sources: onboard finite tank when no source is available; a stream intake
in Forest; a red hydrant or other explicit supply in Industry. Existing streams
and water terrain provide a foundation, but hose/intake/refill behavior is new
work. Show which supply is connected and whether it works. A simple hose link
and refill rules suffice; a continuous fluid simulation is unnecessary. Define
distance, obstruction and disconnection if the truck/source moves or breaks.
Unlimited supply from a stream/hydrant must be a deliberate source property,
not an unnoticed free tank refill. Damaged, empty or unavailable sources must
change what the crew does visibly.

Crew routines can reuse generation-checked truck/source/target handles, cells,
counters and timers in the existing entity style. Reserve/select reachable fires
so everyone doesn't chase the same extinguished cell or jam the truck entrance.
Release stale targets, allow yielding, and leave room for crew return/refill.
Water lands through the shared wetting/extinguishing rules; temporary wetness
has an authoritative lifetime. Members' carried water, refill, quench, movement
and environmental effects synchronize. Siren playback, flashing presentation,
spray droplets and footstep detail are local consequences of the shared state.
If sirens attract enemies, emit a deterministic hearing event separately.

Do not assume firefighters are enemy combatants or count the crew as several
new enemy species. Their allegiance, reaction to harm, exit policy and interaction
with hostile actors are design choices. They can accidentally frustrate the
player's fire plan while helping contain a disaster; they shouldn't automatically
erase all danger just because the rare event appeared.

Ice variants are deliberately very rare: water workers may create durable slippery
ice or explicit ice-block obstructions, becoming annoying rather than helpful.
Freezing a shallow puddle into a slippery surface and creating a body-blocking
wall are different actions with different rules/visuals; do not conflate them.
Ice walls need telegraph, occupant handling and available ways to break/melt/bypass
them, rather than silently trapping players forever. “Permanent” slippery patches
can persist for the floor while still obeying explicit thaw/material rules.
An exceptionally rare joke variant uses literal fire to melt Ice, with unmistakable
equipment/cues so it is not confused with water behaving incorrectly.

## Biome transitions and feature placement windows

User direction: hint at the approaching biome using scenery/materials near the
boundary, and retain a little of the prior biome immediately after crossing.
This is not permission to broaden ordinary enemy or item pools. The separate
native/rare-import loot policy still applies.

Suggested initial profiles:
- Forest stage 3: occasional hints where fitting; features may start spanning the
  thematic transition window without changing the whole floor's identity.
- Forest stage 4: more snow and ice patches, snowy/icy tree silhouettes in the
  overhead vignette, colder vegetation/material accents. Keep readable dark canopy.
- Ice stage 1: small remnants of woodland, roots or forest structures among the Ice.
- Ice stage 4: more thaw, liquid water, steam and occasional visible heat/fire,
  hinting at the coming mines/foundries rather than adding an unexplained warm tint.
- Industrial stage 1: residual ice/snow/meltwater around entrances or machinery,
  transitioning into hot stone, metal and active work areas.
- Fourth-biome transition waits on its chosen identity.

Compose profiles from named biome and stage, optional neighboring biome hints,
material/scenery weights and eligible features. Do not hardcode a whole new biome
for every mixture or apply an indiscriminate global palette blend. Snow on trees,
specific frozen/thawed objects and changing water sources should sell geography.
Cosmetic frost is decoration; an actually slippery/frozen tile needs the shared
gameplay state. Audit rules currently gated only on `ice_floor`: isolated Ice in
Forest or Industry must obey its material/condition behavior deliberately, while
floor-wide climate remains an explicit separate rule.

Give uniques, landmarks, cross-room features and events optional placement windows
and environmental requirements. Example: a frozen logging camp could fit Forest
stages 3–4 and Ice stage 1 (run floors 3–5), but not any random Forest/Ice stage.
Likewise a thawing pumping station can fit the Ice/Industrial boundary. These are
examples for future catalogs, not newly implemented uniques. Express windows using
biome/stage identities where practical so future progression edits do not silently
move an encounter. Matching a window makes a feature eligible, not guaranteed.

Keep caps/rarity across the entire window: three eligible floors must not turn a
rare unique into three appearances. Some features may require roads, real water,
fuel or heat in addition to stage eligibility. Availability, scarcity and actual
placement success are distinct. Expose forced profiles/events in debug tools for
static inspection and later human playtesting, while respecting normal run order.

## Layering and rarity

Small dwelling candidates: a little Forest house, isolated tent, hovel, tree house
or hut in a tree. These can be compact scene pieces within a room or clearing;
they need not each consume a multi-room landmark or unique-floor slot. Use shared
overhead fading for enterable covered interiors. A tree dwelling can suggest
height through art and an explicit one-plane entrance without requiring decks.

The same structure can have different frequency and context by biome: a lone tent
is an occasional Forest find, while clustered tents/encampments could be common
in the provisional military fourth biome. Define biome/stage/source placement
weights and compatible furnishings, inhabitants and loot; do not mark a structure
universally rare just because its first appearance was rare. The fourth-biome
use remains conditional on the final theme. Existing per-feature rarity budgets
apply to major features; ordinary small camps need their own suitable density.

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

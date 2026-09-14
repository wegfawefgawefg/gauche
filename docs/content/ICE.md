# Ice catalog: the drowned observatory

Design specification, not implemented-content claims. Track completion only in
`../MASTER_TASKS.md`. Fifty regional finds supplement shared equipment and the
items carried out of the forest. User playtesting owns final balance.

## Identity and rules

A frozen reservoir surrounds abandoned bathhouses, fishing huts and an
observatory cut into blue stone. Warm maintenance rooms interrupt cold galleries.
The inhabitants want food, warmth, shelter or the machinery still running below.
Use flat slate floors, broad snow banks, dark water and sparse pale fractures.
Bright white is reserved for attack tells, exposed ice edges and small highlights.
No forest canopy, uniform blue screen tint or noisy snow texture on every tile.

Heat, water, footing and sound are the biome's compositional rules. They affect
friends and enemies. Avoid a constant cold meter that taxes players for existing.
Chill is a timed movement penalty; frozen is a separately signaled short root
that does not silently disable inventory use. Heat clears chill/frozen and can
melt designated ice. Water extinguishes fire but does not cure chill. Snow absorbs
an oil spill until burned or cleared; exposed ice slips, gritted ice does not.
Deep water blocks ordinary walkers. Thin ice has explicit integrity and becomes
shallow water on breaking; no surprise bottomless death under an occupied tile.

All gameplay surfaces, thaw timers, cracking ice, enemy memories and moving
objects are authoritative integer state. Steam wisps, drifting snow, ripples,
scraps, decorative fog and ambient audio are local. Visual steam must correspond
to a gameplay smoke surface when it blocks targeting. Sound propagation used by
enemy hearing is gameplay, independent of whether a client plays the sound.

## Twenty adversaries

Each drop row is one roll unless noted. Undropped probability means nothing.
Carried stolen items always return intact. Passive fish and harmless drifting
jellyfish can be additional wildlife; neither counts toward the twenty.

| # | Enemy | Behavior, tell and counterplay | Habitat; drops |
| --- | --- | --- | --- |
| 1 | Rime skater | Pushes off along one cardinal lane and cannot turn until reaching rough ground or stopping. Scraping blades precede departure; bait it into a bank or another creature. Implemented: 50 HP, 0.5s tell, 18 contact damage. | Reservoir; 20% grit pouch, 10% skate blade. |
| 2 | Bell diver | Implemented: 72 HP; swims between visible air holes, rings its brass helmet for 0.6s, emerges for 0.4s, then winds up an 18-damage swing for 0.5s. Blocking a hole redirects it; never attacks submerged. | Reservoir/fishing huts; 20% air bladder, 25% 2–5 gold implemented. |
| 3 | Snow burrower | A moving mound exposes its route; bursts at its committed destination and remains exposed while digging back in. Cleared ground prevents reburrowing there. | Snow banks; 25% raw meat, 15% snow scoop. |
| 4 | Ice mason | Carries one visible block, places temporary breakable cover between itself and a threat, then retreats to cut another from eligible ice. Interrupt its long cutting beat. | Quarry; 20% ice brick, 15% chisel. |
| 5 | Mirror knight | A shield reflects frontal projectiles during a visible raised stance; attacks require lowering it. Flank, shove or strike during recovery. Reflection has the existing finite flight lifetime. | Observatory; 20% mirror shard, 20% gold. |
| 6 | Steam leech | Crawls toward heat, latches adjacent to an emitter and temporarily suppresses its light/fuel output. Swells before a scalding burst; cold or a shove detaches it. | Boiler rooms; 20% heat capsule, 10% sealant. |
| 7 | Glass eel | Lives in connected water, charges visibly, then shocks a short wet path. Leave the water, interrupt charging or break connectivity; dry tiles stop propagation. | Reservoir; 20% eel battery, 15% raw meat. |
| 8 | Avalanche ram | Pawing marks a short lane; a headbutt shoves an actor and breaks fragile snow banks. A wall collision stuns the ram. Short attack range distinguishes it from the forest boar. | Cliff paths; 35% two raw meat, 10% wool wrap. |
| 9 | Frost bat | Hangs still, exhales a narrow traveling frost puff, then relocates to another perch. Implemented: 24 HP, 0.5s inhale, six-cell traveling puff for 6 damage and 2s chill; flames dissolve it and shields block it. | Ice caves; 15% ice needle. |
| 10 | Snow effigy | Inert unless watched from nearby; turns and takes a step only after observers look away. Its twig face lights before an adjacent strike. Fire exposes a frail frame. | Memorial court; 20% candle stub, 10% effigy mask. |
| 11 | Fishing widow | Casts a visible hook, reels the first hooked actor toward her, then pauses to untangle line. Crossing behind cover severs the tether; hook can catch an ally of hers. | Fishing huts; 20% fishing line, 20% smoked fish. |
| 12 | Candle keeper | Relights a bounded set of nearby candles and protects the nearest one. Its flame strike dims its own lamp; stealing or wetting a candle redirects its attention. | Chapel; 30% candle stub, 15% wick spool. |
| 13 | Shard colony | Three separate crystals connect with dangerous pulses only after all endpoints flash. Destroy or displace one node to break that connection. Nodes share a colony handle. | Crystal gallery; 25% crystal lens per colony, not per node. |
| 14 | Whiteout drummer | Beats three audible cues before a short snow squall obscures targeting in a marked area. Cannot attack directly; interrupt or use its squall as cover. | Weather station; 20% muffling felt, 15% signal flare. |
| 15 | Seal thief | Slides in to steal a loose fish or food item, then retreats to a reachable water edge. Does not delete stolen food instantly; biting follows a warning bark. | Landing; carried food, 25% raw meat. |
| 16 | Icicle spider | Anchors a strand to a wall, stretches it across a corridor, then waits beside it. Strand roots briefly and can be cut or burned; spider must rebuild after triggering. | Service passages; 20% fishing line, 10% ice needle. |
| 17 | Boiler porter | Pushes a steaming tank one cell at a time. A leaking pressure cue precedes a directional vent; rupture sends hot water along open cells, including toward other enemies. | Bathhouse; 25% pressure valve, 20% coal lump. |
| 18 | Echo hound | Investigates propagated footsteps and weapon sounds, commits to the last heard cell, then listens again. Stillness and thrown noise can fool it; no omniscient chase through walls. | Dark tunnels; 25% raw meat, 10% muffling felt. |
| 19 | Frozen pilgrim | Limps toward warmth, thaws into a quick but fragile attacker, then refreezes after leaving heat. Both changes are visible windups; cold can slow it again. | Shelter/chapel; 25% gold, 15% hot broth. |
| 20 | Lens warden | Rotates a visible mirror toward a fixed lamp, locks a reflected cardinal lane, then fires. Break sight, move the mirror or get behind it during the turning beat. | Observatory vault; 35% lens carbine, otherwise 4–8 gold. |

Reuse shared counters, timers, points and generation-checked handles. Document
state meanings beside each species. Colonies and porters need bounded helper
actors only when those helpers move, target or hurt things; ordinary candles,
strands, blocks and lamps belong in prop/surface/fixture storage.

## Fifty regional finds

Values are first-pass design targets. Times below are seconds at 60 Hz, stored as
integer ticks. Counts distinguish stack capacity from charges or durability.
Every gun has its own magazine/reserve. No shared pistol/rocket ammunition pool.
Projectiles travel unless explicitly described as instant. Failed placement or
an inapplicable repair consumes nothing. Patterns and comparisons derive from the
same effective item definition as attacks, including rare attributes.

| # | Item | Distinct use and initial state | Rarity; price |
| --- | --- | --- | --- |
| 1 | Ice needle | Implemented: recoverable straight throw to 7, 8 damage and 1s chill; melts on a hot landing. Stack 8. Strong/Agile/Heavy/Long keep their effects after recovery. | Common; 4 |
| 2 | Chisel | Adjacent 9-damage jab, dig power 1, 0.3s cooldown; double tile damage to ice. 40 durability. | Common; 12 |
| 3 | Skate blade | Three-cell lateral slash, 12 damage; striking while slipping extends the slash one cell ahead. 36 durability. | Common; 16 |
| 4 | Harpoon gun | One shot loaded, 28 damage to range 10; leaves a tether. Holding use reels the victim until blocked; reload releases it. | Uncommon; 32 |
| 5 | Lens carbine | Instant 18-damage line to 12, three charges per magazine; crosses clear crystal but stops at opaque cover. Mirrors redirect cardinally. | Rare; 40 |
| 6 | Snowball | Thrown to 5, 1 damage; wets the target and interrupts a fragile charging action. Stack 12. | Common; 2 |
| 7 | Ice brick | Place blocking 35-HP cover or throw to 3 for 14 damage; thrown bricks shatter, heat melts placed ones. Stack 3. | Common; 8 |
| 8 | Cold flask | Throw to 5; chills actors in a cross, freezes shallow water for 8s, extinguishes small fires. Stack 3. | Common; 10 |
| 9 | Grit pouch | Fans across three adjacent ice tiles; stops slipping until washed away. Six uses; Big covers five, Durable supplies twelve. Oil still slips. No universal terrain immunity. | Common; 6 |
| 10 | Snow scoop | Clears a three-cell fan of snow, revealing covered loot/props; secondary packs collected snow into a snowball if space permits. 60 durability. | Common; 9 |
| 11 | Heat capsule | Break next to the user for a 4s warm patch; thaws nearby ice and removes chill, but ignites oil. Stack 4. | Common; 8 |
| 12 | Coal lump | Feed a lit stove for longer heat or throw an inert lump for 4 damage. Does not spontaneously ignite. Stack 8. | Common; 3 |
| 13 | Candle stub | Place a small finite lamp; pick it back up with remaining fuel. Eighty seconds of light per candle, stack only equal unused candles. | Common; 5 |
| 14 | Wick spool | Adds 30s to an adjacent fuel lamp or candle; cannot refill batteries or living emitters. Four portions. | Common; 7 |
| 15 | Signal flare | Fires to 8 and burns where it lands for 15s; bright red light, hot contact and noise on launch. Stack 3. | Common; 10 |
| 16 | Storm lantern | Aim to shutter its directional light; secondary toggles shutter fully closed. No damage; 120s fuel, conserved while shut. | Uncommon; 20 |
| 17 | Heat siphon | Transfer up to 5s of burn/fuel from an adjacent source into a carried charge, then discharge a short flame cone. Six charge capacity. | Rare; 34 |
| 18 | Air bladder | Implemented: shove adjacent actors one cell outward, or aim at loose shallow-water loot to attach a float. Travels straight up to 16 cells, stopping at shore or obstruction. Three uses; Big widens pulse, Durable doubles uses. | Common; 10 |
| 19 | Fishing line | Hook loose items across water within 6 and drag them back; unlike a harpoon, cannot pull actors. 20 uses. | Common; 8 |
| 20 | Pressure valve | Attach to an adjacent vent to redirect its next bursts toward the aimed direction; recoverable while cool. | Uncommon; 18 |
| 21 | Sealant | Patch a leaking tank or temporarily plug a water/steam outlet for 10s. Three portions; pressure remains a hazard on reopening. | Common; 9 |
| 22 | Steam kettle | Fill in shallow water; heat beside fire, then release a short scalding cone for 12 damage and wetting. One carried water charge. | Uncommon; 22 |
| 23 | Eel battery | Discharge adjacent for 14 damage; connected wet cells carry weaker shock up to four steps. Three charges. Friends conduct too. | Uncommon; 24 |
| 24 | Copper wire | Lay up to six linked conductive cells; transfers a received shock, consumes a segment per cell. No idle damage or free power. | Common; 11 |
| 25 | Grounding spike | Plant to absorb one nearby conducted shock and leave a hot spent spike; recover when cool. Stack 3. | Uncommon; 15 |
| 26 | Mirror shard | Place a fragile diagonal reflector; secondary rotates it. Returns eligible beams, not arbitrary bullets or explosions. Stack 3. | Uncommon; 16 |
| 27 | Crystal lens | Place to split one incoming cardinal beam into two weaker perpendicular beams; 18 HP. Each beam has a finite shared reflection budget. | Rare; 30 |
| 28 | Prism bomb | Throw with 1.5s fuse; bursts four short light beams for 16 damage each. Mirrors/lenses change those paths. Stack 2. | Rare; 28 |
| 29 | Black felt | Cover one adjacent reflector or light fixture until removed/burned. Three pieces; useful for dark routes and stopping beam puzzles. | Common; 7 |
| 30 | Muffling felt | Wrap held equipment to suppress its next six ordinary use-noise events; impacts and explosions remain audible. Two wraps. | Uncommon; 14 |
| 31 | Echo pebble | Throw to 7; repeats the last nearby non-explosion use-noise three times from its landing. No duplicate gameplay attack. Stack 3. | Uncommon; 16 |
| 32 | Tuning fork | Short 0.5s windup sends a straight vibration through contiguous brittle crystal to 8; breaks weak crystal, 8 actor damage. 24 uses. | Uncommon; 22 |
| 33 | Alarm clock | Wind and place a 3s delayed repeated ring; wakes/investigates for 5s, then becomes recoverable. No stun. | Common; 10 |
| 34 | Wool wrap | Self: clears chill and resists new chill for 8s; catches fire readily and burns away its protection. Three wraps. | Common; 8 |
| 35 | Hot broth | Self: 12 HP over 4s and removes chill. Three servings; interrupted regeneration does not refund consumed food. | Common; 9 |
| 36 | Smoked fish | Self: 14 immediate HP, or drop as strong seal/eel bait. Stack 6. | Common; 6 |
| 37 | Salted kelp | Eat to clear nausea, at a cost of 3 HP; placed kelp is bait for reservoir wildlife. Stack 5. | Common; 4 |
| 38 | Ice poultice | Self: stops burning and grants 10 HP over 5s, but chills for 3s. Stack 4. | Common; 7 |
| 39 | Flask of brine | Throw a small puddle that melts fragile ice and resists refreezing for 8s. Causes 4 damage on an open wound; stack 3. | Common; 9 |
| 40 | Snow shelter | Place a two-cell breakable windbreak, only if both cells are free; blocks wind/snow attacks, not overhead shots. Two uses. | Uncommon; 18 |
| 41 | Crampons | Activate for 5s of traction: no ice slide, slower ordinary steps, still vulnerable to deliberate shoves. Six activations. | Uncommon; 17 |
| 42 | Sled | Deploy facing forward; step onto it to slide until stopped, taking one carried loose item along. Steer only while stopped. 45 HP. | Uncommon; 25 |
| 43 | Ice anchor | Place a tether point, then activate to return along the clear tether up to 5 cells. Obstructions stop travel; hazards apply to each step. | Rare; 32 |
| 44 | Folded bridge | Place three walkable planks across shallow/deep water; requires support at both ends. Burnable, 30 HP per section. One kit. | Uncommon; 24 |
| 45 | Thaw charge | Place a 2s fuse against a wall; opens only heat-fragile ice in a two-cell line. Steam hurts adjacent actors for 10. Stack 3. | Uncommon; 19 |
| 46 | Effigy mask | Hold still while facing to draw watching effigies' attention as though another observer were present; consumes one of 12 charges per second. | Rare; 28 |
| 47 | Snow globe | Place a 6s local whiteout that obscures targeting for all sides; the glass breaks after one use. Stack 2. | Uncommon; 20 |
| 48 | Borrowed summer | A 4s moving warmth aura thaws surfaces and clears allied chill; cold creatures react to the real heat source. Two uses, no damage immunity. | Rare; 38 |
| 49 | Stillwater bell | Ring to stop surface currents and active slips within 4 for 3s; does not stun actors or halt voluntary movement. Four uses. | Rare; 30 |
| 50 | Emergency doorstop | Jam an adjacent moving gate open until the 25-HP wedge breaks; only works while that gate is open. Recoverable; no opening a locked gate for free. | Uncommon; 21 |

Do not introduce a permanent stat for each interaction. Use existing HP, movement
beat, damage, cooldown, stack/use count and item condition. Add statuses only with
a readable icon, remaining duration and concrete effect in the detail panel.
Rare Big/Long/Heavy/Durable attributes need explicit sensible support per item;
unsupported attributes must not appear just to inflate a reward pool.

## Rooms, routes and four-floor progression

1. Reservoir approach: dry connected banks, small ice patches, fishing supplies;
   introduce skaters and divers separately before combining them.
2. Bathhouse works: warm/cold branches, water valves and optional pressure routes;
   introduce leeches, porters and eels with visible alternatives to combat.
3. A special layout roll: thawing ferry landing, the silent chapel, or a premade
   observatory annex. Specials change room rules, not just the enemy count.
4. Observatory ascent: mirrors, shutters and interrupted sightlines; combine
   familiar threats around a guarded exit without making every room an arena.

Route graph owns entry, exit, key/switch dependencies and optional loops. Geometry
uses bent galleries, paired reservoir banks, courtyard shelters, narrow service
routes and broad observatory rooms. Every required lock cuts all ordinary routes;
place its key/switch on the reachable side. Bridge kits, heat tools and mirrors
may create shortcuts but are never mandatory random loot for a required route.
Supply enough dry space for all joined players to spawn and regroup.

Room roles: landing (food/bait), fishing hut (hooks/line), frozen quarry
(mason/ice tools), bathhouse (heat/water), boiler gallery (pressure/coal), reservoir
(eels/divers), shelter (medicine/candle), chapel (keeper/effigy), crystal garden
(colony/reflection), weather station (drummer/snow), mirror hall (knight), vault
(warden), snow drift (burrower/cache), echo tunnel (hound/noisemakers), cliff path
(ram/traction), wrecked ferry (wood/bridge), maintenance duct (spider/sealant),
warm pantry (pilgrim/food), quiet memorial (optional gold), hidden lens workshop
(rare beam utilities). Mix complementary roles, not twenty required rooms a floor.

Encounter examples: one eel threatens a wet approach to an otherwise harmless
porter; a keeper relights a lamp that thaws pilgrims; an echo hound searches the
noise made by an ice mason; a mirror knight accidentally redirects a warden shot.
Never silently summon an infinite colony, refill a nest, or duplicate fuel/loot.

## Containers and drops

Roll contents once, preserve item instances, and leave debris on opening/breaking.
Rarity and coin budgets are floor-level constraints, not guaranteed bonus loot
for every prop. Furniture that looks like a source of wood should leave wood.

| Container | Initial weighted pool | Placement |
| --- | --- | --- |
| Fishing creel | 35 smoked fish, 20 line, 15 air bladder, 30 empty | Huts and landing |
| Frozen lunch tin | 30 broth, 20 kelp, 20 poultice, 30 empty | Shelters; heat or ordinary damage opens |
| Maintenance locker | 20 coal, 20 sealant, 15 valve, 15 ammo, 30 empty | Boiler gallery |
| Candle cabinet | 35 stub, 20 wick, 15 wool, 30 empty | Chapel/shelter |
| Buried pack | 20 grit, 15 scoop, 20 bandage, 15 gold, 30 empty | Visible snow mound; no pixel hunting |
| Lens case | 30 shard, 20 lens, 15 black felt, 15 gold, 20 empty | Guarded observatory |
| Locked expedition chest | 40 regional weapon, 35 regional utility, 25 medicine; plus 6–12 gold | Optional key branch |

Ordinary shops offer medicine, fuel, ammo and two local tools. Rare shops can
offer a beam or route-breaking item at its listed initial price. Keep a few
shared supplies available without replacing regional identity with forest stock.

## Twenty local debris types

Snow clump, ice flake, clear shard, blue shard, frost dust, icicle tip, slate chip,
wet wood splinter, rope fiber, fishing float, fish scale, kelp scrap, wool tuft,
candle wax, charred wick, coal crumb, brass rivet, broken glass, copper curl and
painted ceramic. Give each a small silhouette and appropriate source, friction,
wind response and lifetime. Snow/ice may visually melt near heat; those cosmetic
pieces cannot create water, block light, change traction or damage anything.
Use the existing local debris pool and wall collision, not a second physics world.

## Twenty ambient cues and small scenes

Produce new offline source scripts and committed audio. Positional loops use
volume/falloff with short crossfades and voice limits; enter triggers use local
cooldowns. Global cues use elapsed local time and seeded local randomness, never
a per-render-frame chance that changes with frame rate. No gameplay dependence.

| # | Cue | Playback and scene |
| --- | --- | --- |
| 1 | Ice groan | Rare positional one-shot from a broad reservoir, separate from actual cracking tells. |
| 2 | Water under ice | Low continuous positional loop at an exposed pool. |
| 3 | Dripping thaw | Positional loop below a warm ceiling; tiny cosmetic splash circles. |
| 4 | Thin wind | Level bed outdoors, ducked indoors; quiet enough to hear windups. |
| 5 | Window whistle | Narrow positional loop at a broken window. |
| 6 | Hanging chain | Cooldown trigger near a ferry mooring. |
| 7 | Distant bell | Sparse global cue with a long cooldown; no gameplay hearing event. |
| 8 | Timber creak | Once on entering a shelter, reusable after leaving for a long interval. |
| 9 | Snow settling | Sparse local cue beside a drift with a little falling powder. |
| 10 | Boiler idle | Positional loop tied to a working boiler fixture. |
| 11 | Pressure hiss | Positional loop at a harmless leak; damaging vents have distinct timed attack cues. |
| 12 | Pipe knock | Room-local cooldown event along maintenance pipes. |
| 13 | Waterwheel | Positional mechanical loop at a half-frozen spillway. |
| 14 | Slush lap | Gentle positional loop on reservoir banks. |
| 15 | Glass tinkle | Entry cue in a broken lens workshop. |
| 16 | Observatory motor | Quiet positional loop from rotating decorative machinery. |
| 17 | Cloth flutter | Positional loop at a torn expedition flag. |
| 18 | Far animal call | Rare global outdoor cue, distinct from nearby enemy attack tells. |
| 19 | Chimney draft | Positional loop beside a warm stove and visible smoke outlet. |
| 20 | Submerged knock | Rare positional event at a sealed diving hatch; no phantom damage or enemy spawn. |

Snow steps, ice slips, water wading, crystal breaks, flesh impacts, metal parries,
food eating, reloads and every enemy tell need action sounds in addition to these
twenty ambience cues. Keep variation/headroom and preserve directional playback.

## Implementation order and acceptance

Implement shared terrain/material rules and regional selection first; no change
to the existing forest's floor identities by accident. Current runtime order is
forest/fire/ice and only three zones; integrating all four must explicitly update
HUD, generation, stock, rewards, end-of-run rules and compatibility together.
The design does not pretend the present placeholder ice floors are complete.

Then implement coherent room slices: reservoir footing and skater; fishing and
diver; heat and bathhouse; snow and listening; crystal/reflection and observatory.
Each slice includes bespoke behavior, source sprites, new sounds, sensible loot,
item UI/patterns, world interactions and deterministic state serialization.
Keep descriptions and actual behavior aligned; a table entry is not completion.
Use strict builds and static captures of assets/room compositions. Record any
behavior still awaiting the user's playtest without silently checking it off.

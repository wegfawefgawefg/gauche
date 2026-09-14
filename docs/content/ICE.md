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
| 3 | Snow burrower | Implemented: 36 HP; moves as a visible snow mound, commits an adjacent bite for 0.45s then deals 14 damage and stays exposed for 1.25s. A 0.5s dive needs dry snow. Hits, sleep, stun or clearing its snow expose it for 1.5s. | Quarry snow banks; 25% raw meat, 15% snow scoop, one roll. |
| 4 | Ice mason | Implemented: 60 HP; places carried 35-HP cover after 0.5s, cuts replacement ice for 1.5s, and defends with a 0.4s jab for 12 damage. Damage, sleep, stun or displacement interrupts work. | Quarry; 20% ice brick, 15% chisel, 25% 2–4 gold, one roll. |
| 5 | Mirror knight | Implemented: 90 HP, 18-tick shield raise, 60-tick fixed-facing guard; reflects eligible shots and beams. Lowers for a 27-tick, 22-damage sword tell. Damage, shove, sleep or stun cancel the stance. | Observatory; 20% mirror shard, 20% gold. |
| 6 | Steam leech | Implemented: 38 HP; seeks exposed flame or warm terrain, latches for 0.3s, feeds for 2s, then swells for 0.6s before a 12-damage cross. Drains light/heat output; cold, sleep, stun or displacement releases it. | Bathhouse/shelter; 20% heat capsule. Boiler rooms and 10% sealant await that content. |
| 7 | Glass eel | Implemented: 42 HP, swims connected water; 0.8s visible charge then 18-damage pulse, weakening along up to four wet steps. Damage, chill, sleep, stun or displacement interrupts it. Dry/frozen ground cuts the circuit. | Reservoir/bathhouse; 20% eel battery, 15% raw meat, one roll. |
| 8 | Avalanche ram | Implemented: 76 HP, 0.6s pawing tell then a fixed two-cell lunge at six ticks/cell. 12 damage and one-cell shove; ploughs snow and ice blocks. Solid cover or a facing buckler stuns it for 1.25s. Damage/sleep/stun/displacement interrupt. | Cliff paths; implemented single roll: 35% two raw meat, 10% wool wrap. |
| 9 | Frost bat | Hangs still, exhales a narrow traveling frost puff, then relocates to another perch. Implemented: 24 HP, 0.5s inhale, six-cell traveling puff for 6 damage and 2s chill; flames dissolve it and shields block it. | Ice caves; 15% ice needle. |
| 10 | Snow effigy | Implemented behavior: 60 HP; wakes on first observation or disturbance. Any awake player watching within seven tiles holds it still. Looking away permits eighteen-tick steps and a 0.5s fixed 16-damage strike; looking back cancels it. Heat exposes an 18-HP frame. | Memorial court implemented. Implemented 20% candle stub drop; the planned additional 10% mask remains pending that item. |
| 11 | Fishing widow | Implemented: 64 HP, 0.6s fixed-lane tell, six-tile traveling hook for 14 damage. Pulls the first struck actor every ten ticks, then untangles for 1.5s. Sidestep, cover or interrupt her to cut the line; allies can be hooked. | Fishing huts; 20% fishing line, 20% smoked fish. |
| 12 | Candle keeper | Implemented: 56 HP; tends reachable candles within eight of home, relights a dry fueled wick after 0.6s. Guards lamps; witnessed theft provokes 5s pursuit inside its territory. A fixed two-cell flame tell lasts 0.6s, then deals 10 damage plus up to 8 burning damage; its lamp goes dark for 3s. Water suppresses the lamp. | Chapel implemented; one roll: 30% candle stub, 15% wick spool. |
| 13 | Shard colony | Implemented: three 28-HP crystals share a colony handle and 0.8s warning. Valid connections pulse once for 18 damage, then recover for 2s. Cover cuts links; damage, sleep, stun, root or displacement disable an endpoint. Surviving pairs keep working; one lone crystal cannot pulse. | Crystal gallery implemented; 25% crystal lens on the final node only. |
| 14 | Whiteout drummer | Implemented: 44 HP; three beats, 0.4s apart, then a four-second radius-2 squall at a fixed target within six cells. No direct damage. Damage, sleep, stun or displacement interrupts the 1.2s buildup. | Weather station; 20% muffling felt. Signal flare drop awaits that item. |
| 15 | Seal thief | Implemented: 56 HP; steals one loose food, prefers fish, then takes seven-tick steps to a reachable bank. Holds food for 3s before eating. Adjacent threats provoke a 0.5s bark then 16-damage fixed bite; damage/sleep/stun/displacement interrupts. | Reservoir/fishing huts; carried food returned intact, independent 25% raw meat. |
| 16 | Icicle spider | Anchors a strand to a wall, stretches it across a corridor, then waits beside it. Strand roots briefly and can be cut or burned; spider must rebuild after triggering. | Service passages; 20% fishing line, 10% ice needle. |
| 17 | Boiler porter | Pushes a steaming tank one cell at a time. A leaking pressure cue precedes a directional vent; rupture sends hot water along open cells, including toward other enemies. | Bathhouse; 25% pressure valve, 20% coal lump. |
| 18 | Echo hound | Implemented: 44 HP, nine-tick steps; commits to a heard cell for up to 3s, warns 0.4s before a 14-damage bite, then listens again. Snow quiets steps; water amplifies them. Stillness and thrown noise can fool it. | Echo tunnels; 25% raw meat, 10% muffling felt, one roll. |
| 19 | Frozen pilgrim | Implemented: 48 HP; frozen crust halves ordinary damage, 36-tick steps toward reachable warmth. Thaws for 0.8s into an eight-tick walker, with a 0.3s tell for 16 damage. Four seconds away from heat starts 0.6s refreezing; chill forces it early. HP persists. | Shelter; late bathhouse. 25% 2–5 gold, 15% hot broth, one roll. |
| 20 | Lens warden | Implemented: 64 HP; turns a station mirror for 0.5s, charges its physical lamp for 0.8s, then fires a 26-damage reflected beam. Damage, displacement or interrupted apparatus cancels it. | Observatory vault; 35% lens carbine, otherwise 4–8 gold. |

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
| 2 | Chisel | Implemented: 9-damage jab with 0.05s windup, dig power 1, 0.3s cooldown; double damage to ice-material walls and ice blocks. 40 condition, wears per swing and accepts glue. | Common; 12 |
| 3 | Skate blade | Three-cell lateral slash, 12 damage; striking while slipping extends the slash one cell ahead. 36 durability. | Common; 16 |
| 4 | Harpoon gun | One shot loaded, 28 damage to range 10; leaves a tether. Holding use reels the victim until blocked; reload releases it. | Uncommon; 32 |
| 5 | Lens carbine | Implemented: instant 18-damage beam to 12, three loaded/six spare; 0.4s cooldown, 1.33s reload. Mirrors turn it, lenses split it, opaque cover stops it. Returned light can hit the shooter. | Rare; 40 |
| 6 | Snowball | Implemented: 1-damage throw to 5 at 5 ticks/cell; breaks into a 2s wet patch and quenches the impact cell. Damage disrupts fragile windups. Stack 12; Long reaches 9. Shields/parries and All Piercing retain their rules. | Common; 2 |
| 7 | Ice brick | Implemented: tap-release places adjacent 35-HP cover for 10s; hold 0.3s-release throws to 3 for 14 damage. Heat melts it; impact shatters it. Stack 3. Long throws to 7; Strong/Heavy increase thrown damage. | Common; 8 |
| 8 | Cold flask | Implemented: traveling throw to 5, 3s chill in a cross, 8s shallow-water ice, douses small fires. Stack 3; Big extends each arm, Long throws to 9. | Common; 10 |
| 9 | Grit pouch | Fans across three adjacent ice tiles; stops slipping until washed away. Six uses; Big covers five, Durable supplies twelve. Oil still slips. No universal terrain immunity. | Common; 6 |
| 10 | Snow scoop | Implemented: clears three adjacent snow cells, exposing burrowers and snow-cache supplies. Holds 12 snow; secondary packs one normal snowball if inventory has room. 60 repairable condition, one wear per successful scoop. Big clears five cells; Durable has 120 condition. | Common; 9 |
| 11 | Heat capsule | Implemented: a 4s warm cross around the user; thaws ice, clears chill and resists cold, but ignites oil even when spilled later. Stack 4; Big extends each arm. | Common; 8 |
| 12 | Coal lump | Implemented: feed an adjacent stove +20s fuel, capped at 120s. Cold stoves need separate ignition. Otherwise throw to 6 at four ticks/cell for 4 damage and recover the lump. Stack 8. | Common; 3 |
| 13 | Candle stub | Implemented: place an 80s lamp with 6 HP (Durable 12), recover remaining fuel and condition. Only equal unused candles stack, up to four. Water snuffs it; warmth thaws nearby ice. | Common; 5 |
| 14 | Wick spool | Implemented: add up to 30s fuel to an adjacent candle, capped at 80s. Four portions (Durable eight); no repair or automatic relight. No electrical or living emitter refill. | Common; 7 |
| 15 | Signal flare | Fires to 8 and burns where it lands for 15s; bright red light, hot contact and noise on launch. Stack 3. | Common; 10 |
| 16 | Storm lantern | Aim to shutter its directional light; secondary toggles shutter fully closed. No damage; 120s fuel, conserved while shut. | Uncommon; 20 |
| 17 | Heat siphon | Transfer up to 5s of burn/fuel from an adjacent source into a carried charge, then discharge a short flame cone. Six charge capacity. | Rare; 34 |
| 18 | Air bladder | Implemented: shove adjacent actors one cell outward, or aim at loose shallow-water loot to attach a float. Travels straight up to 16 cells, stopping at shore or obstruction. Three uses; Big widens pulse, Durable doubles uses. | Common; 10 |
| 19 | Fishing line | Implemented: visible hook to 6, reels the actual loose item to your feet at four ticks/cell. Stay still; cover, actors or displaced cargo cut the line. 20 casts. Long reaches 10; Durable gives 40. No damage or actor pulling. | Common; 8 |
| 20 | Pressure valve | Attach to an adjacent vent to redirect its next bursts toward the aimed direction; recoverable while cool. | Uncommon; 18 |
| 21 | Sealant | Patch a leaking tank or temporarily plug a water/steam outlet for 10s. Three portions; pressure remains a hazard on reopening. | Common; 9 |
| 22 | Steam kettle | Implemented: fill at fresh water; heat for 1.5s near warmth. Hot for 30s away from heat; wading cools. One-charge cone deals 12 damage including friends and wets for 5s; cold water douses. Reusable. | Uncommon; 22 |
| 23 | Eel battery | Implemented: adjacent 14-damage contact, weakening along four connected wet steps; 0.75s cooldown, three charges. Hits friends and the user through water. Eels are immune. Big reaches five wet steps; Durable has six charges. | Uncommon; 24 |
| 24 | Copper wire | Lay up to six linked conductive cells; transfers a received shock, consumes a segment per cell. No idle damage or free power. | Common; 11 |
| 25 | Grounding spike | Plant to absorb one nearby conducted shock and leave a hot spent spike; recover when cool. Stack 3. | Uncommon; 15 |
| 26 | Mirror shard | Implemented: place an 8-HP diagonal beam reflector. Secondary rotates it with a shard or empty hand. Bullets break it; footsteps do not. Stack 3. | Uncommon; 16 |
| 27 | Crystal lens | Implemented: place an 18-HP nonblocking lens. Splits an incoming beam into two perpendicular half-damage branches, sharing one finite travel budget. Bullets break it; consumes on placement. | Rare; 30 |
| 28 | Prism bomb | Implemented: throw to 3 with 1.5s fuse from launch; four range-4 beams, 16 damage each, one shared bounded trace. Mirrors/lenses/knights redirect them. Big extends beams; Long extends throw. Stack 2. | Rare; 28 |
| 29 | Black felt | Implemented: cover an adjacent mirror, lens or beam lamp. Stops light beams, disables lamp emission and interrupts its warden. Stack 3; consumes one. Fire burns it off; secondary with felt or an empty hand tears it off without refund. | Common; 7 |
| 30 | Muffling felt | Implemented: wrap the next unwrapped melee weapon or gun in inventory slot order. Six quiet uses; impacts/explosions stay loud. Two applications, or four with Durable. Remaining quiet uses stay on dropped/traded weapons. | Uncommon; 14 |
| 31 | Echo pebble | Throw to 7; repeats the last nearby non-explosion use-noise three times from its landing. No duplicate gameplay attack. Stack 3. | Uncommon; 16 |
| 32 | Tuning fork | Short 0.5s windup sends a straight vibration through contiguous brittle crystal to 8; breaks weak crystal, 8 actor damage. 24 uses. | Uncommon; 22 |
| 33 | Alarm clock | Implemented: place, wait 3s, then five once-per-second radius-10 acoustic pulses. Recover after 5s, keeping damage. 8 HP, or 16 Durable; attacks can destroy it. No stun. | Common; 10 |
| 34 | Wool wrap | Implemented: clears chill and resists new chill for 8s; ignition burns away protection and sustains a 5s weak burn. Cannot apply while burning or already wrapped. Stack 3. | Common; 8 |
| 35 | Hot broth | Implemented: 12 HP over 4s and removes chill. Any actual damage ends recovery; blocked hits do not. Stack 3. Restorative: 18 HP over 6s. | Common; 9 |
| 36 | Smoked fish | Implemented: 14 immediate HP (21 Restorative), stack 6. Dropped fish lures eels within six connected water steps and land meat-eaters within nine tiles. Does not cancel a committed shock. Seals prefer it within eight reachable steps. | Common; 6 |
| 37 | Salted kelp | Implemented: clears nausea for 3 HP (can kill). No use without nausea. Stack 5; eels and seals eat dropped kelp, paying the same cost. | Common; 4 |
| 38 | Ice poultice | Implemented: stops burning and grants 10 HP over 5s, but chills for 3s; Wool Wrap prevents that chill. Stack 4. Restorative: 15 HP over 7.5s. | Common; 7 |
| 39 | Flask of brine | Implemented: visible throw to 5, radius-1 splash. Melts floor/prop ice, leaves 8s wet antifreeze; stings already-wounded actors for 4. Stack 3. Big widens; Long throws farther. | Common; 9 |
| 40 | Snow shelter | Place a two-cell breakable windbreak, only if both cells are free; blocks wind/snow attacks, not overhead shots. Two uses. | Uncommon; 18 |
| 41 | Crampons | Activate for 5s of traction: no ice slide, slower ordinary steps, still vulnerable to deliberate shoves. Six activations. | Uncommon; 17 |
| 42 | Sled | Deploy facing forward; step onto it to slide until stopped, taking one carried loose item along. Steer only while stopped. 45 HP. | Uncommon; 25 |
| 43 | Ice anchor | Place a tether point, then activate to return along the clear tether up to 5 cells. Obstructions stop travel; hazards apply to each step. | Rare; 32 |
| 44 | Folded bridge | Place three walkable planks across shallow/deep water; requires support at both ends. Burnable, 30 HP per section. One kit. | Uncommon; 24 |
| 45 | Thaw charge | Place a 2s fuse against a wall; opens only heat-fragile ice in a two-cell line. Steam hurts adjacent actors for 10. Stack 3. | Uncommon; 19 |
| 46 | Effigy mask | Hold still while facing to draw watching effigies' attention as though another observer were present; consumes one of 12 charges per second. | Rare; 28 |
| 47 | Snow globe | Implemented: break in the adjacent cell for 6s of radius-2 whiteout. Blocks sight for all sides, not shots; no damage/chill. Walls constrain spread. Stack 2; Big gives radius 3. | Uncommon; 20 |
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
| Fishing creel | Implemented: 35 smoked fish, 20 line, 15 air bladder, 30 empty; one roll on breaking | Fishing huts; 12 HP, blocking, burnable woven basket |
| Frozen lunch tin | Implemented: 30 broth, 20 kelp, 20 poultice, 30 empty | Shelters; 16 HP, blocking, not fuel. Warmth or ordinary damage opens once. |
| Maintenance locker | 20 coal, 20 sealant, 15 valve, 15 ammo, 30 empty | Boiler gallery |
| Candle cabinet | Implemented: 35 stub, 20 wick, 15 wool, 30 empty | Chapel alcoves; 18 HP, blocking, burnable wood. Shelter placement remains optional future work. |
| Buried pack | 20 grit, 15 scoop, 20 bandage, 15 gold, 30 empty | Visible snow mound; no pixel hunting |
| Lens case | Implemented: 30 shard, 20 lens, 15 three-piece black felt, 15 gold (3–6), 20 empty | Guarded observatory; 16 HP, blocking, breakable wood |
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
Eighteen regional debris types are implemented: ice chips, snow clumps, mirror
chips, crystal splinters, felt scraps, clock gears, wool tufts, rope fibers, fish bones,
wicker strips, fishing floats, globe glass, copper curls, kelp scraps, tin lids, wax, charred wick and coal crumbs. Kelp chewing leaves wet scraps; globes leave glass;
Coal impacts/stove burnout leave coal crumbs. Bathhouses can contain fuel-limited iron stoves.
Spent/broken candles leave wax and wick. Memorial courts hold recoverable candles.
Each opened lunch tin leaves one metal lid and ice chips; weather vanes shed copper and brass. Creels scatter wicker, line and a small float.
Bones scatter when fish is eaten; fibers from snapped line and exhausted spools. Tufts come
from dead pilgrims and burned Wool Wraps. Clock gears scatter from
broken alarms. Felt comes from torn/burned covers
and smashed lens cases, with light-piece friction and local wind response.
All use the shared cosmetic pool; remaining catalog materials are still open.

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


Weather-station slice: courtyard geometry with snowy edges, a central ruin floor,
a drummer with an early frost bat or later Echo Hound, and Snow Globe supplies
within floor budgets. Sparse 18-HP weather vanes are nonblocking metal props,
not fuel or guaranteed loot. Existing route protection constrains placement.
See [weather implementation](../history/ICE_WEATHER.md) for synced obscuration,
interrupt rules and the distinction between action cues and ambient audio.

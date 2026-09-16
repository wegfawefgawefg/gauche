# Industrial catalog: the underworks

Design specification, not a claim of implemented content. The runtime currently
has lava terrain, Ember enemies, native charcoal/molten terrain art, and
working mine crews and whistle orders, finite-coal Ember Stokers, Powder Monkeys,
Strikebreaker escorts, Rivet Gunners, Arc Welders, Magnet Cranes, Pressure Rats, Cable Crawlers, Walking Kilns, Audit Clerks, Furnace Moths, Slag Snails, Ash Sleepers, Counterweights, Emergency Pumps, Mold Thieves, Tar Choirs, Rail Shunters, twenty-seven regional items, metal cover/bins,
assembly-room conveyors, wet repair bays, cooling works, cable trenches, kiln courts, pay offices, lamp alcoves, slag banks, ash lofts, hoist shafts, casting floors, settling tanks, freight sidings, scrap-yard sorting lanes, the Freight Exchange unique floor, ten integrated catalog debris materials and eight ambient cue families. Track implementation in `../MASTER_TASKS.md`.
This is biome three, following Forest and Ice. The fourth biome remains open.

## Identity and shared rules

A mine became a foundry, then kept excavating underneath itself. Crooked timber
supports give way to brick furnace halls, iron service passages and black slag
banks. Work crews, stubborn machines and heat-fed creatures share the place.
The joke is that production continues during a dungeon raid: someone still
checks your punch card while a furnace rolls past eating the furniture.

Flat charcoal floors, muted rust, broad dark stone walls and occasional cream
labels leave plenty of negative space. Brass marks controls; orange marks heat;
cold blue marks coolant. Avoid a uniform orange wash or scratches on every tile.
Wall art is top-down mass with occasional exposed material, not a platform rim.
Dust belongs near active excavation; smoke belongs near actual exhausts and
retains the large player-centered visibility cutout.

Compose around excavation, heat/pressure, material transport and noisy work.
Reuse existing damage, digging, fire, wetness, electricity, hearing and item
condition. Belts add discrete forced steps, not sprite interpolation. Machines
use bounded integer cycles and fuel/charge; there is no whole-map fluid solver.
Ordinary heat is local, never a passive biome-wide health tax.

* Normal rock can be cut at its dig threshold. Structural boundary rock and
  protected objective barriers cannot. Every excavation action uses shared
  tile break rules, including enemy picks, drills, blasts and the train.
* Implemented grates have 60 HP and block bodies/melee while passing sight,
  bullets, arrows/bolts, ice needles, coal/frost pellets and beams without damage
  to the bars. Bombs, rockets and larger thrown objects still collide. Mining
  crews can cut blocked routes; Press Hammers break the mesh quickly. Short
  partitions avoid protected routes. A grate is not a bridge.
* Implemented powered belts carry grounded actors, real loose items and boiler
  vessels one cell every 20 ticks. Flying actors, grip/root effects, other anchored
  fixtures, sled-linked bodies/cargo and trains remain stationary. Occupancy is
  captured before movement: no hidden pushing chains, overlap or corner cutting.
  Hand cranks move connected manual runs once per turn; shoes jam a run for 6s.
  Cart and sled transport remain follow-up work when those carrier rules are ready.
* Cold stops small fires, cools marked workpieces and stalls hot mechanisms.
  Sudden cooling of a pressurized vessel gives a visible steam warning; it does
  not silently convert a cold flask into an immediate explosion.
* Work orders are audible authoritative events. A whistle identifies a caller
  and a direction; followers hold generation-checked leader handles. Decorative
  voice samples never decide whether a worker heard an order.

Client-only chips, casings, ash, paper, sparks, oil-colored cosmetic smears and
ambient sound cannot obstruct, conduct, attract enemies or become collectible.
Collectible bolts/coal and hazardous spills are separate real gameplay objects.
Debris is bounded, sleeps, has no piece-to-piece collisions and cannot accumulate
into a gameplay wall. Material only changes its local bounce/drag/sound.

## Twenty adversaries

Numbers are initial tuning targets. Each drop row is one percentage roll; the
unlisted remainder is nothing. Real stolen or carried cargo returns separately
and exactly once. Helpers do not multiply loot. Sound names describe new offline
assets to make, not substitutes using the current generic sounds.

| # | Enemy | Behavior, tell, interruption and sound | Habitat; one-roll drops |
| --- | --- | --- | --- |
| 1 | Ember stoker | Implemented: 48 HP, six coal shots including the loaded one. Packs for 0.5s, spits a traveling 12-damage hot pellet to eight cells, scoops for 1s between shots. Water/cold spoil the next ignition: 4 damage, no fire. Empty sack uses a 0.5s shovel tell for 14 damage. Coal can refuel the reserve. Cough, shovel scrape, furnace spit. | Existing Industrial encounters/spawners; 25% up to two unspent coal lumps, otherwise nothing. Slag scoop drop and dedicated furnace aisles await implementation. |
| 2 | Pickhand | Implemented: 80 HP; follows a separate lane at the foreman's work front, braces 0.6s, then cuts walls/props for 25 damage, dig power 1. Single weak hits provoke a complaint; a hit of at least 12 damage or accumulated 20 damage in 2s causes pursuit. Interrupted swings still need a new tell. Boot scurry, pick ring, mutter. | Implemented workfront; 20% pickaxe at 12 condition, next 20% 3–6 gold, otherwise nothing. Lunch tin drop awaits item implementation. |
| 3 | Shift foreman | Implemented: 110 HP; surveys, whistles for 0.6s, then advances behind three linked pickhands in separate lanes. Attacking him or killing two crew members rallies surviving crew against the attacker. Points, whistle, three-part “hup” cadence. | Implemented workfront; single roll 25% Foreman's Whistle, next 25% 5–9 gold, otherwise nothing. Chalk drops await its item. |
| 4 | Powder monkey | Implemented: 42 HP, two charges; finds reachable dry cover, lights a real 2s Quarry Charge, spends 0.4s placing it, then scrambles away. Killed/displaced carriers leave the same bomb at its original cell. Cold slows the fuse; water/scissors recover it. Empty monkeys flee and use a 0.4s, 6-damage swipe when cornered. Cork pop, panicked chatter and fast steps. | Implemented reserved blasting alcove with scissors; 25% one unspent quarry charge, next 15% fuse scissors, otherwise nothing. Already-armed charge is separate and persists. |
| 5 | Rail shunter | Implemented: 120 HP, slow worker; moves behind its real cart, warns with a 0.8s hand bell, then shunts. Cart rolls at eight ticks/cell, stops before its first victim after 18 damage, carries exact cargo and spills it when broken. Cut track, brake/haul cart or interrupt worker with damage/cold/control. Adjacent 12-damage swings warn for 0.6s. Eleven freight/worker cues. | Optional loaded freight siding. One roll: 20% Brake Shoe, next 20% Rail Switch Key, next 20% 4–8 gold. Actual cart cargo is separate. |
| 6 | Rivet gunner | Implemented: 64 HP; braces for 0.5s and fires three physical 9-damage rivets along a fixed lane, 0.15s apart. 1.5s reload; damage/displacement/control effects spoil the burst. Seeks reachable firing lanes and can use grates. Pneumatic pop, brace, reload and death sounds. | Blasting-alcove side posts where safe (ordinary room spawn otherwise), plus incidental Industrial encounters. One roll: 25% rivet gun, next 20% ammo. Assembly rooms now supply working belts, cranks and brakes. |
| 7 | Magnet crane | Implemented: 90-HP anchored base; head travels 0.5s, marks a fixed cell for 0.75s, then pulls its original target one cell and retracts for 1s. Held/dropped magnets outrank steel-carrying actors/workers, then loose steel. Actor tug deals 6; blocked pulls pinch without instant crush; grip/root/sled resist. Switching away from steel or leaving the mark makes it miss. Cold and hits of at least 12 interrupt. Base is the damageable part; no separate chain hitbox. Slew motor, rising coil, tug and collapse sounds. | Optional scrap yard with a hot sorting lane, worn pickaxe, bank magnet and native bins. One roll: 25% Horseshoe Magnet, next 20% Copper Wire, otherwise nothing. |
| 8 | Slag snail | Implemented: 96 HP, slow crawl; shell takes one-third blockable damage. Tucks for 0.8s before a fixed two-cell lunge, 18 damage on first contact, 1.2s exposed recovery. Cold/water exposes it for 2s; heavy health hits/control/displacement spoil commitments. Trails stay hot 1.5s, then become safe crust that breaks underfoot; cold quenches early, water washes away. Three poses, seven sounds. | Optional slag bank with lava trench, dry bypasses, coolant and ore bin; incidental encounters cost two. One roll: 30% shared Cooked Meat, otherwise nothing; distinct cinder sausage/refractory paste remain pending. |
| 9 | Pressure rat | Implemented: 22 HP; carried boiler bladder inflates with a 0.6s whistle, then rushes one cell per 0.1s in a fixed lane. First obstacle/body or 24-cell pressure limit causes a single fatal 8-damage steam cross. Cold/water and control effects deflate it; flees while cooled. Early kills vent safely. Three poses, rising whistle, rush, burst and death vent. | Incidental Industrial encounters, cost 1; one roll 25% raw meat. Dedicated outlet/pipe-crawl ecology and 10% rubber-hose drop remain pending. |
| 10 | Tar choir | Implemented: three stationary 32-HP singers swell and hum before coughing at 0.8/1.0/1.2s into a committed three-cell strip. Gobs travel at six ticks/cell, deal 6 and leave 10s tar. Cover breaks the rhythm; damage, cold/control and displacement interrupt. Survivors and separated singers work alone. Cold clots tar, water washes it, fire hurts allies too. Three bubbling voices, cough, disruption and death cues. | Optional settling tanks: paired tar basins, grates, cover, shallow water, Cold Flasks and Emergency Foam. Final member only: 30% Tar Flask; next 15% solvent rag remains pending. |
| 11 | Walking kiln | Implemented: 160 HP, four slow movement beats before a 1s door warning, then three fixed flame rows. Two initial fuel, capacity three. Eats real adjacent coal or crate/log/twigs after a 1s feed. Open mouth doubles physical damage; cold/water cancels heat and leaves it exposed. | Optional kiln court and incidental encounters. 15% up to two remaining coal; planned ceramic plate (25%) and coal biscuit (20%) remain unimplemented slots. |
| 12 | Audit clerk | Implemented: 38 HP, normally neutral; seeks loose coins within nine cells, stamps for 0.5s, carries up to 20 actual gold to its linked pay cage and deposits for 0.5s. Real damage or nearby cage tampering starts a 0.6s raised-bell warning, then calls existing audible workers and flees. Cold/control/displacement interrupt commitments. Three poses; stamp, paper, gasp, bell and death cues. | Optional pay office. Actual carried gold returns on death, plus one 25% roll for 3–7 personal gold. Punch-card and inspector-stamp ranges remain empty until implemented. |
| 13 | Arc welder | Implemented: 70 HP; lowers visor for 0.7s, then sweeps three fixed lanes, two cells deep, 0.15s apart. Each lane deals at most 7 electrical damage per victim; shared water/wire paths can hit allies and the welder. Ready grounding sinks one entire lane pulse. Hits of at least 10 damage, control effects and displacement interrupt; 1.2s recovery. Cuts grates/metal bins for 14. Mask hinge/clunks, shared arc/cutting cues, death grunt. | Optional repair bay with a wet work area, dry margins and a grounding spike; ordinary room fallback if the authored footprint is occupied. One roll: 25% Arc Torch, next 15% Copper Wire, otherwise nothing. Welding visor remains pending. |
| 14 | Ash sleeper | Implemented: 44 HP mound; loud audible work or damage starts a 0.5s shaking rise. Swirling pursuit commits an 0.5s fixed-cell swipe for 8, then 0.5s recovery. Damage/displacement/control interrupt strikes. Six seconds without new agitation settles it; water/cold settles and suppresses waking for 2s without healing. Five dry-grit sounds, two poses. | Optional ash loft with manually cranked belt, Nail Board cargo, coolant or bellows and bins; incidental encounters cost 2. Ash-sack/filter-mask drop ranges remain empty until those items exist. Hand Bellows directly disturbs and pushes ash; fixed fan outlets remain future content. |
| 15 | Mold thief | Implemented: 58 HP; 0.3s tongs tell before taking a real loose steel item/stack. Runs to an empty mold; stationary 3s sealing with visible progress and crank cues. Hurt spills cargo and triggers 3s flight; cold/control interrupt without deleting cargo. Cannot steal equipped, flying, anchored or sled-loaded gear. Broken carrier/mold returns exact ammo, wear and modifiers even with a full entity pool. | Optional casting floor; 20% Mold Key. Reserved 20% Foundry Tongs remains empty until implemented. |
| 16 | Cable crawler | Implemented: 50 HP; wall-biased cardinal crawl, eight lifetime wire cells. Braces 0.8s, then pulses at a committed adjacent contact or through its current six-step circuit; 14 contact damage falls with wire/water distance. Own electrode insulated, other electricity still hurts. Rebuilds circuit at firing: cuts and ready grounding spikes work immediately. Cold, displacement, root/control and hits of at least 8 cancel; 1.5s recovery. Two poses, relay charge, snap, claw steps and broken-spool sounds. | Optional cable trench with five-cell wiring, wet branch, dry margins, diggable rock shelf, scrap bin and a Grounding Spike or Insulated Boots; incidental Industrial encounters, cost 2. One roll: 25% Copper Wire, next 15% Insulated Boots. |
| 17 | Counterweight | Implemented: 140-HP anchored chain winch; travels overhead for 0.5s, warns at a fixed cell for 0.8s, then drops over 8 ticks. Impact deals 48 to any creature there and 120 to props; 2s rewind. Heavy hits of 12+, cold/control or displacement cancel the drop. Moving shadow, hanging mass, five mechanical sounds. Destroying the winch leaves an 80-HP cuttable wreck at its original footprint, with no surprise impact. | Optional hoist shaft with pressure rat, bins, eight coins, coolant and Chain Hook. Decoys can bait it. One roll: 25% Chain Hook, otherwise nothing; Counterweight Bag remains pending. |
| 18 | Strikebreaker | Implemented: 150 HP, finite 90-HP frontal shield, neutral crew escort. Defends linked workers against real attackers. An 0.3s shove tell leads to a fixed-cell 0.7s hammer tell for 32 damage; rear/cold/attack recovery expose him. Breaks obstructing ordinary rock and props with the same warned hammer. Shield bang, heavy boot steps and press slam. | Workfronts on Industrial stages 2–4; 20% press hammer, roll 35–54 gives 5–9 gold, otherwise nothing. Steel toe cap remains pending; reserved 15% currently empty. |
| 19 | Furnace moth | Implemented: 18 HP, flying; seeks nearby bright lights, siphons up to 300 actual heat after a 0.5s sip, then spends its reserve on a committed cardinal dive after a 0.75s folded-wing warning. Six ticks/cell, at most five cells, 4–14 contact damage and up to 3s ignition. Cold empties heat; damage/control/displacement interrupt. Cold lamps and dropped lights lure without supplying fuel. Two sprites, six sounds. | Optional lamp alcove with finite stove, cold electric lamp and foam; incidental Industry encounters cost 1. Starts with 180 heat except the alcove feeder, which starts cold. 10% Heat Capsule; reserved 25% glow-slag range remains empty until implemented. |
| 20 | Emergency pump | Implemented: 100-HP mobile machine, three-pour finite tank. One-second refill from a real reachable spill; 0.6s fixed-direction warning, three-cell pressure jet, 1.2s recovery. Full portions deal 6 and push once; tiny portions scale down. Water quenches/cools lava, oil supplies fuel, coolant chills/reduces boiler pressure. Heavy hits of 10+, displacement, cold/root/control cancel without spending. Cover blocks, grates pass; no wall-crush shortcut. Four mechanical/wet cues. | Cooling works pair it with a boiler, starting with water or oil; incidental encounters start with water. One roll: 25% Pocket Pump retaining remaining tank contents, next 15% Nozzle Elbow, next 15% Coolant Can. |

Pickhands use bounded route scoring: ordinary step cost 1, digging cost estimated
from remaining wall HP and actual swing interval. Replan on obstruction, target
movement or a short cooldown, not every tick. Prefer walking around a small wall;
cut a long detour when faster. Outer rock stays impassable. Idle work may strike
outer rock without progress, but combat pursuit must eventually try another route.
Formation cells are requests, not reservations that block the level. Followers
yield and retry alternate open cells; missing leaders cancel work orders. Never
let three waiting followers freeze their leader as the old chick flock did.

Shared entity fields hold phase, counters, two timers, two cells and checked
handles. Crane heads, carts and kiln bodies need separate entities only when
independently targetable/moving. Machine fixtures and belts belong in compact
world storage. All linked enemy death/drop paths must tolerate stale handles.

## Fifty regional finds

Initial design targets, not final balance. C = common, U = uncommon, R = rare;
the number is a starting shop price in gold. Uses/durability are not stack counts.
Unless specified, placement is adjacent, uses aim cardinally, attacks affect
friends under the lobby's friendly-fire rules, and failed use spends nothing.
All throws/shots have visible travel except the explicitly continuous arc tool.
Existing shared/Ice items above are carryover drops, not counted toward these 50.

| # | Item | Action, cost, failure or tradeoff; sound | Rarity / price |
| --- | --- | --- | --- |
| 1 | Rivet gun | Implemented: three-round traveling burst, 9 damage/rivet to 8, 0.15s between shots; 12 loaded/24 spare, 1.5s reload. Fixed burst aim, 0.6s recovery after last shot. Release finishes burst; switching/reload cancels unfired rounds. Stops at bodies, passes grates; Piercing and other supported weapon modifiers work. | U / 30; gunner drops, Industrial stock/rewards |
| 2 | Press hammer | Implemented: 0.6s overhead windup, 32 damage and one-cell shove; 30 repairable condition. Double damage to blocking props; dig power 1 preserves reinforced routes. Taking damage cancels the windup without wear. Iron thud. | U / 25 |
| 3 | Arc torch | Implemented: hold 0.3s, then adjacent 7-damage arcs every 0.25s; 32 pulses. Water/wire carry weaker shocks to 3, including back to the user; grounding sinks one pulse. Cuts grates/metal bins at double damage. Release/switch requires a fresh prime. Electrical start, crackle, metal hiss and spent sputter. | U / 38; stage-two Industrial stock, one-stage early salvage/secret preview |
| 4 | Slag scoop | Collect one safe crust/ash patch, or scoop a hot patch while taking a little heat unless protected. Holds three loads; secondary flings the oldest material to 3. Metal scrape, granular pour. | C / 10 |
| 5 | Chain hook | Implemented: physical five-cell cast, then held hauling every ten ticks. Moves whole loose crates/pots/bins, actual ground gear, loaded/ridden sleds, and rail carts along intact track. Braked carts refuse. Fixed walls/structures pull the user; grounded hazards still apply. Release pauses, secondary cuts, four-second line limit. Blocked or displaced targets cut safely without crushing/rerolling. 24 casts; Long/Durable supported. Five iron-chain sounds. | U / 22; native stage-one stock, scrap-yard supply and 20% scrap-bin drop |
| 6 | Quarry charge | Implemented: place facing ordinary stone. Three-second fuse cuts up to three forward cells; 24 damage along the cut, at the charge and one cell behind. Stack 3. Dig-power-2 reinforcement, other materials and outer boundaries stop it. Water quenches into a recoverable charge; cold slows the fuse. No modifiers. Fuse scratch, deep crack. | U / 19 |
| 7 | Rubber mallet | Implemented: 0.15s swing, 3 damage and one-cell shove; 70 repairable condition. Tap a boiler to shed 40 pressure without cutting it or spending fuel; below 25 cancels its tell. Sealed outlets refuse. Shoving into a wall can crush. Hollow rubber knock. | C / 9 |
| 8 | Bolt pouch | Implemented: five physical 6-damage bolts spread over five cells, four ticks per cell. All five can hit at point-blank range; outer lanes cover crowds. Stack six, ordinary bundle three, 0.7s cooldown. Grates pass bolts; solid cover/corners stop them. Parries return bolts on the same finite clock. Spent bolts become local steel scraps, never reusable ammo. | C / 9; native stage-one supplies, assembly cargo and 20% scrap-bin bundles |
| 9 | Steam lance | Implemented: fill at clean water, heat nearby for 1.5s; 30s heat retention, wetness cools it. 0.45s committed-facing brace, two-cell 18-damage scald; cold spray only wets/douses. One fill per thrust, reusable, 0.75s cooldown. Grates pass the narrow jet, solid cover stops it; bodies do not. Real hits/control/switching interrupt without spending. Strong/Long/Agile/Heavy supported. Eight boiler/valve/water cues, hot/cold sprites, heat meter. | U / 24; stage-one native weapons/stock, half of lamp-alcove supplies start water-filled |
| 10 | Nail board | Implemented: 10-damage swing, 0.1s windup/0.3s cooldown, 35 condition. Secondary lays ahead with 0.3s arming; next grounded entrant takes 16 and leaves the worn board recoverable. Swings cost 1 condition, trap hits 4. Flyers/tossed actors/sled riders pass safely; friends and owner count. Fire and direct hits damage the deployed board. Shared pickup/swap, melee modifiers, four sounds and wood/nail debris. | C / 11; native stage-one weapon/stock tables and assembly-belt cargo |
| 11 | Foreman's whistle | Implemented: gives audible idle pickhands fixed lane destinations four tiles in the aimed direction for up to 4s. Angry workers refuse; audible real leader calls take them back. Six successful calls, 1.5s cooldown; Long reaches eight tiles, Durable has twelve calls. Successful calls wake sleepers and attract hearing enemies. Short-short-long call, worker answer, spent note. | U / 18 |
| 12 | Survey chalk | Draw three bright floor marks per use, six uses. Allies see them; crew uses marked work fronts when idle. Washes away; does not reveal unseen rooms. Chalk squeak. | C / 5 |
| 13 | Pocket drill | Implemented: hold still/aimed for 0.4s, then adjacent 8-damage contacts every 0.2s. Thirty finite battery beats, including open air; Durable sixty, Fragile fifteen. Moving/turning restarts the prime; real health damage, sleep/stun/toss cancel until release. Dig power 1; cuts ordinary stone, wood, props and wire, preserves reinforced boundaries. Motor noise wakes sleepers and attracts hearing enemies. No ammo refill. | U / 27; native stage-two stock, weapon/reward/cache/workshop tables |
| 14 | Fuse scissors | Implemented: snip the shortest landed exposed fuse at your feet or ahead; recover that exact bomb into the pack. A full pack, airborne/expired fuse or sealed Prism/Thaw Charge rejects it without spending. Twelve snips; Durable 24, Fragile 6. Also works on Bomb, Firecracker and Pitch Bomb. Metallic click. | C / 8 |
| 15 | Brake shoe | Implemented: stop an adjacent cart or jam a straight belt run for 6s, four shoes; a hit of at least 16 breaks the brake. Already-braked targets refuse without spending. Carts remain stopped after release until pushed; full-size trains ignore shoes. Rubber drag. | C / 10; assembly/freight supply, shunter drops and Industrial stock/rewards |
| 16 | Rail switch key | Implemented: twelve clockwise turns through existing adjacent rail exits; Durable doubles uses. Occupied points refuse without spending. Fixed direction controls the next cart wheel beat; cut/blocked exits stop it. Breakable 24-HP points, lever clack and spent-key sounds. | C / 9; junction supply and 20% shunter drop only, excluded from general offers/caches |
| 17 | Horseshoe magnet | Implemented: hold to pull the nearest loose steel item one cell per 0.2s along a clear five-cell lane. Moving/aiming stay live. 75 successful pulls; empty/blocked attempts free. Preserves exact item state, no inventory theft or copper/wood/food attraction. Long reaches nine; Durable 150 pulls. Quiet metallic strain and spent crumble. Held/dropped magnets now attract Magnet Crane heads. | U / 18; native stock, 20% scrap-bin drop; one-quarter of compatible hot salvage pockets offer a bank magnet |
| 18 | Foundry tongs | Carry one hot loose object at arm's length; release places it ahead. Occupies held use and prevents attacks, preserving the exact item/heat. Forty handling wear. Tongs clink. | C / 12 |
| 19 | Mold key | Implemented: open adjacent sealed casting molds without breaking them; normal pickup retrieves the real cargo. Eight uses, Durable sixteen. Empty/open molds refuse without spending. The reusable 60-HP mold can instead be smashed for the same cargo. Steel key, heavy lock-turn sound. | C / 8; native stage-one supplies, casting-floor key and 20% thief drop |
| 20 | Pocket pump | Implemented: reusable tank starts with three water pours. Use collects a real adjacent spill; secondary pours one portion ahead, 0.5s cooldown. Holds one liquid; partial spills preserve their exact quantity and storage stops evaporation. Water, oil, sap, honey, rot, brine and coolant supported; lakes, ice, burning fuel and slag refuse. Water douses/cools lava; coolant retains chill and boiler-pressure effects. HUD tank meter, configurable debug contents, two pump sounds. | U / 23; native stage-one stock/cache/workshop tables and one-third of cooling-works supplies |
| 21 | Nozzle elbow | Implemented: attach to a boiler or Emergency Pump to bend future jets 90 degrees; secondary selects clockwise/counterclockwise before fitting. Current warned jet keeps its fixed direction. 18-HP fitting, Durable 36; real health hits wear the fitting too, breakage restores future straight aim. Recover during idle/recovery; boilers require below 25 pressure. Exact condition/handedness survives pickup, drops and snapshots. Occupied outlets refuse; steel fitting responds to magnets. Four threaded-metal/latch sounds. | U / 14; native stage-one supplies, 15% pump drop and one-third of cooling-works supplies |
| 22 | Rubber hose | Connect two nearby outlets across up to four clear floor cells; redirects a finite supplied flow, does not generate pressure. Cut/fire breaks it. One recoverable coil. Rubber unroll. | U / 16 |
| 23 | Hand bellows | Implemented: eight puffs through a three-tile cone, 0.75s cooldown. Push actors and loose gear one cell without wall-crush damage; ground hazards still apply. Drift smoke/spores/whiteout, fan existing flame one cell onto real fuel, disturb ash and scatter local debris. Walls/doors stop the gust; grates pass air. Big widens, Long extends, Durable holds sixteen. Two leather/air sounds. | C / 10; native stage-one stock, containers and treasure; half of ash lofts supply it instead of coolant |
| 24 | Inspection mirror | Hold to look around one adjacent corner with a narrow reflected sight wedge. Cannot shoot through it; hard hits break its 12 condition. Tiny handle rattle. | U / 17 |
| 25 | Belt crank | Implemented: hold to turn an adjacent unpowered straight run once per 0.2s beat. Forty turns (Durable eighty), one real step per turn. Brakes refuse; broken sections disconnect the drive. Powered runs cannot be accelerated with it. Ratchet clicks. | C / 11; assembly supply and Industrial stock/rewards |
| 26 | Tar flask | Implemented: eight-tick/cell throw to five, five-cell cross of 10s tar. Grounded walkers gain twelve recovery ticks; flyers/tossed actors ignore it. Fire consumes at most four seconds of remaining fuel; cold/native ice clots it into passable nonsticky crust. Heat reignites, water washes away. Unburning liquid tar transfers through pumps with exact quantity; crust refuses. Big/Long supported; four original glass/viscous/crackle cues. | C / 8; native stage-one tables in bundles of three, one-third of small-item assembly cargo |
| 27 | Coolant can | Implemented: four three-cell pours, Durable eight. Shared quench plus 3s chill; removes 80 boiler pressure without consuming fuel/fittings. Cool lava to stone. Green 10s residue slips, conducts shocks and prevents refreezing; no clean kettle filling or lava immunity. Pour, empty-can and slip sounds. | C / 12; native stage-one stock, 15% ore-bin drop, cooling-works supply |
| 28 | Refractory paste | Coat held tool for six contacts with hot objects, avoiding heat wear; does not give the player full fire immunity. Three applications. Gritty smear. | U / 14 |
| 29 | Solvent rag | Clean adjacent tar/oil or a fouled mechanism; five wipes. Used rag becomes flammable until rinsed. Cannot erase an entire room. Cloth swish. | C / 6 |
| 30 | Ash sack | Dump a three-cell fan that smothers exposed ground flame and quiets steps for 8s; two dumps. Wind scatters it; hot machines remain hot. Dusty thump. | C / 7 |
| 31 | Ceramic plate | Hold a directional plate against one hot projectile; breaks on the block. Physical bullets still hurt. Stack 3 unused plates. Ceramic ping/shatter. | C / 10 |
| 32 | Welding visor | Hold to shield facing light/arc glare and reduce short frontal arc damage; narrows vision while down. 20s active wear. No all-direction shock immunity. Visor snap. | U / 19 |
| 33 | Insulated boots | Implemented: six 4s activations, Durable twelve; later water/wire circuit nodes cannot hurt the wearer, but direct electrode/arc contact still does. Steps take twice as long; can stow after fitting. No circuit break, team shield, cold/fire resistance or traction. Active timer, movement preview and six rubber/electrical cues. | U / 20; native stage-one stock, half of cable-trench supplies and 15% crawler drop |
| 34 | Steel toe cap | Next three voluntary kicks/shoves gain force; also safely kicks one armed ground object away. Twelve charges, one per successful kick. Cannot shove anchored hoists. Clang. | U / 15 |
| 35 | Filter mask | Hold to resist smoke/ash breathing penalties, with muffled local audio and narrower sight. 30s filter life; no protection from actual flame. Cloth breath. | U / 16 |
| 36 | Lunch tin | Eat one of two portions for 10 HP over 3s; damage interrupts. Empty tin remains and can be thrown as a loud distraction. Lid pop, chew, hollow landing. | C / 9 |
| 37 | Cinder sausage | Eat for 8 HP and 8s warmth; briefly attracts furnace moths and heat-seeking creatures. Dropped sausage is real food bait. Stack 4. Crunchy munch. | C / 6 |
| 38 | Coal biscuit | Awful snack: lose 2 HP, resist nausea for 12s; alternatively feed it to a kiln as fuel. Stack 5. No free healing conversion. Dry crunch. | C / 3 |
| 39 | Salt tablet | Clears one heat-fatigue penalty but gives 2s thirst/slow recovery; use only when affected. Stack 6. Never a requirement to survive normal floor temperature. Capsule click. | C / 5 |
| 40 | Emergency foam | Implemented: throw to four cells, then expand for 1s after landing. Quenches a small cross through shared fire/fuse rules; empty solid floor gets 12-HP cover for 10s. Bullets tear it, adjacent heat costs 2 HP each 0.5s. No terrain conversion, buried loot, overwrite or water bridge. Stack 2, sealed reaction, no modifiers. Six original can/hiss/tear/collapse sounds. | U / 18; native stage-one stock, half of kiln-court supply rolls |
| 41 | Punch card | Insert into a marked service reader for one optional access/maintenance cycle. Stack 3, consumed; main route never requires a randomly found card. Paper punch. | C / 7 |
| 42 | Inspector stamp | Mark one idle worker/machine for inspection: it pauses one work cycle unless already fighting. Five stamps; foreman notices repeated misuse. Official thump. | U / 13 |
| 43 | Counterweight bag | Place a heavy 30-HP bag on a pressure plate, or throw to 3 for a 10-damage shove. Recoverable; belts carry it, flying cannot trigger plates. Sand thud. | U / 17 |
| 44 | Folding barricade | Implemented: one kit unfolds three independent low steel sections across the adjacent lane. All cells must be clear. 20 HP each, Durable 40. Walkers stop, narrow shots pass, lobbed objects pass overhead. Torch/welder cutting and worker smashing reopen gaps. Metal survives fire; folded kit is magnetic. Hinges/latches clatter. | U / 23; native stock from regional stage 2 |
| 45 | Warning cone | Place a bright recoverable marker; idle crews route around it, angry crews kick it aside. No physical immunity or invisible wall. Stack 3 unused cones. Rubber bonk. | C / 4 |
| 46 | Glow slag | Implemented: recoverable throw to six, 4 damage, four ticks per cell. Finite 20s light/heat clock persists while stowed, carried or flying; exposed heat thaws adjacent ice and can ignite fuel. Secondary extracts real heat ahead, or fully reheats at lava. Water/cold quenches, moths and Heat Siphons steal stored heat. Individual non-stackable lumps preserve exact heat; cold stacking is deferred. Hot/cold sprites, heat meter, four glass/heat sounds. | C / 6; native stage-one supplies and one-third of lamp-alcove supplies |
| 47 | Tension spring | Implemented: set underfoot aimed; arms in 0.3s. Next susceptible walker launches four cells in 0.4s, skipping ground contacts while airborne. Walls cause 12 damage/0.5s stun; unsafe landings stay unsafe. Stack 3, single use; 6-HP mechanism softens in heat. Flyers, grip/root and sled riders refuse. Uses existing toss arc; carts remain pending. | U / 15; native stage-one stock and one-quarter of hot salvage banks |
| 48 | Scrap effigy | Assemble an 18-HP metal decoy ahead. Cranes prioritize it and ranged work guards may commit one attack to its sound. No universal taunt. Two kits. Loose-metal jangle. | U / 20 |
| 49 | Furnace seed | Plant a small 24-HP hungry burner; feed actual wood/coal to keep a warm cross active. Fire can ignite nearby spills; cold stops it. One recoverable cold seed. Tiny furnace cough. | R / 31 |
| 50 | Overtime clock | Place a 12-HP clock, wait 1s; for 5s nearby working machines/crew shorten idle beats, including dangerous attacks. Affects allies' crank/drill work too; no global player haste. Two windings. Fast mechanical ticking. | R / 34 |

Big/Long/Durable/Strong variants are supported only where the operation has a
meaningful radius, range, wear or damage parameter. They must not expand a
machine's protected digging permissions or multiply returned cargo. Trades and
death drops preserve partial batteries, packed liquids, hot-state timers and wear.

## Rooms, routes and progression

One quiet work vignette before the first combined encounter. Each floor adds a
new combination, not a blanket HP increase. Floor 3-1 introduces excavation and
grates; 3-2 belts, carts and cooling; 3-3 casting/pressure branches; 3-4 the central
foundry exit. Special layouts replace a floor only once their full route works.

| Room | Shape and route job | Placement and interaction |
| --- | --- | --- |
| Lamp room | Implemented initial optional alcove: offset dry lighting station, protected central route | Two Furnace Moths (one cold), a stove with 30s real fuel, a cold electric Beam Lamp, a small shallow-water source, and either two Emergency Foam cans or a cold-filled Steam Lance. Whole-footprint checks; shares maintenance selection. Stove heats the lance; water refills it, and its spray cools the moths. Larger safe-entry/water-tap variants remain pending. |
| Work front | Bent gallery, breakable interior spur, protected outer wall | Foreman and up to three pickhands, chalk marks, one side bypass. Their digging opens optional space. |
| Blasting alcove | Two offset pockets around a thick rock seam | Powder monkey and exposed fuse, clear retreat bay, optional seam-cache reward. |
| Rail junction | Implemented: T siding with working keyed points, mirrored placement and straight fallback | Cuttable track with walkable margins, shunter and real loaded cart. Branch rooms supply a Rail Switch Key; straight rooms a Brake Shoe. Cargo is three Bolt Pouch handfuls or a 12-durability Press Hammer. No bodies on the reserved crossing or mandatory gate footprint. See [freight](../features/RAIL_SHUNTER.md) and [points](../features/RAIL_POINTS.md). |
| Assembly line | Implemented: two opposing belt lanes with stationary protected crossings, including a disconnected manual run | Reserved rivet gunner and short grates where safe, crank, brake shoes, real Nail Board cargo and either three-handful Bolt Pouch (two-thirds) or three Tar Flasks (one-third); dry ordinary route. Belts can be broken, hand-driven or braked. |
| Scrap yard | Implemented: open dry yard with a short optional hot sorting lane and crane pedestal | Worn six-condition pickaxe, magnet beyond the initial five-cell crane reach, scrap/ore bins. Protected route and occupancy checks precede all placement. Up to one eligible yard per floor, half selection chance. Scrap effigy remains pending. |
| Slag bank | Implemented initial offset lava trench and two dry banks, open routes around its ends | Slag Snail, short hot trail, opposite-bank Coolant Can and Ore Bin; protected central cross remains intact. Whole-footprint checks and mirrored fallback. Larger curved-basin variants remain candidates. |
| Pipe crawl | Narrow alternate service lane parallel to main hall | Pressure rats, readable leaking outlet and cutoff valve. Can be bypassed or deliberately vented. |
| Settling tanks | Implemented paired shallow tar basins with a dry central crossing | Three linked singers, cover bins, shoot-through rim grates, Cold Flasks, Emergency Foam and shallow water. Solvent shelf awaits the rag item. See [implementation](../features/TAR_CHOIR.md). |
| Kiln court | Implemented initial variant: open court with an offset oil or tar lane (equal chance) and clear dry margins | Walking kiln, crate/log stock, and an equal choice of Coolant Can or two Emergency Foam cans on the opposite dry side. Shared maintenance-room selection, protected footprint; U-shaped alcoves/ceramic chest remain candidates. |
| Pay office | Implemented initial variant: optional dry clearing with an offset grated pay cage | Clerk, six loose coins, finite cage balance and a nearby Pickhand. Whole footprint checked before placement, mirrored if needed; protected central route stays open. Tampering warns before calling existing audible workers. |
| Repair bay | Implemented: broad dry clearing with an offset 3×3 wet work area and two grate benches opposite | One welder on the dry bank, one ready grounding spike beside the water. Entire footprint is checked before placement and avoids protected routes. Maintenance-room selection has a two-thirds chance, then picks one of ten maintenance encounters equally, including ash loft, hoist shaft and casting floor. Drain/toolbox remain pending. |
| Ash loft | Implemented: two sleepers beside a three-cell manual belt and dry open margins; protected central route retained | Belt Crank, real Nail Board cargo, Coolant Can or Hand Bellows, scrap/ore bins. Cranking wakes nearby sleepers through the normal action sound; quiet approach and wet suppression remain options. Whole-footprint/mirrored fallback. Bagged ash and fan-outlet variants remain pending. |
| Casting floor | Implemented initial optional variant: two offset mold workstations around a protected central route | One Mold Thief, two empty 60-HP molds, a ten-use Pocket Drill to steal/recover, eight-use key on the dry side, four short grate partitions. Whole footprint validated before placement; two threat/two equipment. Full connected-yard and tongs-rack variants remain pending. |
| Hoist shaft | Implemented initial variant: offset hoist/bins, open central route and marked overhead strike | Counterweight plus pressure rat, eight loose coins within reach, Chain Hook and coolant on dry margins. Full footprint and five free entity slots required; mirrored fallback. Costs three threat/two equipment. Ring/pressure-plate-cache variant remains pending. |
| Cooling works | Implemented: finite-fuel boiler and water- or oil-loaded Emergency Pump across a three-cell lava trench, dry routes around both ends | Coolant Can, water-filled Pocket Pump or Nozzle Elbow (equal supply chances) and ore bin on the dry side; a real coolant spill can refill either pump. Lure a water jet into lava, or arrange oil near the boiler vent; coolant can stall the mobile pump or reduce boiler pressure. Protected center and four-cell vent lane stay separate; three free entity slots, three threat and one equipment. Shares maintenance-room selection with repair bays, cable trenches, kiln courts, pay offices, lamp alcoves, slag banks, ash lofts, hoist shafts and casting floors. Reservoir variants remain pending; nozzle supplies now redirect future boiler/pump jets. |
| Service reader | Small optional locked branch | Punch-card reader beside a breakable alternate route. Consumable access is never the sole mandatory exit path. |
| Shift shelter | Safe-ish reunion room, camp stove and benches | Lunch box, repair supplies; occasional shop approach and human-scale quiet. |

Required objectives still obey the route graph. Compound key+lever or two-key
gates need independently reachable prerequisites and a real cut across all exit
paths, including grates, nearby digging and belt displacement. Workfronts cannot
excavate quest seals. Routes may permit deliberate tool-earned shortcuts; they
must not accidentally leave the key irrelevant as the earlier misplaced door did.

Implemented unique: **Freight Exchange**. Industry 3-2 alone has a seeded 20%
chance to replace its generated map with connected loading halls, two loaded
cart routes and switch points, a crane salvaging worn gear, a sorting belt near
an Ash Sleeper, a guarded control booth and a Counterweight over a coin cache.
The entrance supplies a Rail Switch Key, Chain Hook and Rubber Mallet; cart
cargo is a worn Press Hammer and a three-use Bolt Pouch stack. Breakable lamps
mark work areas, dry crossings remain open, and ordinary interior walls can be
dug. A required switch opens the sealed exit; all living online players gather
there through the usual rule. Rewards, shop and Industry 3-3 follow normally.
Debug Levels can force, start at or repeat it. See
[implementation/checks](../features/FREIGHT_EXCHANGE.md). It is a whole-floor
layout, not a multi-room overlay or the reactor event below.

Unique candidate: **The Last Shift**. A mostly authored central foundry has a
clearly labeled emergency shutdown. Activating it cuts power to the exit but
starts a 60s escape through opening service gates. Hot cracks spread from fixed
vents through a bounded frontier, faster through conductive walls than floors.
Cap work per simulation tick and store queue order deterministically. Telegraph
new hot cells before damage. Guarantee a timed escape path, include split/rejoined
players, and pause the timer only under the chosen team-pause policy. This is
not ready to enable merely because the banner and countdown exist.

## Containers and deterministic loot

One roll per container, remainder empty unless stated. Quantities below are
actual items; no decorative debris becomes loot after the roll. Each row uses
only thematic stock. Healing/ammo budgets remain separate from bonus caches.

| Container | Break/open rule | Proposed single-roll table |
| --- | --- | --- |
| Tool chest | 28-HP metal; keyless opening is slower and quiet | 20% rubber mallet, 15% survey chalk, 15% fuse scissors, 15% worn pickaxe, 15% pocket drill. |
| Lunch box | 8-HP tin, opens or breaks | 35% lunch tin, 25% cinder sausage, 15% two salt tablets, 15% two coal biscuits. |
| Coolant locker | 35-HP cabinet; bullets can puncture its cosmetic casing | 30% coolant can, 20% solvent rag, 15% refractory paste, 15% emergency foam. No unrolled free liquid on break. |
| Fuse crate | 12-HP wood; fire ignites only after a fuse tell | 25% quarry charge, 20% bolt pouch, 20% ammo, 10% tension spring. Fire destruction trades safe loot for a warned burst. |
| Scrap bin | Implemented 18-HP sheet metal, noisy destruction; local tin/copper scraps | Implemented roll 0–19: horseshoe magnet; 20–39: chain hook; 40–59: copper wire; 60–79: three Bolt Pouch handfuls. Other ranges stay empty. |
| Ore bin | Implemented 30-HP mining bin; local ore/basalt fragments | 25% two coal lumps, next 15% 2–4 gold, next 15% Coolant Can, otherwise empty. |
| Pay cage | Implemented optional 60-HP shoot-through metal cover; break or cut to open, no key interaction | Initial generation roll: 60% 8–14 gold, otherwise empty pending punch card/inspector stamp. Clerk deposits add real gold up to 60,000; overflow remains carried. Breaking releases the stored balance once, without rerolling loot. Clerk-carried gold returns separately on death. |
| Ceramic chest | 20-HP brittle shell, ordinary opening | 30% two ceramic plates, 20% refractory paste, 15% furnace seed, 15% coal biscuit. |
| Machine spares | 25-HP crate | 20% nozzle elbow, 20% rubber hose, 15% brake shoe, 15% belt crank, 10% overtime clock. |
| Casting mold | 40-HP hinged iron | Real stolen cargo if present; otherwise 25% steel toe cap, 20% rivet gun, 20% press hammer. Never both cargo and bonus reroll. |

## Twenty debris materials

Ten rows are integrated; other rows remain candidates. Current solids are local,
with varied rotation and small stack-dependent sizes. Use short-lived
sparks in addition to settled solids; no fragment-to-fragment simulation.

| # | Material | Source; movement character |
| --- | --- | --- |
| 1 | Basalt chip | Implemented: Industrial wall impacts and ore bins; short heavy skid. |
| 2 | Ore flake | Implemented: ore bins; small metallic skip, dull highlight. Ore seams remain pending. |
| 3 | Brick corner | Furnace wall; angular tumble, fast stop. |
| 4 | Mortar dust | Brick damage; fine short puff, settles/fades. |
| 5 | Timber splinter | Supports/crates; elongated skitter, burns only visually. |
| 6 | Bent nail | Implemented: crate impacts/destruction leave one or two nails; short high-friction skid. No vertical bounce yet. |
| 7 | Rivet casing | Implemented: one local casing per discharge, small rolling brass piece; no ammo, loot or gameplay collision. |
| 8 | Steel washer | Implemented: grate destruction; longer roll before settling. |
| 9 | Chain link | Implemented: destroyed Magnet Cranes leave three links alongside washers; short heavy skid. |
| 10 | Copper strand | Implemented shared copper curls: cut wire, scrap bins and boiler breakage; ordinary solid-fragment drag. |
| 11 | Rubber scrap | Implemented for broken conveyor strips; high friction and short skid. Hose/boot sources remain pending. |
| 12 | Ceramic shard | Plates/kiln; bright brief shard then muted rest. |
| 13 | Slag glass | Snail/crust; broad dark reflective chip. |
| 14 | Coal crumb | Implemented shared coal crumbs: thrown coal landings, spent/broken stoves. Fuel-sack props remain pending. |
| 15 | Ash tuft | Sleeper/sack; light wind response, quick fade. |
| 16 | Tar fleck | Choir; soft sticky-looking short landing, no actual surface. |
| 17 | Paper slip | Clerk/cards; flutters, easily stirred by feet. |
| 18 | Tin curl | Implemented: grates and scrap bins; short curled-metal skid. Lunch boxes remain pending. |
| 19 | Filter fiber | Mask/loft sacks; pale tiny drifting threads. |
| 20 | Moth scale | Wings; sparse warm flecks, float then fade. |

## Twenty ambient sources

These are distinct offline-generated assets beyond combat cues. Loops follow
their visible local machinery and fade/fall off by listener distance. One-shots
use cooldowns, never a fresh independent chance every rendered frame. No ambient
sound triggers authoritative AI hearing. A semantic work action may separately
emit both a gameplay noise event and a matching local sound.

| # | Source | Placement and schedule |
| --- | --- | --- |
| 1 | Furnace breath | Implemented: low uneven loop follows loaded, undamped Stokers and fueled boiler tanks; frozen/empty sources fall silent. |
| 2 | Distant picks | Implemented: sparse taps follow a living, peaceful foreman with a linked worker cutting; suppressed within six tiles of the foreman. Minimum 12s after playback. |
| 3 | Chain sway | Implemented: quiet loose-chain loop follows active crane travel, grab and return; idle/frozen/stunned/sleeping cranes go quiet. |
| 4 | Belt rollers | Implemented: quiet local loop near visible powered rollers; fades out when the nearby drives are braked/broken. Falls off by distance, never produces gameplay noise. |
| 5 | Coolant trickle | Wall tap into an actual shallow pool; footsteps make rings. |
| 6 | Pipe drip | Positional irregular drops, 1–4s local cooldown. |
| 7 | Exhaust hiss | Short periodic vent loop, matches fixture duty cycle. |
| 8 | Timber creak | Rearmed area entry at a support passage, at least 20s apart. |
| 9 | Distant ore fall | Rare level event, at least 45s apart; no unseen damage. |
| 10 | Cooling metal ticks | Implemented: sparse ticks at frozen cranes/tanks or low-pressure tanks standing in cooling liquid; minimum 10s after playback. |
| 11 | Slag bubbling | Implemented: low glugging loop on actual lava; stops when its source tile cools into stone. |
| 12 | Air shaft draft | Directional soft loop at wall gaps. |
| 13 | Loose sign rattle | Trigger at a hanging warning sign, wind-themed local variation. |
| 14 | Office clock | Gentle sparse loop within the pay office. |
| 15 | Shift bell | Implemented: rare distant level cue, at least 60s between repeats; not a foreman order. |
| 16 | Muffled work song | Very quiet, nonverbal tonal texture beyond occupied workrooms. |
| 17 | Electrical cabinet hum | Local hum at a powered cabinet, stops when disabled. |
| 18 | Water hammer | Implemented: occasional pipe knock at fueled, pressurized, unfrozen boiler tanks; at least 18s between repeats. |
| 19 | Freight rumble | Distant short pass near rail rooms, not a damaging invisible train. |
| 20 | Kiln roof grit | Brief ceramic patter on entering a quiet furnace alcove. |

## Carryover and implementation order

Ice cold flasks/coolant cool kilns, clot tar, suppress stoker ignition and settle
hot slag. Grounding spikes and wire matter around welders, cable crawlers and wet
pumps. Mirrors/black felt remain useful against optical machinery if it appears;
do not insert optical enemies merely to force every carried item to stay optimal.
Forest fire still burns wooden stock, lights charges and feeds kilns, but many
native threats resist it. Water, oil, food bait, thrown rocks and sound lures
retain their ordinary uses. Item advantage comes from materials and situations.

Implement in cohesive playable slices: native terrain/props and workfront crew;
grates/rivet gunner and salvage tools; belts/carts and brake/crank tools; hot
creatures/cooling; pressure plumbing; offices/theft; hoists/casting; remaining
items, room compositions and ambience. Every new rule needs multiple uses before
it counts as integrated: belts move actors/cargo/carts; grates offer cover and
shooting lanes and can be demolished; work orders affect real crew, decoys and
player tools. Build and inspect static assets/scenes; human players own playtests.

# Industrial catalog: the underworks

Design specification, not a claim of implemented content. The runtime currently
has lava terrain, Ember enemies, native charcoal/molten terrain art, and
working Pickhand/Shift Foreman crews, their usable whistle, and finite-coal Ember Stokers. Track implementation in `../MASTER_TASKS.md`.
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
* Grates block bodies but pass flat shots and beams through visible openings.
  They have HP, stop melee reach unless the weapon explicitly reaches through,
  and can be broken. Molten baths still need safe banks; a grate is not a bridge.
* Powered belts move grounded actors, loose gameplay items and pushable fixtures
  one cell per beat. Flying enemies ignore them. Occupancy stops motion; no
  hidden pushing chains, overlap, diagonal movement or collision-free conveyors.
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
| 5 | Rail shunter | 120 HP, slow; pushes a separate cart down an existing rail after ringing a bell for 0.8s. Cart stops at obstruction, hurts on collision, spills real cargo when broken. Cut the route, brake the cart or flank the worker. Hand bell, wheel squeal. | Freight junction; 20% brake shoe, 20% rail switch key, 20% 4–8 gold. |
| 6 | Rivet gunner | 64 HP; braces behind a grate and fires three traveling rivets along a fixed lane, 0.15s apart, after a 0.5s buzz. Long 1.5s reload invites crossing. Displacement spoils the burst. Pneumatic buzz and three dry snaps. | Assembly line; 25% rivet gun, 20% ammo. |
| 7 | Magnet crane | 90-HP anchored machine; sweeps a visible lifting head toward loose metal, pauses 0.75s, then pulls the first marked target one cell. Can grab its own allies' shields; never deletes inventory. Break its chain, de-energize it or use nonmetal bait. Relay clack, rising coil hum. | Scrap yard; 25% horseshoe magnet, 20% copper wire. |
| 8 | Slag snail | 96 HP; lays a short hot trail, retreats into its shell before a slow two-cell lunge. Cold turns its trail into safe brittle crust and exposes a vulnerable body for 2s. Shell scrape and wet furnace burble. | Slag bank; 30% cinder sausage, 15% refractory paste. |
| 9 | Pressure rat | 22 HP; gnaws a leaky outlet, visibly inflates for 0.6s, then rockets in one cardinal direction until a wall. Pops in a small steam cross; cooling deflates it and removes the burst. Rubber squeak, kettle whistle. | Pipe crawl; 25% raw meat, 10% rubber hose. |
| 10 | Tar choir | Three 32-HP bodies sway together before coughing sticky, flammable tar at a fixed gap. Break line of sight between singers to stagger their rhythm. Cold clots the patch; fire ignites friends too. Low bubbling thirds, separate coughs. | Settling tanks; final body only: 30% tar flask, 15% solvent rag. |
| 11 | Walking kiln | 160 HP; four slow steps, door clanks open for 1s, then a wide short flame breath. Eats adjacent wood/coal between attacks to refuel. Open door takes extra damage; cool the mouth or feed it junk to occupy it. Hinge groan, ceramic roar. | Kiln court; 25% ceramic plate, 20% coal biscuit, 15% two coal lumps. |
| 12 | Audit clerk | 38 HP, normally neutral; follows loose gold, stamps it, then carries it toward a locked pay cage. Recover actual stolen coins on death. Its alarm calls nearby workers, never summons an infinite army. Stamp thump, paper shuffle, shrill bell. | Pay office; 25% punch card, 15% inspector stamp, 25% 3–7 gold. |
| 13 | Arc welder | 70 HP; lowers visor for 0.7s, then sweeps a two-cell electric arc across three facing lanes. Water/wire carries the received shock; grounding spikes absorb it. Blind side is open. Mask click, crackling arc, cooling tick. | Repair bay; 25% arc torch, 15% welding visor. |
| 14 | Ash sleeper | 44 HP; a flat ash mound wakes to nearby loud work, shakes for 0.5s and rises into a fragile swirling body. Calm air or wetness settles it; a fan can shove it into a furnace. Dust inhale, hollow sandy rattle. | Quiet ash loft; 25% ash sack, 15% filter mask. |
| 15 | Mold thief | 58 HP; steals one loose metal item and runs to a casting mold. Takes 3s to seal the lid; interrupt to recover it unchanged. Sealed molds remain breakable and contain that same item, not a rerolled replacement. Tongs clank, lid slap. | Casting floor; 20% foundry tongs, 20% mold key. |
| 16 | Cable crawler | 50 HP; advances along wall edges and lays a short wire tail. An exposed charging segment flashes for 0.8s before a pulse. Cut/ground the tail; it cannot shock through a deleted segment. Insulation creak, ticking relay. | Cable trench; 25% copper wire, 15% insulated boots. |
| 17 | Counterweight | 140 HP, anchored hoist; pursues through a visibly moving floor shadow, then drops after 0.8s. Crushes props and creatures equally and takes 2s to wind back up. Its chain is the target; destroyed machinery stays down. Chain rattle, heavy stone impact. | Hoist shaft; 25% chain hook, 20% counterweight bag. |
| 18 | Strikebreaker | 150 HP; shield forward, slow march toward a chosen worker's attacker. Shoves before attacking, making belts dangerous; blocks flat front shots but has a full 0.7s overhead hammer tell. Cool/grease footing or attack rear. Shield bang, boot march, hammer slam. | Guarded work front; 20% press hammer, 15% steel toe cap, 20% 5–9 gold. |
| 19 | Furnace moth | 18 HP, flying; circles active lamps and steals one small portion of exposed flame fuel before diving at the brightest nearby target. Extinguishing its bait redirects it. Charged wings glow before contact; no ground slipping. Paper wings, tiny gas flare. | Lamp gantry; 25% glow slag, 10% heat capsule. |
| 20 | Emergency pump | 100-HP mobile machine; connects to a nearby wet spill, fills for 1s, then warns 0.6s before spraying that liquid down a lane. Water quenches, oil spreads real fuel. Break/cool/turn its nozzle; no liquid duplication beyond its finite tank. Pump knock, slurp, sputtering jet. | Cooling works; 25% pocket pump, 15% nozzle elbow, 15% coolant can. |

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
| 1 | Rivet gun | Three-round traveling burst, 9 damage/rivet to 8; 12 loaded/24 spare, 1.5s reload. Stops at bodies, passes grates. Pneumatic rattle. | U / 30 |
| 2 | Press hammer | Implemented: 0.6s overhead windup, 32 damage and one-cell shove; 30 repairable condition. Double damage to blocking props; dig power 1 preserves reinforced routes. Taking damage cancels the windup without wear. Iron thud. | U / 25 |
| 3 | Arc torch | Hold to charge 0.3s, then adjacent 7-damage electric ticks every 0.25s; 8s total battery. Water/wire endangers the user. No instant full-room beam. Electrical spit. | R / 38 |
| 4 | Slag scoop | Collect one safe crust/ash patch, or scoop a hot patch while taking a little heat unless protected. Holds three loads; secondary flings the oldest material to 3. Metal scrape, granular pour. | C / 10 |
| 5 | Chain hook | Traveling hook to 5 catches a prop/cart and pulls it one cell per held beat; anchored targets pull the user instead. 24 casts, blocked lanes cut the line. Chain whip. | U / 22 |
| 6 | Quarry charge | Implemented: place facing ordinary stone. Three-second fuse cuts up to three forward cells; 24 damage along the cut, at the charge and one cell behind. Stack 3. Dig-power-2 reinforcement, other materials and outer boundaries stop it. Water quenches into a recoverable charge; cold slows the fuse. No modifiers. Fuse scratch, deep crack. | U / 19 |
| 7 | Rubber mallet | Implemented: 0.15s swing, 3 damage and one-cell shove; 70 repairable condition. Tap a boiler to shed 40 pressure without cutting it or spending fuel; below 25 cancels its tell. Sealed outlets refuse. Shoving into a wall can crush. Hollow rubber knock. | C / 9 |
| 8 | Bolt pouch | Fling five 2-damage bolts in a short fan; three handfuls. Bolts are cosmetic after impact; spent ammo is not infinitely recoverable. Tin scatter. | C / 7 |
| 9 | Steam lance | Fill at fresh water, heat at a furnace; spends one fill on a two-cell 18-damage steaming thrust. Cold thrust only wets. Reusable, long 0.45s windup. Valve hiss. | U / 24 |
| 10 | Nail board | 10-damage melee board, 35 condition; secondary lays it as a recoverable 16-damage ground trap, hurting the next grounded entrant including its owner. Flying enemies ignore it. Wood slap, nail crunch. | C / 11 |
| 11 | Foreman's whistle | Implemented: gives audible idle pickhands fixed lane destinations four tiles in the aimed direction for up to 4s. Angry workers refuse; audible real leader calls take them back. Six successful calls, 1.5s cooldown; Long reaches eight tiles, Durable has twelve calls. Successful calls wake sleepers and attract hearing enemies. Short-short-long call, worker answer, spent note. | U / 18 |
| 12 | Survey chalk | Draw three bright floor marks per use, six uses. Allies see them; crew uses marked work fronts when idle. Washes away; does not reveal unseen rooms. Chalk squeak. | C / 5 |
| 13 | Pocket drill | Hold 0.4s to start, then cut adjacent diggable rock every 0.2s while still; 6s battery. Loud and slower to retarget than a pick. Mechanical chatter. | U / 27 |
| 14 | Fuse scissors | Implemented: snip the shortest landed exposed fuse at your feet or ahead; recover that exact bomb into the pack. A full pack, airborne/expired fuse or sealed Prism/Thaw Charge rejects it without spending. Twelve snips; Durable 24, Fragile 6. Also works on Bomb, Firecracker and Pitch Bomb. Metallic click. | C / 8 |
| 15 | Brake shoe | Jam one adjacent belt/cart mechanism for 6s; can break under a heavy impact. Four shoes. Does not stop trains. Rubber drag. | C / 10 |
| 16 | Rail switch key | Rotate a nearby existing rail junction; twelve turns. Cannot conjure track or remotely turn occupied points under a cart. Lever clack. | C / 9 |
| 17 | Horseshoe magnet | Hold to draw one loose metal item along a clear five-cell lane. Also attracts a nearby active crane head toward the user. 15s active wear. Low metallic tremolo. | U / 18 |
| 18 | Foundry tongs | Carry one hot loose object at arm's length; release places it ahead. Occupies held use and prevents attacks, preserving the exact item/heat. Forty handling wear. Tongs clink. | C / 12 |
| 19 | Mold key | Open adjacent casting molds without breaking them; keep trapped items intact. Eight uses. Also releases a mold thief's sealed stash. Heavy key turn. | C / 8 |
| 20 | Pocket pump | Collect a real shallow spill into three-cell capacity; secondary discharges one portion ahead. No draining whole lakes/lava; stored liquid type cannot mix. Hand pump squeak. | U / 23 |
| 21 | Nozzle elbow | Fit a compatible machine outlet to bend future jets ninety degrees. Recover while idle; current warned shot keeps its marked lane. 18-HP fitting. Threaded metal rasp. | U / 14 |
| 22 | Rubber hose | Connect two nearby outlets across up to four clear floor cells; redirects a finite supplied flow, does not generate pressure. Cut/fire breaks it. One recoverable coil. Rubber unroll. | U / 16 |
| 23 | Hand bellows | Four short wind shoves, no damage; feeds exposed flame, moves ash and smoke, pushes moths. Walls block, actors stop as normal. Wheezy puff. | C / 10 |
| 24 | Inspection mirror | Hold to look around one adjacent corner with a narrow reflected sight wedge. Cannot shoot through it; hard hits break its 12 condition. Tiny handle rattle. | U / 17 |
| 25 | Belt crank | Temporarily drive a stopped adjacent belt while held, using the user's action beat. Requires power-free accessible mechanism; no automatic indefinite motion. Forty turns. Ratchet clicks. | C / 11 |
| 26 | Tar flask | Traveling throw to 5 leaves a short sticky fuel patch for 10s. Stack 3. Slows friends; cold clots it, fire makes a dangerous crossing. Thick glug. | C / 8 |
| 27 | Coolant can | Pour three adjacent cells to quench/cool; four uses. Cools machines faster than water but leaves slippery residue. No lava swimming immunity. Watery metal glug. | C / 12 |
| 28 | Refractory paste | Coat held tool for six contacts with hot objects, avoiding heat wear; does not give the player full fire immunity. Three applications. Gritty smear. | U / 14 |
| 29 | Solvent rag | Clean adjacent tar/oil or a fouled mechanism; five wipes. Used rag becomes flammable until rinsed. Cannot erase an entire room. Cloth swish. | C / 6 |
| 30 | Ash sack | Dump a three-cell fan that smothers exposed ground flame and quiets steps for 8s; two dumps. Wind scatters it; hot machines remain hot. Dusty thump. | C / 7 |
| 31 | Ceramic plate | Hold a directional plate against one hot projectile; breaks on the block. Physical bullets still hurt. Stack 3 unused plates. Ceramic ping/shatter. | C / 10 |
| 32 | Welding visor | Hold to shield facing light/arc glare and reduce short frontal arc damage; narrows vision while down. 20s active wear. No all-direction shock immunity. Visor snap. | U / 19 |
| 33 | Insulated boots | 4s resistance to shocks entering through the floor, with slower steps; six activations. Direct arc contact still hurts. Squeaky rubber steps. | U / 20 |
| 34 | Steel toe cap | Next three voluntary kicks/shoves gain force; also safely kicks one armed ground object away. Twelve charges, one per successful kick. Cannot shove anchored hoists. Clang. | U / 15 |
| 35 | Filter mask | Hold to resist smoke/ash breathing penalties, with muffled local audio and narrower sight. 30s filter life; no protection from actual flame. Cloth breath. | U / 16 |
| 36 | Lunch tin | Eat one of two portions for 10 HP over 3s; damage interrupts. Empty tin remains and can be thrown as a loud distraction. Lid pop, chew, hollow landing. | C / 9 |
| 37 | Cinder sausage | Eat for 8 HP and 8s warmth; briefly attracts furnace moths and heat-seeking creatures. Dropped sausage is real food bait. Stack 4. Crunchy munch. | C / 6 |
| 38 | Coal biscuit | Awful snack: lose 2 HP, resist nausea for 12s; alternatively feed it to a kiln as fuel. Stack 5. No free healing conversion. Dry crunch. | C / 3 |
| 39 | Salt tablet | Clears one heat-fatigue penalty but gives 2s thirst/slow recovery; use only when affected. Stack 6. Never a requirement to survive normal floor temperature. Capsule click. | C / 5 |
| 40 | Emergency foam | Traveling can to 4, 1s expansion then radius-1 fire smothering and temporary soft cover. Two cans. Bullets tear it; heat slowly consumes it. Expanding fizz. | U / 18 |
| 41 | Punch card | Insert into a marked service reader for one optional access/maintenance cycle. Stack 3, consumed; main route never requires a randomly found card. Paper punch. | C / 7 |
| 42 | Inspector stamp | Mark one idle worker/machine for inspection: it pauses one work cycle unless already fighting. Five stamps; foreman notices repeated misuse. Official thump. | U / 13 |
| 43 | Counterweight bag | Place a heavy 30-HP bag on a pressure plate, or throw to 3 for a 10-damage shove. Recoverable; belts carry it, flying cannot trigger plates. Sand thud. | U / 17 |
| 44 | Folding barricade | Place three linked low grate sections across a lane, 20 HP each. Walkers stop, shots pass. One kit; break individual sections to open gaps. Hinges clatter. | U / 23 |
| 45 | Warning cone | Place a bright recoverable marker; idle crews route around it, angry crews kick it aside. No physical immunity or invisible wall. Stack 3 unused cones. Rubber bonk. | C / 4 |
| 46 | Glow slag | Recoverable throw to 6, 4 damage; lights its landing cell for 20s. Cold turns it dark, furnace reheats it. Carries remaining heat through trading. Stack only identical cold pieces. Glassy clink. | C / 6 |
| 47 | Tension spring | Place a directional one-use launch pad; next grounded actor/cart gets a three-cell shove with normal collisions. Stack 3; fire weakens it before trigger. Compressed squeal, sproing. | U / 15 |
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
| Lamp room | Small safe entry with two exits and room for four players | Spare lamps, idle moth, visible cold-water tap; teach heat attraction without a mandatory fight. |
| Work front | Bent gallery, breakable interior spur, protected outer wall | Foreman and up to three pickhands, chalk marks, one side bypass. Their digging opens optional space. |
| Blasting alcove | Two offset pockets around a thick rock seam | Powder monkey and exposed fuse, clear retreat bay, optional seam-cache reward. |
| Rail junction | Broad T crossing with walkable margins | Shunter/cart, lever points, crate platform; no rails through party spawn or mandatory gate footprint. |
| Assembly line | Two belt lanes joined by stationary islands | Rivet gunner behind breakable grates, crank, brake shoe supply; ordinary safe walking route. |
| Scrap yard | Open yard, isolated metal piles, crane corner | Magnet crane, scrap effigy bait, scrap bin. Loose real metal is sparse; most litter is cosmetic. |
| Slag bank | Curved hot basin with two dry shores | Snail, cooling splash source, brittle optional shortcut; no required lava crossing. |
| Pipe crawl | Narrow alternate service lane parallel to main hall | Pressure rats, readable leaking outlet and cutoff valve. Can be bypassed or deliberately vented. |
| Settling tanks | Paired rectangular pits with dry central bridge | Tar choir with broken sightlines, solvent shelf. Fire clears tar but endangers the bridge approach. |
| Kiln court | Wide U around one furnace | Walking kiln and burnable stock, alcoves to bait a breath, ceramic chest. |
| Pay office | Quiet side room with window grates | Clerk, pay cage and ledger; optional theft alarms existing nearby crew, no infinite respawn. |
| Repair bay | Offset benches framing two short lanes | Welder, drain, grounding point, toolbox. Water is useful cooling and dangerous conduction. |
| Ash loft | Broad quiet shelf with a single noisy floor strip | Sleepers, bagged ash, fan outlet; acoustic choice before combat. |
| Casting floor | Two connected yards and a mold recess | Mold thief, breakable molds, tongs rack; stolen loot stays recoverable. |
| Hoist shaft | Ring around a marked lifting rectangle | Counterweight, chain target reachable from both sides, pressure plate holding an optional cache. |
| Cooling works | Reservoir beside a dry pump platform | Emergency pump, alternate nozzle positions, coolant locker; core route remains dry. |
| Service reader | Small optional locked branch | Punch-card reader beside a breakable alternate route. Consumable access is never the sole mandatory exit path. |
| Shift shelter | Safe-ish reunion room, camp stove and benches | Lunch box, repair supplies; occasional shop approach and human-scale quiet. |

Required objectives still obey the route graph. Compound key+lever or two-key
gates need independently reachable prerequisites and a real cut across all exit
paths, including grates, nearby digging and belt displacement. Workfronts cannot
excavate quest seals. Routes may permit deliberate tool-earned shortcuts; they
must not accidentally leave the key irrelevant as the earlier misplaced door did.

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
| Scrap bin | 18-HP sheet metal, noisy opening | 20% horseshoe magnet, 20% two glow slag, 20% copper wire, 15% chain hook. |
| Pay cage | Locked optional 60-HP bars, shoot-through cover | 60% 8–14 gold, 20% punch card, 10% inspector stamp. Clerk-carried gold returns separately. |
| Ceramic chest | 20-HP brittle shell, ordinary opening | 30% two ceramic plates, 20% refractory paste, 15% furnace seed, 15% coal biscuit. |
| Machine spares | 25-HP crate | 20% nozzle elbow, 20% rubber hose, 15% brake shoe, 15% belt crank, 10% overtime clock. |
| Casting mold | 40-HP hinged iron | Real stolen cargo if present; otherwise 25% steel toe cap, 20% rivet gun, 20% press hammer. Never both cargo and bonus reroll. |

## Twenty debris materials

All are local, with varied rotation and a few silhouettes each. Use short-lived
sparks in addition to settled solids; no fragment-to-fragment simulation.

| # | Material | Source; movement character |
| --- | --- | --- |
| 1 | Basalt chip | Mined walls; short heavy skid. |
| 2 | Ore flake | Ore seam; small metallic skip, dull highlight. |
| 3 | Brick corner | Furnace wall; angular tumble, fast stop. |
| 4 | Mortar dust | Brick damage; fine short puff, settles/fades. |
| 5 | Timber splinter | Supports/crates; elongated skitter, burns only visually. |
| 6 | Bent nail | Wooden stock; one hard bounce then flat. |
| 7 | Rivet casing | Rivet gun ejection; tiny brass roll. |
| 8 | Steel washer | Machinery break; edge roll then wobble. |
| 9 | Chain link | Broken hoist; heavy localized tumble. |
| 10 | Copper strand | Cut cable; curled light drag. |
| 11 | Rubber scrap | Hose/boots; springy bounce, high friction. |
| 12 | Ceramic shard | Plates/kiln; bright brief shard then muted rest. |
| 13 | Slag glass | Snail/crust; broad dark reflective chip. |
| 14 | Coal crumb | Fuel sack; brittle black pieces, little roll. |
| 15 | Ash tuft | Sleeper/sack; light wind response, quick fade. |
| 16 | Tar fleck | Choir; soft sticky-looking short landing, no actual surface. |
| 17 | Paper slip | Clerk/cards; flutters, easily stirred by feet. |
| 18 | Tin curl | Lunch box/cabinet; hollow flutter and short roll. |
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
| 1 | Furnace breath | Low uneven loop at fueled furnace mouths. |
| 2 | Distant picks | Sparse room-area taps beyond a work front. |
| 3 | Chain sway | Hoist anchor loop, quiet and slow. |
| 4 | Belt rollers | Loop only beside a powered visible belt. |
| 5 | Coolant trickle | Wall tap into an actual shallow pool; footsteps make rings. |
| 6 | Pipe drip | Positional irregular drops, 1–4s local cooldown. |
| 7 | Exhaust hiss | Short periodic vent loop, matches fixture duty cycle. |
| 8 | Timber creak | Rearmed area entry at a support passage, at least 20s apart. |
| 9 | Distant ore fall | Rare level event, at least 45s apart; no unseen damage. |
| 10 | Cooling metal ticks | Quiet series beside cooled machinery. |
| 11 | Slag bubbling | Low loop near a hot basin, distinct from water. |
| 12 | Air shaft draft | Directional soft loop at wall gaps. |
| 13 | Loose sign rattle | Trigger at a hanging warning sign, wind-themed local variation. |
| 14 | Office clock | Gentle sparse loop within the pay office. |
| 15 | Shift bell | Rare distant level cue, at least 60s apart; not a foreman order. |
| 16 | Muffled work song | Very quiet, nonverbal tonal texture beyond occupied workrooms. |
| 17 | Electrical cabinet hum | Local hum at a powered cabinet, stops when disabled. |
| 18 | Water hammer | Occasional pipe knock around cooling works, at least 15s apart. |
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

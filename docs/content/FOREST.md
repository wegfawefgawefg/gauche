# Forest catalog

Design specification for the forest content pass. Implementation progress remains
in `../MASTER_TASKS.md`; entries here are targets, not claims that they all exist.
Use the existing shared essentials alongside these regional finds. Keep low-HP
wildlife, dangerous combinations, readable tells and six-slot inventory decisions.
All timings use a 60 Hz simulation. Prices and damage are starting values for the
user's playtesting, not a finished balance model.

## Twenty adversaries

Every row has an explicit empty-drop outcome. The passive dog, chicken flock and
rabbit remain additional wildlife; they are not used to pad this target.

| Enemy | Behavior and tell | Habitat; drop table |
| --- | --- | --- |
| Wolf | Pursues in a pack; followers take a flank while the leader howls. Fast movement, committed bite. | Thicket/den; 20% meat, otherwise nothing. |
| Bear | Territorial, returns home; rears before a broad adjacent slam. Slow enough to circle, dangerous beside walls. | Den/shrine; 50% two meat, otherwise nothing. |
| Bat | Circles, then commits to a short dive; rests briefly after missing. Wingbeat and squeak mark the turn. | Brook/cave; 10% bitter root, otherwise nothing. |
| Zombie | Mostly wanders and scratches neighbors; event zombies deliberately hunt the party. Growl and separate scratch effect. | Ruins/mansion; 33% pocket gold, otherwise nothing. |
| Zombie stack | Wobbles forward as several bodies; one death topples the survivors, with a delayed spill in cramped cells. | Late ruins/mansion; each killed body has a pocket-gold roll. |
| Boar | Snorts and locks a cardinal lane, charges, shoves and can wall-crush; a blocked charge leaves it stunned. | Trail/thicket; 35% meat, otherwise nothing. |
| Thorn snail | Its front shell deflects blows; hitting it makes a temporary immovable shell, useful as a crush surface. Back attacks or digging tools bypass the shell. | Root tunnels; 15% thorn caltrops, otherwise nothing. |
| Lantern moth | Seeks the strongest nearby actual emitter, circles it, and sheds sleep dust at close range. A planted lamp can lure it away. | Canopy/cave; 15% lantern seed, otherwise nothing. |
| Spore toad | Swells with an audible croak before a sleep puff, then hops away. Its puff affects other creatures too. | Brook/mushroom patch; 25% mushroom spores, otherwise nothing. |
| Root turret | A rooted plant turns slowly, marks a lane and lashes through it. Cannot pursue; can be routed around or baited into other actors. | Thicket/old shrine; 20% seed bag, otherwise nothing. |
| Bramble guard | Keeps a generation-checked protected ally, physically interposes between it and an attacker, and returns to territory when orphaned. | Guarded shrine; 15% resin glue, otherwise nothing. |
| Mosquito | Brief hover, dart, drain health, then retreat while digesting. A successful bite heals it; missing leaves an opening. | Water/grass; 10% water flask, otherwise nothing. |
| Woodpecker | Repeated straight drilling run chips props and eligible terrain; pauses with its beak stuck after a hard collision. | Rotten wood/workshop; 15% digging claws, otherwise nothing. |
| Burrow worm | A linked head and short chain follow saved cells; killing a link divides the chain, exposing the remaining heads. Bodies count as one species. | Root tunnel; head has 20% bitter root, otherwise nothing. |
| Owl | Watches a territory from stillness, marks the player's previous position, swoops there, then returns home. | Dark clearing; 15% bird seed, otherwise nothing. |
| Crate mimic | Waits disguised as a supply crate; a nearby actor wakes it. It bites, retreats, and hides again after losing its target. | Cache/workshop; 50% a real supply find, otherwise empty. |
| Forager goblin | Patrols two points, takes loose useful items, visibly carries them and flees home. Returns stolen contents on death. | Camp/workshop; stolen contents always, 50% small gold, otherwise nothing. |
| Carrion crow | Feeds on meat finds, calls other crows, steals food from a careless adjacent carrier, and retreats to its perch. | Ruins/animal dens; carried food always, otherwise nothing. |
| Wasp nest | Stationary destructible source with a finite brood; buzzing swells before releasing linked wasps. Smoke/sleep suppresses the brood beat. | Orchard; 40% honey pot, otherwise nothing. |
| Wasp | Follows a nest/leader, remembers a threat, then attacks in staggered stings instead of occupying one tile together. Orphaned wasps scatter. | Orchard/nest; 5% honey, otherwise nothing. |

Use shared points, handles, counters, labels and timers for these behaviors. Keep
species-specific meanings beside their functions. Room membership has a separate
handle so a bodyguard or worm does not lose its AI references during a wave.

## Fifty regional items

Patterns describe cells relative to the user, aimed right and rotated into the
other three cardinal directions. State the actual pattern, damage, cooldown,
charges/durability and relevant status effects in the same comparison UI used by
shared weapons. Attributes alter the same pattern/damage data used by gameplay.
Common = ordinary local pool; uncommon = guarded/side-room pool; rare = secrets or
rare reward/shop rolls. Failure should not silently consume the item.

| # | Item | Pattern, state and distinct use | Rarity / initial price |
| --- | --- | --- | --- |
| 1 | Throwing rock | Recoverable throw, line 1–6; 8 damage, 30-tick beat. Lands at the target's feet or last free tile. | Common / 3 |
| 2 | Hatchet | Three-cell adjacent cleave; 16 damage, 35 ticks, 35 uses. Cuts wooden props without gaining stone dig power. | Common / 14 |
| 3 | Hunting spear | Two-cell piercing thrust; 12 damage, 28 ticks, 45 uses. | Common / 15 |
| 4 | Crossbow | Line 1–14, one loaded bolt, slow reload; 32 damage. Each instance owns its ammunition. | Uncommon / 30 |
| 5 | Blunderbuss | Short three-lane cone 1–5; 10 damage per lane, two-shot magazine, heavy recoil. | Uncommon / 32 |
| 6 | Wooden maul | One adjacent cell; 28 damage, 60 ticks, shove on hit, 24 uses. | Uncommon / 23 |
| 7 | Rake | Three adjacent cells across the facing edge; 7 damage, 24 ticks. Clears grass and catches small swarms. | Common / 9 |
| 8 | Flint knife | One cell, 7 damage, 12 ticks; double damage against a sleeping or rear-facing target. | Common / 12 |
| 9 | Boomerang | Outward line 1–6 and a returning flight; can hit once on each leg. Unavailable until it returns or lands. | Uncommon / 24 |
| 10 | Torch | Held light; adjacent strike ignites flammable material, 6 damage, 90 fuel uses. | Common / 8 |
| 11 | Lighter | Adjacent spark, 20 uses; little direct damage, useful with oil, sap and dry growth. | Common / 6 |
| 12 | Oil flask | Thrown 1–5, three-by-three slick; flammable, slippery, three flasks per stack. | Common / 8 |
| 13 | Sap jar | Thrown 1–4, sticky cross; slows steps and burns longer than oil. | Common / 9 |
| 14 | Water flask | Three adjacent cells; extinguishes actors/ground fire and cools a little hot terrain. | Common / 5 |
| 15 | Seed bag | Plant the next cell; a shoot grows into destructible cover after a visible delay. Six seeds. | Common / 8 |
| 16 | Mushroom spores | Small sleep cloud at range 1–3; affects friends and enemies, damage wakes them. | Common / 10 |
| 17 | Smoke pot | Throw 1–5; obscures creature targeting in a small area without changing authoritative lighting math. | Common / 10 |
| 18 | Stink bomb | Throw 1–4; a lingering scent draws scavengers and distracts hunting animals. | Common / 7 |
| 19 | Honey pot | Place bait; draws bears and wasps, sticky to walk through. A deliberate trap ingredient. | Common / 8 |
| 20 | Hand bell | Audible radius 10; wakes sleepers and pulls investigation toward the user. Durable, no damage. | Common / 5 |
| 21 | Hunting horn | Facing cone 1–3 (one/three/five cells wide); shoves each target once and wakes it. Walls crush, friends count. 100-tick cooldown, eight uses. | Uncommon / 22 |
| 22 | Wolf whistle | Narrow call 1–6; briefly redirects an unattached wolf toward another hostile actor. Limited uses. | Uncommon / 20 |
| 23 | Bird seed | Place a small bait pile; nearby birds/flocks gather rather than creating endless free animals. | Common / 4 |
| 24 | Herb bag | Self heal over time, 18 HP total; cannot stack its own regeneration. Four portions. | Common / 7 |
| 25 | Splint | Self; removes movement stun and briefly protects against another root/stun. Two uses. | Common / 6 |
| 26 | Bitter root | Self; wakes the user and gives short sleep resistance, with a small immediate HP cost. | Common / 5 |
| 27 | Chili | Self; faster steps for a short burst, followed by a brief weak burn. Three peppers. | Common / 7 |
| 28 | Rotten fruit | Throw 1–4; splats a nauseating patch, useful as scavenger bait; eating it is a bad heal. | Common / 2 |
| 29 | Firecracker | Place/throw 1–4; delayed bang wakes, startles and briefly stuns nearby small creatures. | Common / 7 |
| 30 | Pitch bomb | Throw 2–6; delayed burning splash with sticky residue. Strong against growth, dangerous around friends. | Uncommon / 18 |
| 31 | Acorn mine | Place next cell; actor contact bursts cardinal splinters. Three mines per stack. | Uncommon / 16 |
| 32 | Thorn caltrops | Scatter over three facing cells; repeated small step damage, finite thorns. | Common / 10 |
| 33 | Rope snare | Place next cell; traps movement without the bear trap's huge damage, recoverable after release. | Common / 9 |
| 34 | Throwing net | Three-cell wide cast 1–3; briefly roots actors while leaving their attacks available. | Uncommon / 15 |
| 35 | Shield lantern | Facing block with a light; 24 durability, weaker than a plain buckler but illuminates a fight. | Uncommon / 24 |
| 36 | Reflecting pan | Short active parry window; returns a direct ranged hit, costs durability even on success. | Rare / 32 |
| 37 | Digging claws | Adjacent dig-power-1 strike; quick but only 18 uses, modest actor damage. | Uncommon / 17 |
| 38 | Root drill | Bores a committed line 1–5 through eligible terrain; loud, three charges, dig power 3. | Rare / 38 |
| 39 | Rope hook | Traveling line 1–6; reels an actor/loose item or the user toward a solid anchor, up to three steps. Sideways motion breaks the tether. 75-tick cooldown, 24 uses. | Uncommon / 25 |
| 40 | Swap seed | Line 1–5; swaps with the first movable actor if both destinations remain legal. | Rare / 32 |
| 41 | Pocket door | Two placements create linked personal passages; finite pair, may bypass a normal lock. | Rare / 45 |
| 42 | Spring trap | Place facing next cell; launches the next actor two cells in that direction, with ordinary crush rules. | Uncommon / 20 |
| 43 | Straw decoy | Place next cell; has HP and draws nearby enemy attention until destroyed. | Common / 12 |
| 44 | Sticky boots | Activate self; temporarily resists pushes and slipping, but slows ordinary steps. | Uncommon / 18 |
| 45 | Resin glue | Repairs the most damaged carried durable item; does not refill ammo or consumed charges. | Uncommon / 14 |
| 46 | Scarecrow | Place a destructible ward; birds and small animals hesitate around it, larger predators do not. | Uncommon / 16 |
| 47 | Lantern seed | Plant a stationary dim lamp; useful for seeing and drawing moths away. Four seeds. | Common / 8 |
| 48 | Thunder acorn | Line 1–5 to first target, then a short adjacent chain; wet targets conduct farther. | Rare / 30 |
| 49 | Fungal bread | Immediate 25 HP heal followed by a short sleep; someone else should guard the eater. | Common / 9 |
| 50 | Rabbit charm | Use to retreat several cells opposite aim, stopping at obstructions; three escapes, no permanent dodge stat. | Rare / 28 |

## Room and material sources

Clearings and orchards have broad calm ground, occasional canopy gaps, leaf piles,
seed husks and wildlife. Thickets combine dry grass, sap-bearing growth, snails,
boars and root turrets; ordinary walking routes stay clear. Brooks have dry banks,
small crossings, reeds, moths, mosquitoes and toads. Root tunnels introduce worms,
cuttable obstructions and optional digging shortcuts. Ruined courts hold zombies,
loose masonry, urns and old tools. Workshops favor goblins, mimics, wooden crates,
repair materials, firearms and their own ammo. Dens supply beasts and meat; their
spawners must have a finite brood or a clear pressure cap.

Add discrete surface rules for oil, sap, water, heat, smoke and scent so items can
interact with materials rather than enumerating every item/enemy pair. Sleep,
root, burning and knockback are gameplay; dust, flame wisps and loose scraps are
local presentation. Enemy attack previews come from committed cells/directions,
so a visible windup is an actual opportunity to dodge or shove the attacker.

Container pools: supply crate (ammo/medicine/tools/empty), seed basket
(seeds/herbs/bait/empty), rotten stump (root/spores/insects/empty), old urn
(gold/odd charm/empty), locked strongbox (weapon/utility plus gold), abandoned
lunch tin (food/water/empty). A mimic is an actor wearing a crate silhouette;
ordinary containers remain cheap props. Loot is rolled once on breaking/opening.

## Twenty loose materials

The forest's current local debris collection already contains oak leaf, birch
leaf, pine needle, twig, bark, wood chip, root, fern leaf, grass blade, mushroom
cap, mushroom stem, spore, acorn, seed husk, feather, bone chip, pottery, cloth,
brass case and stone chip. Connect each added content source to the relevant
family. Use settled piles and friction/wind/step impulses, not mutual rigid-body
collisions. None of these scraps changes gameplay or the network hash.

## Sound production

Each adversary needs its tell plus an impact/death cue appropriate to flesh,
shell, leaves, wood or metal. Reuse material impacts deliberately; do not give
every species the zombie scratch sound. New weapons need a recognisable use cue;
firearms also need reload and empty-magazine cues. Produce short variations of
frequent footsteps, flaps, stings and shots with controlled headroom. Offline
sources live in `tools/sound`; commit finished OGG files with their content slice.

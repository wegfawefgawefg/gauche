# Teeming content sketch: forest world

This is a candidate pool, not a locked item list. The first forest floor should
make fists, a stick, a bow, herbs, animals, traps, and route shortcuts feel
different before the game asks players to understand a large inventory.
The wider per-biome targets and new room direction are in
[the content farm brief](CONTENT_FARM_BRIEF.md).

## Design bias

Make the run **item-led**. Keep player-wide numbers legible: current/max HP and
the number of ticks between tile steps. Weapon damage, magazine size, reach,
reload, and any blocking strength belong to the item itself. Use a few named
item modifiers that visibly change behavior, plus rare authored uniques.
Avoid a global strength/agility/defense/crit/armor-penetration spreadsheet.

Armor is easiest to read as a shield, coat, or enemy trait with a clear effect:
for example, a buckler blocks one facing hit, a heavy coat reduces one point
of ordinary damage but slows steps, and a piercing spear bypasses a plated
enemy. A universal armor-penetration percentage would add bookkeeping without
making the first forest fight more interesting. Likewise, prefer a timed
sidestep, conditional backstab, or first-hit evasion over hidden passive dodge
and crit rolls. A specific crit weapon can still have a documented chance,
using the saved gameplay RNG.

The buckler's base use is a facing block or one-tile shove. A shoved actor
crushes instantly if the destination is a hard, impassable blocker: solid wall,
fixture, or an enemy that cannot be moved. A player or teammate shoved into a
wall can die this way; the blocker survives. An ordinary teammate or loose
item is not itself a crush surface. A movable item shoved into a wall breaks.
This is a strong spatial weapon: facing, telegraph, and recovery time
balance it, rather than a small damage roll.

Let enemies outscale the party through combinations and situations: more
aggressive mixes, dens, tight rooms, and new tells. Merely multiplying wolf HP
each floor would make the many weapon effects matter less.

## Forest floor generation

Choose the route graph first, then embed its room stamps on an expandable tile
canvas. Eight to twelve rooms is a starting test for forest floor one, not a
map-size ceiling; later floors can have more rooms and longer branches. Stamps
declare cardinal door sockets and may have different tile dimensions. Rotate
or mirror one only where its fixtures still work. Match required sockets,
connect corridors, and add a few loops and optional dead ends. Trim/pad the
finished bounds to fit the layout. Validate a walkable route from spawn to
exit, plus key-before-lock order. Train rails, pickaxes, and bombs may create
extra routes but are never needed to make a floor solvable. Irregular cave
walls, roots, broken tree lines, pools, and lighting keep the assembly from
looking like visible square modules while tile-by-tile collision stays clear.

Give each room a **role and tags**, not a single global loot roll. A room can
specify its own weighted loot pool, plus spawn slots for enemies, passive
animals, fixtures, light, and secret passages. A floor-wide budget then keeps
the combined result sensible: enough early weapons for the party, at least
one modest healing opportunity, and no firearm without a plausible way to
find ammo. Scale opportunities with player count rather than guaranteeing that
every player receives the same starter weapon. A player may still choose to
punch through the first rooms.

| Room stamp | Job on floor 1 | Local loot candidates |
| --- | --- | --- |
| Abandoned camp | Safe spawn, first landmarks, usable fire | Stick, herbs, tiny coin stash |
| Forked trail | Three-way connection with a loop opportunity | Nothing or small cache |
| Fallen tree | Soft chokepoint; chop, bomb, or go around | Stick, hatchet, hidden herb bag |
| Rabbit meadow | Passive bunnies and a little cover | Herbs, rare raw meat |
| Chicken clearing | Existing chickens, optional campfire interaction | Rare raw chicken, bandage |
| Herb glade | Low-risk supply branch | Bag of herbs, sleep herbs, bandage |
| Hunter blind | Elevated-looking but still grid-based firing lane | Bow, arrows attached to bow, rare musket |
| Shallow pool | Bridge/switch or dry bank choice | Ammo box, coin purse |
| Cave mouth | Change from roof light to local torchlight | Torch, shield, mushrooms |
| Root tunnel | Narrow collision puzzle with alternate cut-through | Pickaxe, small cache |
| Old shrine | Optional detour with one memorable find | Artifact or unique item |
| Small den | Optional risk: capped wolf or bat spawner | Better weapon, meat, coins |
| Broken watchpost | Fixed enemy and a visible switch/key | Shield, musket, coins |
| Exit hollow | Clear landmark and safe party gathering space | Usually no random loot |

An example first-floor route is camp → trail fork → cave mouth → watchpost →
exit. The glade and hunter blind form a side loop; a den is a dangerous branch.
The exact room identity can vary by seed while the roles remain. Floor 1 needs
mostly readable hazards. Crusher rooms, stacked zombies, and long spike runs
belong later, after players know the step rhythm.

Passive rabbits and chickens need no hunger system. Give them a low, fixed
chance to drop raw meat when killed (10% is a test value), then let a fire turn
it into a small heal. Some runs produce no meat. That makes a campfire useful
without turning the floor into a food farm. Track the drop roll and cooking in
deterministic gameplay state.

## Weapon and usable-item pool

These are behavioral identities, not damage-tier names. Each ranged weapon
instance owns its loaded/spare count; generic ammo pickups add to that weapon
or separately to several carried weapons. There is no shared ammo reserve.

| Candidate | Distinct job |
| --- | --- |
| Fists | Always available; short reach, quick swing, can shove a small foe |
| Stick | Common first weapon; wider swing and can prod a spike tile safely |
| Flint knife | Fast adjacent hit; stronger from behind or on sleeping targets |
| Hatchet | Slow cleave that also cuts roots and wooden barricades |
| Spear | Hits one tile farther, but awkward when surrounded |
| Pickaxe | Cracks stone shortcuts and armored shells; slower melee |
| Buckler | Active facing block or shove; a hard-blocker collision squishes the pushed actor |
| Bow | Quiet aimed shot, own arrow count, longer nock time |
| Sling | Cheap fast projectile, weak against armor, can wake or lure enemies |
| Crossbow | Strong line shot with a slow reload |
| Musket | Loud high-impact shot; valuable but invites nearby threats |
| Pistol | Reliable compact gun and quick reload |
| Revolver | Fewer shots, precise, strong against a single target |
| Shotgun | Close cone, knocks targets back into hazards |
| Nail gun | Pins a target briefly to a wall or barricade |
| Sleep dart gun | Low damage but puts an isolated target to sleep |
| Flare gun | Lights dark rooms and ignites oil or dry brush |
| Grenade launcher | Arcing blast around cover, changes tiles |
| Rocket launcher | Rare room-breaking blast with its own rockets |
| Conductor hat | Existing rail layer then train; extreme route-making unique |

Usable finds can include a bag of herbs, bandage, sleep meds, torch, lantern,
bear trap, smoke pot, oil flask, bomb, barricade bundle, and animal lure. Each
should solve a room problem as well as have a combat use. A shield, pickaxe,
torch, or barricade can be more valuable than another gun because the exit
route itself is part of the fight.

## Named item modifiers and upgrades

Start with one special modifier on a found weapon, or an authored unique with
two designed effects. Do not roll arbitrary stacks of every modifier onto
every weapon. Upgrades in a floor-clear draft can attach to the chosen item,
and swapping the item should carry its modifier with it.

| Modifier | Visible rule |
| --- | --- |
| Double swing | Second melee arc a few ticks after the first; can hit a new tile |
| Ember | Applies burning; also lights brush, oil, and cooking fires |
| Drowsy | Repeated hits build sleep; heavy damage wakes the target |
| Corpseburst | Killed enemy leaves a short-fuse blast; position matters |
| Piercing | Shot continues through one target or ignores a plated trait |
| Ricochet | First wall hit redirects a projectile once |
| Rooting | Hit briefly prevents movement, not attacks |
| Rebounding | Shield block automatically shoves the attacker back one tile |
| Quickdraw | First shot after switching weapons fires sooner |
| Miserly | Every few hits return one round to that weapon's spare count |
| Long haft | One extra melee tile, with a slower swing |
| Cinder wake | A killed burning enemy ignites its tile briefly |

Make special interactions explicit. Ember plus Corpseburst is a burning chain;
Rooting plus a bear trap can hold a charging wolf in a lane; a shield can push
an enemy onto a spike floor. These are the build discoveries to chase, rather
than a tiny percentage increase to a hidden rating.

## Artifact and floor-clear reward pool

Artifacts are run-long passives outside the six quick slots. Auras use tile
distance and named stacking rules. Rare named artifacts can be once per run;
common artifacts can recur if their behavior remains legible. Candidates:

| Artifact | Effect |
| --- | --- |
| Hare's Anklet | Step one tick sooner, subject to the movement-speed floor |
| Bear Heart | +2 maximum HP and heal those two HP immediately |
| Moss Stitch | Heal a small amount after each cleared floor |
| Butcher's Thread | Passive animals more often leave meat; cooked meat heals more |
| Campfire Tin | Cooking at a fire produces one extra portion for a friend |
| Lantern Bell | Nearby allies gain a small local light source |
| Pack Standard | Nearby allies step faster briefly after you block a hit |
| Medic's Ribbon | Healing yourself also gives an adjacent ally a small heal |
| Wolf Tooth | A melee kill grants a short movement burst |
| Grave Wick | Burning enemy corpses spread a little fire |
| Bone Counter | Every fifth kill adds spare ammo to the held weapon |
| Spare Spring | First reload after entering a room is faster |
| Crow's Eye | Marks a hidden side-room entrance when nearby |
| Rail Pass | Its holder can stand on an active rail without train damage |
| Rabbit Charm | Once per world, a lethal hit leaves you at one HP |
| Hunter's Knot | Shots against sleeping enemies pierce the first target |
| Iron Oath | While holding a shield, adjacent allies take less ordinary damage |
| Ashen Pouch | First bomb in each floor ignites the blast area |
| All Piercing | Every direct weapon attack passes through actors in its line or arc, until ordinary solid terrain stops it |
| Mirror Carapace | A visible chance to reflect a direct hit back at its source; reflected hits cannot reflect again |
| Iron Domino | Buckler shove pushes two tiles, making wall crushes easier but riskier near friends |

Three-choice rewards need not always have one of each category. Mix an
immediate rescue with a long-term build choice and a weapon upgrade. Example
drafts to test:

1. **After a rough first floor:** heal now; take a Double Swing stick; take
   Moss Stitch for healing after future floors.
2. **Before a den floor:** +2 max HP; add Drowsy to the current bow; take a
   buckler and its block utility.
3. **After finding a musket:** add a faster first reload to that musket; take
   Bone Counter for ammo; take Lantern Bell to help the party in caves.

Other power-up candidates: increase max HP, heal now, shorten step interval,
gain one quick slot (cap it), improve a held weapon's magazine, or cleanse a
status and gain temporary protection. A heal-now pick should be appealing when
the party barely survived, even though an artifact lasts longer. Reject an
offer whose required item or condition the player cannot reasonably use.
All Piercing and Mirror Carapace are rare rule-breakers; their effects should
be obvious when they trigger, and their chances and collision order must live
in deterministic gameplay state.

## Enemies, hazards, and floor pacing

| Floor | Candidate content |
| --- | --- |
| 1 | Passive bunnies/chickens, slow zombie, occasional bat, simple wolf, tiny den branch |
| 2 | More wolf packs, bears as slow heavy blockers, brambles, spikes, larger den |
| 3 | Special layout: trick room with a telegraphed crusher wall, rolling boar/beetle, spike floor and wall, alternate safe path |
| 4 | Harder forest exit: guarded den, mixed wolf/bat/bear pressure, stacked zombie that drops live zombies when its upper bodies die |

Give every dangerous mechanism a visible or audible tell. The crusher wall
should advance along a known lane with one-tile side niches; spikes should
show an inactive phase before firing. The rolling enemy should commit to a
line so a player can sidestep or lure it into a barrier. A stacked zombie can
occupy one grid cell while drawing two or three 16×16 sprites vertically; its
upper bodies fall as ordinary zombies when killed. These enemies test tile
blocking and item utility without requiring continuous physics.

## First shop price sketch

Use these only to test the economy. If a reasonably explored floor yields
roughly 12–18 coins, a shop after floor two leaves about 24–36 coins. The
player can buy several supplies or one strong build item.

| Stock | Test price |
| --- | ---: |
| Herb bundle or small heal | 5–7 |
| Held-weapon ammo | 6 |
| All-carried-weapon ammo | 11 |
| Stick, knife, or bear trap | 8–11 |
| Bow or buckler | 15–18 |
| Musket, pistol, or named modifier | 23–28 |
| Rare artifact | 30–36 |

Keep common healing and ammo in stock so the shop is useful even when its rare
item does not suit a build. Show the next shop before players spend or discard
loot. The free three-choice reward still happens on every cleared continuing
floor; a shop is an occasional extra stop.

## Art and production note

Rust Teeming draws 16-pixel tiles, and most of its tile, actor, and item PNGs
are 16×16. Chick is 8×8; many blood/footprint particles are 4×4; one train
detail is 32×32. For new content, rough Python-generated 16×16 pixel sprites
are acceptable working assets. Make silhouettes and colors readable first,
then polish only the ideas that survive playtesting. No new art is needed to
decide this plan.

## Reference lens

- [Vagante developer notes](https://vagantegame.tumblr.com/) describe
  once-per-run artifact attributes and continued work on room-generation
  content. Borrow the distinct-find goal, not its whole equipment/stat sheet.
- [SYNTHETIK's weapon notes](https://www.synthetikgame.com/weapons) and
  [item notes](https://www.synthetikgame.com/items) show the value of different
  handling, variants, and weapon-item synergies. Teeming can keep its own
  simpler ammo and movement rules.
- [Slay the Spire's official page](https://store.steampowered.com/app/646570/Slay_the_Spire/)
  emphasizes path choices and relic interactions; the Teeming draft should
  produce that kind of build decision with physical items and co-op effects.
- [Caves of Qud's developer roadmap](https://cavesofqud.com/roadmap/)
  describes mixing authored places, procedural detail, fauna, and item
  modifications. Use that compositional spirit for Teeming's faster tile-step
  floors, at whatever size gives its rooms enough room to breathe.

# Gauche content farm brief

This records the requested direction for a large content-design pass. It is a
brief for ideation and iteration, not a claim that these enemies, items, rooms,
or sounds already exist. The current generator and content set are a prototype.

## Inspirations and shape

- Use a Zelda / Binding of Isaac-like **route graph** for choices, branches,
  objectives, and locks. Make the spaces within that graph more varied in size,
  outline, and tactical complexity, taking Barony's involved rooms as another
  reference. A graph node need not be a plain rectangular box.
- Mix hand-authored rooms, composed room pieces, and generated connections.
  Entire floors may be premade when an encounter or story idea benefits from
  it. Include rare **unique floors** with their own rules and layout.
- Keep the fast rectilinear combat, expressive items, terrain changes, and
  co-op escape objective. Fantasy objects and firearms can coexist freely.
  The tone may be strange, funny, or deliberately lopsided if the interaction
  is memorable.

## Biomes and content targets

| Biome | Current direction |
| --- | --- |
| Forest | Woods, grassy caves, animals, camps, canopy light, old structures. |
| Ice | Cold terrain and encounters with their own hazards and room language. |
| Industrial / mine / lava | The Underworks: still-working mines and foundries; excavation crews, heat, pressure and material transport. See [catalog](content/INDUSTRIAL.md). |
| Fourth biome | Military / robot area is a candidate, **not decided**. |

Desired progression is **Forest → Ice → Industrial/mine/lava → fourth biome**.
The runtime now follows Forest → Ice → Industrial for its three implemented
zones. The fourth zone remains to be built and added to progression. Preserve
cross-biome utility: forest fire tools burn wooden obstacles and remain valuable
in Ice; their usefulness drops in the hot industrial zone, where carried ice
tools gain new uses. Inventory choices should anticipate the next area's
materials and hazards, rather than simply replace old items with higher numbers.
Give these carryover tools several concrete interactions in the next biome.

Minimum content targets for **each** biome, to design and then implement:

- **20 genuinely distinct enemy types.** Unique means different behavior,
  encounter role, attack tell, silhouette, and sound, not a recolor or HP tier.
  Every enemy needs a coherent drop table, including an explicit chance of
  dropping nothing. Passive wildlife counts only when its behavior matters.
- **20 distinct loose-debris types or material families.** Examples may come
  from broken props, walls, vegetation, machinery, enemies, and environmental
  hazards. Pieces are locally simulated cosmetics while they cannot affect
  movement, damage, AI, loot, or other gameplay rules.
- **50 distinct biome-specific items.** Give them concrete use patterns,
  tradeoffs, sounds, and interactions; keep generic essentials as a small
  shared pool. Some finds can be weak, awkward, or silly and still be valuable
  because they do something specific. Avoid fifty stat or color reskins.
- A substantial pool of **room types, tile types, props, hazards, containers,
  and landmarks**. Each should change a decision, reveal a place's identity,
  or help the world feel inhabited. Preserve visual negative space instead of
  filling every tile with texture noise.

With four biomes, that means at least 80 enemy types, 80 debris types, and
200 biome-specific items before a small shared pool. The fourth biome's theme
remains open even though its eventual content target is the same.

Items can be weird in grounded ways: a rock is thrown, lands at the target's
feet, and can be recovered; a lighter only becomes potent around oil, leaking
sap, or another flammable substance. These illustrate the intended range;
items need not all be powerful. The item, prop, tile, and status rules should
combine naturally without hand-coding every pair.

## Rooms, locks, and solvability

- Build the floor's route and dependencies before terrain: spawn, ordinary
  route to exit, optional branches, keys and levers, gates, secrets, and any
  locked-in encounters. The final layout may have loops, varying room sizes,
  irregular outlines, and larger multi-part rooms.
- A key can open a door; a lever can alter passages; gates can appear or a
  door can close after the party enters a room. A lock-in fight needs a clear
  completion condition, readable warning, and a deterministic release. Handle
  split parties, disconnects, deaths, and rejoining without trapping a run.
- Validate that required keys/switches are reachable before their locks and
  that every generated floor is completable with ordinary available tools.
  Digging, bombs, and the train can create optional shortcuts. Authored unique
  floors get the same solvability review as generated ones.
- Give each room a role, entrance/exit shape, and local placements for enemies,
  props, light, loot, and containers. Use room-specific and floor-wide budgets
  so encounters and supplies form a pace rather than random scatter. Include
  quiet rooms, risky detours, guarded rewards, and occasional set pieces.
- Build multiple types of chests, boxes, and other containers with distinct,
  sensible drop tables. Enemy and container drops should follow the source:
  a den, workshop, military cache, and forest animal should not draw from one
  undifferentiated global table. Save all gameplay loot rolls in run state.

## Rewards and economy: still open

- Occasional between-floor shops sound promising. Put some gold or other
  currency into the world through pickups, containers, enemy drops, or room
  rewards so finding and spending it is a real choice. Shop stock and price
  rules should fit the biome and run stage.
- Perks, post-floor three-choice rewards, and artifacts need playtesting.
  Artifacts could become central build-defining finds, or be cut if the items
  and room interactions already make the game fun enough. Do not finalize a
  large perk/stat system before the core content has been played.

## Sound and impact

- Make actual new sound assets for new enemies, weapons, props, tiles, drops,
  doors, hazards, and room events. The inherited Gauche sounds are a starting
  set, not the quality or coverage target. Important repeated actions should
  have variations so a busy room does not sound identical every second.
- Generate sounds **offline** when useful: Python synthesis may use LFSR-like
  noise, oscillators, envelopes, filters, layering, and analysis of waveforms
  or histograms. Other recording, editing, or synthesis methods are welcome.
  Commit finished audio files; ordinary game playback uses those assets and
  positional/directional mixing where appropriate.
- Give guns their physical aftermath. Shots can eject casings, shed small
  fragments on impact, and kick nearby loose debris. Reloading and an empty
  magazine need their own clear sound and visual feedback; a dry trigger pull
  should sound different from a fired shot. These scraps stay cosmetic unless
  a particular item explicitly makes them gameplay objects.

## Example unique floor: haunted house

A largely open level has trees, grass, perhaps water, and a mansion-like
fortress in its center. Reaching and flipping a switch inside closes the way
out and starts a survival-wave encounter inspired by classic Call of Duty
Zombies. The party must survive or complete a defined sequence before an exit
opens again. The exterior, approach, building, switch, lockdown, waves, loot,
and release can be mostly authored. Work out co-op entry and lock rules so a
teammate outside or a reconnecting player cannot strand the party.

## Next design pass

Use this brief to generate large candidate catalogs biome by biome, then
iterate on what is fun and thematically coherent. Each enemy entry should name
its behavior, tell, sound, context, and drop table. Each item should specify
its pattern, use/cooldown or ammo state, failure case, synergy, sound, and
rarity. Each room should describe its route role, geometry, locks, placements,
loot sources, and completion condition. Each prop/debris family should say
what breaks it, what remains, and how the fragments move.

The existing [forest content sketch](FOREST_CONTENT_IDEAS.md) is a smaller
candidate pool for the first floor. The [master task list](MASTER_TASKS.md)
tracks all open work; this brief captures the larger content ambition and
unresolved choices.

## Ambient scenes (2026-09-14 addition)

Add at least 20 ambient sound sources/cues per biome, separate from combat SFX.
Support positional persistent loops with volume/falloff, one-shot or rearmed
area triggers, and occasional level-wide events with chance and cooldown.
Environmental vignettes combine sources and visible context: a stream falling
from a wall into shallow, steppable water, puddle rings from feet, wind through
a gap, creaking ruins and distant screams. These are room/prefab scene contents,
not full actors. Playback, ripples and local trigger history are cosmetic and
not network-synchronized; any gameplay terrain/material rules still are.

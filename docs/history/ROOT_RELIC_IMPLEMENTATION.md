# Root relic implementation

## Root drills and swap seeds

Root drills and swap seeds bring forest finds to 33. Drills have three uses,
a 90-tick cooldown and a committed five-cell line. After a 12-tick start they
advance or bite every six ticks, with a six-second maximum life. Each bite does
24 actor damage or twice that to terrain with dig power 3. They pause to chew
wall HP, pierce bodies and armor, and stop at unbreakable/too-hard terrain,
closed doors, encounter gates and crusher machinery. Damage/reach modifiers
remain in the projectile's copied item; changing the owner's held tool cannot
change a shot already traveling. The debug preview uses the same stop rules.

Swap seeds stack to three and consume one on launch. They travel five cells at
four ticks per cell, then exchange current positions with the first movable
actor hit. Both destinations must remain walkable and free of other blockers.
The swap commits both cells before landing effects, so neither actor overlaps
the other's old position during hazard resolution. Friends can be swapped.
Failed impacts fizzle; a dead/stale owner cancels the seed. Players' pending
weapon actions cancel and both bodies receive a movement beat of recovery.
Local swap bursts suppress ordinary arrival footprints and reset the camera
guide, including short swaps. Body rendering still uses authoritative cells.

Three sprites and six new synthesized cues have offline Python sources. Rare
room, reward and shop pools include both tools. Projectile fields already enter
hashes and snapshots; gameplay compatibility is CF, snapshot layout still 20.
Strict builds and static world/item-card captures pass. No live playtest or new
gameplay test suite was run.

# Working mine crews

Industrial generated floors now reserve one workfront: a broad dry court with
a short rock spur beside the main route. A Shift Foreman directs three Pickhands
in separate lanes. A real pickaxe and a crate sit behind them. The four actors
cost five points from the encounter budget before incidental Ember encounters;
they do not appear as extra free enemies after the budget is spent. Their room
cannot be overwritten by the optional secret-room roll.

## Work and pursuit

The 110-HP foreman chooses a cardinal work direction and whistles for 36 ticks.
Living linked workers within eight audible cells receive the order. He follows
their advancing work front without requiring every follower to occupy a reserved
slot. Orders expire after six seconds and are surveyed again. Invalid/dead leader
handles leave workers wandering; a newly spawned entity in that slot cannot
inherit the crew. Work orders pause while the foreman sleeps, is stunned or fights.

Each 80-HP pickhand aims for its own lane four cells ahead of the leader, taking
ten-tick steps. Blocked workers choose among open neighbors instead of repeatedly
trying one blocked direction. No formation cell is an invisible obstacle.

A bounded, stable-order cost search weighs walking against cutting. Walking uses
the movement beat (including chill/grip slowing); each 25-damage cut costs a full
36-tick tell and 12-tick recovery. Picks have dig power 1. Ordinary walls and
blocking breakable props can be removed through shared terrain/prop damage.
Lava, deep water, structural boundaries and stronger dig requirements block the
route. Searches expand at most 512 cells per available movement beat, returning
a useful closer frontier when the target lies beyond that budget. This is a
local bounded search, not guaranteed globally optimal pathfinding.

Industrial exit enclosures now need dig power 2. Crew work cannot accidentally
open the objective's boundary; player pickaxes still can. The outer map wall is
unchanged. All cutting changes real tiles; rubble and sound playback stay local.

## Retaliation and readable attacks

A weak hit makes a pickhand complain and interrupts its work. A hit of at least
12 damage, or 20 accumulated damage in a two-second window, provokes that
worker for ten seconds. Damage to the foreman rallies the
whole surviving crew. Killing two linked pickhands also rallies the survivors;
repeated damage to a corpse cannot count another casualty. Hazard damage without
a real identifiable attacker interrupts work without blaming an arbitrary player.
Workers do not retaliate against members of their own crew.

An angry pickhand digs when cutting offers the cheaper path to its attacker.
The foreman pursues through ordinary paths and does not gain a pick. Both commit
to a single adjacent cell for a 30-tick windup: pickhand 18 damage, foreman 14.
Stepping away avoids it. Damage, sleep, stun, rooting or displacement cancels a
pending cut/strike/order. Attacks resolve through shared cover, damage and debug
pattern rules. No sliding sprites or damage from an interpolated position.

## Art, sound, drops and storage

Seven native 16px poses distinguish soot-brown workers with raised steel picks
from the taller cream-coated foreman and his whistle. Small helmet lights mark
working figures. Twelve generated OGG cues cover whistle, work cadence, heave,
two pick impacts, warning, swing, complaint, alarm, death and two bootsteps.
Whistles/impacts are also semantic noise for existing hearing/echo systems;
mute settings and local playback cannot change who hears them in gameplay.

Current single-roll drops: pickhand 20% pickaxe at 12 condition, next 20% 3–6
gold, otherwise nothing; foreman 50% 5–9 gold, otherwise nothing. Foreman's
whistle, survey chalk and lunch tin drops wait for those real items to exist.
Neither new actor type adds an implemented regional item to the quota.

Shared entity slots store phases, lane, work direction, damage/casualty counters
and generation-checked foreman/attacker handles; no per-species fields were added.
The source documents the slot meanings. Snapshot version 40 and gameplay version
`0x2026091521` cover the new entity kinds and floor population rules.

## Validation

Focused direct-function checks cover order receipt, weak-hit tolerance, local
anger, leader-hit and two-casualty rallies, duplicate death protection, walking
versus digging costs, shared wall damage, reinforced gates, displaced/stunned
windups, stale handles and dodging a committed strike. Sixteen generated
Industrial floors (four stages/four seeds) contain one complete crew and retain
reachable objectives with required locks, repeatable hashes and snapshot round
trips. Release game and static renderer builds passed. Static order/cutting
scenes were inspected; helmet lights were raised for readable silhouettes and
the worksite floor reduced to sparse marks. No autonomous playtesting or
permanent test suite.

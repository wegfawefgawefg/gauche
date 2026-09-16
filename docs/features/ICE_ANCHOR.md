# Ice Anchor

Implemented Ice item 48/50. Place a point at your feet on clear dry ground or ice;
release and hold Use again to reel toward it from up to five Manhattan cells away.
Return uses real cardinal cell steps, one every three ticks, without presentation
interpolation or teleporting. Physical sight and the short cell path must stay
clear. Walls, gates and bodies stop travel; the item never invokes crush damage
on obstruction. Protected encounter boundaries remain sealed.

Every entered cell runs ordinary movement contact, hazards and slipping. A spring,
portal, shove or slip that takes the actor away from the expected next cell ends
the return. Release, movement intent, stowing, death, sleep, stun, root and action
cancellation also stop it. Another press can begin a fresh return when valid;
holding Use after a stop does not repeatedly retry. No attacks or immunity are
bundled into a return.

The point has 30 HP, or 60 Durable. Each successfully traveled cell wears one HP;
melee hits, enemy attacks and blasts can damage the point as well. It sits below
actors and does not obstruct ordinary movement. Secondary recovers it from the
same or an adjacent clear cell, preserving all damage and wear. A destroyed point
loses its linked item; the last worn step completes before the spent item breaks.
If frozen support thaws into water, or another obstruction replaces the support,
the point is lost. Ground reels are removed when their point is lost, preventing
phantom pickups. Orphan points remove themselves.

Stowing and trading preserve the point link and condition. The actual reel must
be held to return. Carried anchors recover on floor transition with their latest
condition; stale old-floor handles do not survive. Item.anchor holds a generation
checked point handle, opened latches Use, and durability mirrors point health.
The fixture's entity_a is the reeling player, point_a the expected player cell,
counter_a remaining cells, timer_b the movement beat, and fixture_open this tick's
held-input permission. All are existing saved/hashed fields. Snapshot format is
38; gameplay compatibility is now 0x2026091517. Decoder checks bound the reel state.

Value 32. Ice masons gain a 10% drop from their previous empty pool, retaining
20% bricks, 15% chisels and 25% 2–4 gold. Cliff paths alternate anchor, crampons
and wool wrap supplies; Ice shops and rewards include it. Two sprites, six offline
steel/rope sounds, a local tether trace and inventory controls/condition diagram
complete the presentation. Setting or breaking a point reaches existing listeners.

Validation: strict game/render builds; focused direct checks for timing, active
snapshot/hash continuation, cover, body obstruction without crushing, hazards,
interruption, damage-preserving recovery, depletion, orphan and ground-item
cleanup, ice support loss, melee contact, damage-preserving floor transition,
malformed state and attributes. Static
world/inventory captures inspected with dummy SDL drivers. No autonomous playthrough
or permanent test suite.

Assets: tools/art/ice_anchor.py and tools/sound/ice_anchor.py.
Static render modes: anchor, anchor-blocked, anchor-items.

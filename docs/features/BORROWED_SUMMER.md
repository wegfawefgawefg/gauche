# Borrowed Summer

Implemented 2026-09-15. Ice regional item 41/50.

A corked jar lends four seconds of moving warmth. Its cardinal cross reaches
one tile, two with Big. It thaws native and temporary ice, clears snow, opens
frozen tins and releases nearby actors from chill regardless of allegiance.
It ignites spilled oil and uses the same warmth queried by pilgrims, leeches
and cold attacks. Solid cover blocks reach; heat left behind expires within
two ticks. It grants neither healing nor damage immunity.

Two activations, four with Durable; nonstackable. Activation while already warm
does not spend a use. The effect persists after changing or exhausting the jar.
The HUD shows remaining time and warns that it thaws everyone. Sparse gold motes
and uncork/fade sounds are local presentation. The uncork is audible to nearby
echo hounds through the shared hearing rules.

Available in Ice shops/rewards, shrine supply rolls, and 5% of candle cabinets.
Price 38. Art and both sounds have reproducible offline generators.

Duration and radius live in shared vital effects, including snapshot validation
and game hashes. Snapshot format is 37; gameplay compatibility is
`0x2026091510`. All multiplayer peers need the same rebuilt version.

Validation: strict game and static-render builds; direct-call checks for use
consumption, duration, Big/Durable, wall blocking, friendly/enemy thaw, oil
ignition, moving/expiring warmth and snapshot round trip/invalid-radius rejection.
Inspected dummy-driver HUD/inventory renders. No autonomous playthrough.

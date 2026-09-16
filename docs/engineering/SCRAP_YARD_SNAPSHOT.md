# Scrap-yard snapshot repair

A generated scrap yard could make an otherwise valid floor snapshot fail to
load. The intentionally worn Pickaxe set `durability=6`, while Pickaxes use
`uses` and have zero maximum shield durability. Snapshot validation correctly
rejected `6 > 0`; the item also incorrectly retained all 32 digging uses.

The room now sets six uses and leaves durability at zero. This fixes both the
intended reward condition and the rejected network state. It does not weaken
item validation or translate malformed snapshots into valid ones.

Snapshot decoding now reports the first invalid tile coordinate, entity slot
and kind, or run-state section. Previously an earlier validation failure could
surface as `Invalid projectile contacts`, hiding the source of the problem.

The freight-room integration check exposed the bug in Industrial floor 9,
seed 30 with the current generator. After the correction, all 128 sampled
Industry floors decoded and retained matching state hashes, in addition to
passing exit reachability and required-lock checks. These were temporary
focused checks, not interactive playtests or a permanent test suite.

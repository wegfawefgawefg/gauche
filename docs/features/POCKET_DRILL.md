# Pocket Drill

Implemented September 15, 2026. Thirteen of fifty Industrial items are now
integrated; enemy count remains ten. This is a native route-cutting/combat tool,
with existing diggable rock, timber, props and wire providing immediate uses.

## Action

Hold Use while remaining in one cell and facing one direction for 24 ticks
(0.4s). After priming, the drill applies an adjacent eight-damage contact every
12 ticks (0.2s). It has 30 battery beats, approximately six seconds of cutting.
Open air and reinforced material still spend battery. The last beat consumes
the item. Ammunition does not recharge it.

Moving or turning restarts the prime without blocking player movement. Holding
movement into an intact wall still permits drilling; once it opens and the
player steps through, the new position needs another startup. Releasing Use
cancels freely. Switching, dropping, interacting, reload/cancel input and real
health damage use the existing release latch. Sleep, stun and airborne toss also
cancel the action; it cannot fire immediately after an incapacitated windup.

The shared damage/dig rules own contact. Dig power 1 cuts ordinary stone and
other damageable terrain; stronger reinforcement and unbreakable boundaries
remain intact. Props use ordinary damage, including copper-wire severing. A
blocking prop absorbs its final beat: the drill does not hit through cover on
the same beat that breaks it. Sleds, anchors and traps use their existing contact
handlers. Real actors use normal blockable damage with a generation-safe
instigator; no separate drill-only defense rules.

Each running beat produces authoritative noise within the shared twelve-cell
acoustic flood. Walls/closed doors block that flood. It wakes sleepers and sends
curious enemies, including Gunners and Welders, to investigate. Cosmetic audio
does not decide AI state. Terrain impacts retain material debris and damage bars,
but the drill owns its motor audio instead of adding the repeated wall beep.
Existing terrain callers keep their default sounds.

## Item integration

One nonstackable drill costs 27 gold. The master supply table offers it from
Industrial stage two in weapon, reward, shop, cache, secret and workshop sources;
the existing rare-import rules remain in force. No second loot list was created.

Durable provides 60 battery beats; Fragile 15 with its normal damage increase.
Strong/Heavy/Agile use shared damage/cooldown adjustments. Long, Big and Piercing
are unsupported. The drill is a substantial steel object for magnet/crane rules.
Inventory details show its finite uses, damage, cooldown and startup.

Player slots: `label_b` 25–30 identifies the committed inventory slot;
`counter_a` is prime progress, `counter_b` the release latch; `ground_item` stores
the committed variant; `point_a` and `point_b` store position and facing. The
reader validates those bounds and the matching live item. Gameplay compatibility
advances to `0x2026091545`; snapshot 50 / wire protocol 14 remain unchanged.

## Assets and verification

Original transparent sprite `pocket_drill.png` was copied unchanged from
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-df16bec7-3cec-4698-9b8f-7bbfb6170532.png`.
The held tool vibrates slightly while active; player/terrain positions stay exact.
`tools/sound/pocket_drill.py` creates four original offline motor sounds:
startup, loaded cutting, free spin and exhausted battery. These have distinct
`pocket_drill_*` names; the older Root Drill's assets remain unchanged.

Strict release build passed. Temporary direct checks covered startup/cadence,
ordinary/reinforced/unbreakable rock, wood, cover absorption, wire cutting,
position/facing resets, release/exhaustion, hurt/control cancellation, switching,
hearing, no ammo refill, supported variants, native supply membership, snapshot
roundtrip and invalid prime rejection. In 2,000 deterministic native weapon
rolls at Industry stage two, 169 selected the drill; that is a fixture result,
not a universal loot-rate promise. SDL dummy loading checked the four sound
assets. Static held-tool capture: `/tmp/gauche-pocket-drill.png`.

No interactive playtest or permanent test suite was added. Sound balance and
combat feel remain for the user's playtesting feedback.

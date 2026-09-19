# Audit Clerk and pay office

Implemented September 15, 2026. Industry now has twelve catalog enemy behaviors
and fourteen regional items. The office adds an optional theft encounter using
loose gold, breakable metal cover, existing workers and the shared hearing rules.
It does not add a new feed or single-purpose remedy item. Music remains parked
after the bounded third batch; only Shop Arrives is approved.

## Money and behavior

The neutral clerk has 38 HP and a sixteen-tick walking beat. It looks for reachable
loose coin piles within nine cells, steps onto one, and stamps for thirty ticks.
Completion transfers up to twenty actual gold from that pile into its purse.
Generation-safe handles and committed cell checks prevent taking a replacement
pile or one that moved during the stamp. It never takes money from player wallets.

The clerk returns to an adjacent reachable stance at its linked cage, then takes
thirty ticks to deposit. The cage holds up to 60,000 gold; excess stays carried.
An absent, broken, full or unreachable cage leaves the purse on the wandering
clerk. Search is throttled and paths are bounded to 512 visits; return-route work
runs on movement beats. There is no money generation during either transfer.

Actual damage, or audible tampering with its cage, starts a thirty-six-tick bell
warning. Repeated hits do not restart that clock. The clerk raises a visible bell
and gasps before ringing, then flees for 180 ticks with a 300-tick alarm cooldown.
Cold, sleep, stun, airborne toss, rooting and displacement interrupt committed
stamp/deposit/alarm actions. Killing it before the bell also prevents the call.
It has no direct attack of its own.

The bell uses an eight-cell acoustic flood, respecting walls, blocking props and
closed doors. Existing Pickhands and Shift Foremen that hear it pursue an identified
living culprit; an unknown source sends unengaged workers to investigate the
alarm position. No new workers spawn. Other existing noise reactions still apply.
Remote cage hits currently have impact position rather than shooter attribution;
they can therefore trigger investigation without falsely naming a player.

Death returns carried gold through ordinary coin placement and makes one personal
loot roll: 25% for three to seven gold. Punch-card and inspector-stamp ranges are
still empty until those items exist. A pay cage has 60 HP, blocks walking and
broad heat, allows narrow shots, and accepts existing metal cutters. It releases
its saved balance once on break, then scatters existing washer/nail debris. Cage
contents do not reroll when opened. Generic coin placement still shares the
existing entity-pool capacity limit; this milestone does not redesign that path.

## Generation and state

Pay Office joins Repair Bay, Cooling Works, Cable Trench and Kiln Court as equal
alternatives within the two-thirds optional maintenance-room selection. It places
a clerk, six loose gold, a nearby Pickhand, one pay cage and two window grates.
The cage starts with eight to fourteen gold on a 60% generation roll; otherwise
empty. Entire footprints and three free entity slots are checked before mutation;
placement tries mirroring and preserves the room's central protected cross. Failed
placement does not spawn an unlinked fallback clerk. Budget: three threat, no gear.

Shared clerk slots: `label_a` phase; `counter_a` purse; `point_a` linked cage;
`point_b` committed origin; `entity_a` coin handle; `entity_b` culprit handle;
`timer_a` phase time; `timer_b` search throttle; `attack_wait` alarm cooldown.
The c slots remain shared hearing memory. Pay Cage uses its existing compact
prop's `growth_ticks` as a saved balance, not a decrementing timer. Snapshot
validation checks phase, purse, timers, capacity and cage variant/live HP.
Gameplay compatibility advances to `0x2026091548`; snapshot format stays 50.

## Assets and checks

Built-in image generation produced these transparent originals, copied unchanged
into `assets/graphics/`:

- `audit_clerk.png`: `exec-d9604ec6-e693-4ac9-9840-769124290f1a.png`
- `clerk_stamp.png`: `exec-3d0a2804-5576-455e-86ec-f8b40e565be2.png`
- `clerk_alarm.png`: `exec-edc7dfbe-850c-4dfc-b6ea-7188907f0940.png`
- `pay_cage.png`: `exec-f4f8d3ad-c798-4014-9680-c3b2770af074.png`

Original directory:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`.
Clerk prompt: minimalist top-down pixel mole accountant, grey snout, green visor,
charcoal waistcoat, tan ledger and rubber stamp, right-facing, two boots, large
flat shapes, muted palette, transparent background without floor/shadow/text.
Stamp edit raises only the stamp arm; alarm edit replaces that stamp with a small
brass handbell and opens the mouth, preserving body, ledger, scale and transparency.
Cage prompt: one-cell squat steel strongbox with thick bars, brass padlock, dark
empty interior and top roof plate, muted blue-grey/brass, transparent background.

Eight original effects are reproducible with `tools/sound/audit_clerk.py`: paper,
stamp, gasp, bell and clerk death, plus cage rattle, deposit and break. Durations
are 0.25–0.70s and peaks 0.14–0.32. Cosmetic sounds and fragments remain local.

Strict release build passed. Temporary direct checks covered conserved transfers,
capacity/overflow, stale handles, cold interruption, delayed audible alarms,
blocked sound, unknown-source investigation, no reinforcements, cage/death
recovery, snapshot roundtrip and malformed-state rejection. Sixty-four generated
Industry floors retained reachable routes and necessary exit locks; eight complete
pay offices appeared. SDL dummy loaded all four sprites and eight sounds; the
inspected static capture is `/tmp/teeming-pay-office.png`. No interactive playtest
or permanent test suite was added. Balance remains for the user's playtesting.

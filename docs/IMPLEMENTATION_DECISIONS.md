# Gauche implementation decisions

- Keep the old Rust commit/history intact. Do not transplant Rust history into
  the new C++ repository; link it as the source reference.
- Do not port dead placeholders merely to match enum names. Replace the old
  settings stubs with Gubsy settings; the new exit objective supplies a real
  win/transition rule in the first run slice.
- Preserve the old control semantics first (movement, use direction, mouse,
  inventory, pickup/drop, zoom). Gubsy remapping can expose those same actions.
- The first pass should use the original authored PNG/OGG assets. Load each
  PNG directly by its `Sprite` name. Keep asset metadata in code unless a
  concrete Gauche asset later needs richer data. New-content placeholder
  sprites may be generated with a small Python script at the source's usual
  16×16 scale.
- Pin the Gubsy dependency rather than relying on whichever of the two local
  Gubsy checkouts happens to be on disk. They currently differ.
- The Rust history remains in `gauche-rs`; the new C++ history is published as
  `gauche`. Both local checkouts track their matching GitHub repository. The
  `port-finish` and `direct-netcode-finish` tags mark the playable port and
  direct rollback milestones.


## September 14 feedback implementation notes

Feedback implementation: the puffball causes 75 ticks of sleep, now explicitly
shown. Status badges show countdowns/effects; burning shows current rate and
remaining damage. Blocked walking uses a latched wood/stone bonk with no debris;
movement turns before explicit aim. Fire wisps render between fixtures and actors
and originate higher. F1 hides/restores debug windows; F2 toggles the categorized
selector while visible. Combat has independent player/enemy world-grid gates,
both off by default; inventory patterns remain. Builds and static status/debug
captures checked. Controller handling and contact feel await user playtesting.

Stack/cooking implementation: ammo max stack is ten; partial pickup merges what
fits and leaves excess on the ground. Shops/rewards/crafting remain all-or-none.
Used tools cannot merge; stacks preserve the longer cooldown. Non-stackable gear
has no fake quantity/quantity meter, and detail cards label stackability alongside
uses/condition/ammo. Cooking processes one portion per 45 ticks with a new sizzle,
without consuming raw meat if the result cannot fit. Static inventory capture
and strict builds checked; user owns interaction/balance playtesting.

## Forest water scenes

Brook rooms now contain walkable shallow channels and a small northern-wall
spring/pool scene. Deep water remains impassable. Standing or stepping in
shallows clears weak/strong burning before its next damage tick. Grounded
actors splash; flying creatures do not. Local expanding rings inherit light
and clip at dry banks. The ambience placement recognizes both depths and stops
a water source if its tile changes. Snapshot terrain IDs and gameplay version
were updated. Strict builds and a static spring capture checked; user owns
interaction and balance playtesting. Poison/residue washing remains pending.

## Chicken follow chains and food cues

Families spawn in a chain. A shared trail helper follows departed cells after
a five-tick delay; dead leaders are replaced by a free preceding chain tail
without cycles. Chicks use nine-tick steps, five while scared, with a faster
body wobble. Nearby adults defend the family with pecks; chicks retain eight
seconds of alarm and renew it while the attacker remains close. Cooking and
eating now have separate sizzle and munch cues. Strict build checked; follower
spacing, protective aggression and sound feel await user playtesting.

## Bow and fused bomb projectiles

Splonks' bow arms on press and fires on release. Gauche follows that interaction:
20-arrow quiver, no reload command, a drawn sprite/twang, 16-tick recovery. Each
arrow travels one tile per three ticks, damages on contact and splinters into
local wood/feather debris. Bombs throw up to three tiles, land against obstacles,
and detonate after a 150-tick fuse measured from use; visible sparks, a small
light and separate throw/landing/fuse sounds communicate it. Gameplay projectile
state uses documented shared entity slots and snapshot serialization. An explicit
input cancel bit handles inventory/menu/stun/death; missing network inputs retain
a bow draw instead of inventing release. Wire version increased. Strict builds
and static projectile/inspection captures checked, without playtesting. General
bullet tracers, rocket travel and persistent spells remain to implement.

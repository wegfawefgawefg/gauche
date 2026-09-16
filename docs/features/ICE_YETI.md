# Territorial yeti and actor throws — 2026-09-15

Ice gains a twenty-first enemy, beyond its initial twenty-enemy baseline. This
answers the request for some simple recognizable creatures among the stranger
observatory machinery. It does not finish the Ice threat-composition review.

## Encounter and counterplay

A yeti sometimes replaces the avalanche ram in a Cliff Path, at the same two
threat-budget cost. It has 112 HP and shuffles within two cells of its territory,
pausing between steps. It does not chase retreating players. A displaced yeti
tries to return; blocked shuffles consider all four directions.

An adjacent player or decoy provokes a 36-tick grab windup. The destination is
committed at the start; moving sideways avoids it. A rumbling vocal cue and
progressive body lean/stretch announce the grab. Debug attack grids show only
the committed adjacent cell. After hit or miss, recovery lasts 72 ticks.

The grab deals two damage, then throws the victim four real cells over 24 ticks.
Facing shields block it. Active grip, rooted feet and a sled seat resist the
throw. Hits dealing at least 12 actual damage interrupt the yeti's windup; small
Fist hits do not permanently lock it down. Sleep, stun and displacement cancel
its grab. A nearby straw decoy can waste its attack.

Walls, blocked props and bodies stop a throw: the thrown actor takes 12 impact
damage and up to 30 ticks of stun, respecting existing stun resistance. The
obstacle actor does not take collision damage in this first version. Normal
landings cause no extra damage. Ice/oil can add the existing one-cell slip;
Crampons prevent ice slipping, while Sticky Boots resist being thrown at all.
Cliff side shelves have ice, outside reserved dry progression paths.

Throws cross deep water, but **landing in it is fatal**, matching a bridge
breaking over deep water. They do not add swimming or the proposed frozen-cube
rescue state. Cliff Path does not itself acquire new deep-water pits in this
change. Flying species are not throwable by this mechanism.

A killed yeti drops two Raw Meat and independently has a 25% Crampons drop.
It has six new offline-generated cues: warning, throw, missed sweep, death,
soft landing and harder collision. Human playtesting still owns tuning.

## Shared movement state

`ActorToss` is a small shared plain struct in Entity; species a/b/c slots remain
available for their AI. It carries origin, cardinal direction, source, a
generation-safe instigator handle and remaining ticks. One bounded actor scan
advances throws before normal actors. Victims cannot act while airborne.

Every horizontal step changes the authoritative cell. Rendering raises the
body and held item only; the contact shadow stays on the real occupied cell.
There is no horizontal body interpolation or delayed collision position.
Airborne victims skip footprints, debris kicks, pressure traps, currents,
conveyors and ground contact effects. Existing burning continues in flight.
Landing restores surface contact. Another displacement cancels the old flight
at the new location instead of snapping the victim back. Death clears it.

All throw fields enter hashes and snapshot serialization/validation. Snapshot
layout is 50; gameplay version is `0x2026091536`; wire remains 14. All co-op
participants need the same build.

## Validation and assets

Strict Release `gauche` build passed. A temporary direct-function check covered
missed grabs, grip/shield resistance, light/heavy-hit interruption, four-cell
flight, collision damage/stun, deep-water crossing/landing, ice landing,
external displacement, ground fire/trap suppression and snapshot/hash replay.
No permanent test suite or live playthrough was added.

SDL-dummy static captures of warning/airborne poses were inspected. The new
sprite reads separately from the existing skeleton player and the body/shadow
separation is visible. Sixty-four generated Ice floors remained reachable with mandatory locks;
nine yetis were placed across those seeds. This is not a difficulty assessment.

The built-in imagegen tool produced `assets/graphics/yeti.png`; generated alpha
is preserved. It uses the existing nearest-scaled sprite path. Original prompt:

> Use case: stylized-concept. Asset type: a single game sprite for Gauche, minimalist tiny pixel art top-down roguelike. Generate one solitary squat yeti, white shaggy fur, broad long arms, small dark slate-blue face with two tiny pale eyes, large mitten hands and short dark feet, hunched gorilla-like standing pose, seen from slightly above/front so the top of shoulders is visible. Body centered, fills about 85% canvas width and height. Extremely simple chunky 16x16 logical pixel grid aesthetic scaled up crisply, only 4 flat colours (ivory white, muted ice grey, dark slate blue, near-black eyes), lots of negative space between arms and torso; no outlines, no texture, no gradients, no antialias, no shadows, no floor, no props, no text, no border. Actual fully transparent background with alpha, not a checkerboard. One sprite only, not a sheet.

Sound source: `tools/sound/yeti.py`, using existing offline synthesis helpers.

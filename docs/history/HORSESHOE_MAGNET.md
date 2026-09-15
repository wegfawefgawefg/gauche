# Horseshoe Magnet — 2026-09-15

Industrial item 17 is implemented. Hold Use to pull the nearest loose steel
item in the aimed cardinal lane one cell toward you every 12 ticks (0.2s),
from up to five cells away. You can move and retarget between beats. It takes
75 successful pulls to exhaust the magnet; empty/blocked attempts do not spend
wear or play repeated failure sounds. Long reaches nine cells; Durable permits
150 pulls. Unsupported damage/area/piercing modifiers are rejected.

## Physical interaction

The same GroundItem entity moves. Its count, attribute, ammo, condition, heat,
uses and generation stay intact. No inventory extraction, new loot roll or
merge occurs. It stops at your feet for ordinary pickup. Walls, closed gates,
blocking props (including grates) and blocking actors stop the ray. Another
loose item jams the landing cell; the magnet does not skip it to grab a farther
piece. It crosses water and hot ground under existing loose-item movement rules.

`magnetic_item` is one shared item property query, not another spawn table.
Substantial iron/steel tools and guns respond; wood, stone, food, copper wire,
brass bells and currency do not. The ammo supply is treated as its steel ammo
box. Cosmetic scrap stays cosmetic. Normal item detail rows label responsive
items MAGNETIC; rows occupied by item-specific action hints retain those hints.

Floating cargo deflates before moving. Strapped sled cargo and items with an
active flight handle are excluded. No target handle persists between beats;
another player, hook or conveyor sees the new real position, without duplicating
or stealing an invisible reservation. Selecting the magnet cancels old committed
bow/melee actions. Release and other interaction buttons stop pulling.

## Acquisition and presentation

The master supply registry has one new Industrial stage-one row: weight three,
price 18, ordinary reward/shop/cache/secret/workshop sources. Scrap bins now use
their reserved 0–19 roll for a magnet; Copper Wire remains 40–59. Other planned
drops are still absent. One-quarter of hot salvage pockets whose ordinary
reward is magnetic offer a magnet on the bank. Rewards still come from the
master weapon table. Coins remain on the plinth as a reason to actually cross.
No foreign Fishing Line is injected into Industrial.

New sounds are short metallic tension/tremolo and spent ferrite crumble,
generated offline by `tools/sound/magnet.py`. Pull sounds participate in ordinary
hearing. A brief grey displacement trail and contracting ring mark the actual
item movement. This is local presentation, with no damage or extra light.
The sprite uses the ordinary held-item and inventory renderers.

Crane-head attraction is still pending with the planned Magnet Crane enemy.
This milestone does not claim that enemy or its scrap-yard room is implemented.
Industrial now has seven catalog enemies and ten of fifty regional items.

## State and validation

Only existing item uses/cooldown and cargo positions change in deterministic
state. The new magnetic flag on transient ShotEvent is cosmetic and is not
serialized or hashed. Gameplay version `0x2026091539`; snapshot layout 50 and
wire 14 are unchanged. Matching game builds are required for co-op.

Reload/secondary-action dispatch moved unchanged from `item.cpp` to
`items/reload.cpp`, keeping the growing use dispatcher below 500 lines.

Strict Release build passed. Temporary direct-function checks covered exact
cargo state, cadence, moving/retargeting/release, range and modifiers, blockers,
nonferrous rejection, float takeover, committed-action cancellation, exhaustion,
native supply membership and snapshot/hash continuation. An ordinary pistol
reload checked the extracted dispatch. 128 authored salvage placements yielded
32 bank magnets and retained their real weapon rewards. Static SDL-dummy renders
were inspected; the first icon was too small, so it was enlarged through an
imagegen edit and the final render inspected. No live game or permanent tests.

## Sprite provenance

Built-in imagegen generated the first sprite with this prompt:

> A single tiny inventory sprite for a minimalist top-down pixel-art roguelike: a chunky U-shaped HORSESHOE MAGNET. Muted brick red curved iron body with two pale grey flat pole tips pointing RIGHT, one above the other; central notch is transparent. Four flat opaque colours only (brick red, dark red, charcoal, pale grey), hard stair-step edges, logical 16x16 pixels. Clearly readable simple silhouette. Centered square canvas, 10 percent transparent padding. TRUE transparent background. No floor, shadow, glow, sparks, lettering, gradient or surface texture. One sprite only, no extra objects.

The inspected initial source was
`exec-95d55ae5-f3cf-4aea-b85f-46456b3df8f5.png`. The final source,
`exec-a5440ad2-b1a8-4ca4-8a1c-5833615a0e03.png`, was copied with alpha intact
to `assets/graphics/horseshoe_magnet.png`. Edit prompt:

> Same single horseshoe magnet pixel-art game sprite, but MUCH LARGER on its canvas. Enlarge the existing shape to fill almost the entire square image: magnet left edge at 5 percent of canvas width, pole tips right edge at 95 percent, top edge at 5 percent, bottom at 95 percent. Only a tiny transparent border. Preserve the U shape with both pole tips pointing right, brick-red iron, grey tips and hard chunky stair-step edges. Keep the central U notch truly transparent, all outside pixels transparent. Flat colours, no texture or gradients. Do not add anything. This is a tiny in-game sprite that must be legible at 16 by 16 pixels; large silhouette is essential.

# Slag Snail and slag banks — 2026-09-15

Industry enemy fourteen: a 96-HP basalt-shelled crawler. Ordinary movement has a
40-tick beat. While crawling or tucked, blockable damage is divided by three
(minimum one); unblocked hazards bypass shell defense. A target within two cells
on a cardinal line starts a 48-tick tucked warning. The committed lunge advances
at most two cells, eight ticks apart, stopping at the first body/obstruction.
It deals 18 blockable damage or damages its blocking prop, then recovers exposed
for 72 ticks. Leaving the fixed lane avoids the hit. It does not retarget mid-tell.

Cold or water immediately opens the shell and cancels the attack. Exposure lasts
120 ticks, refreshed while chilled/wet. Recovery and lunge also take ordinary
full damage, so cooling is useful but not required. Twelve actual health damage,
control effects, root or displacement interrupt a commitment. Burn ticks, floor
fire and lava do not damage the snail. Other physical and electrical damage
retain their ordinary rules; there is no general heat-damage immunity.

Actual crawling/lunging leaves short slag patches on eligible dry floor. Each
uses existing surface storage: `LiquidKind::Slag`, 240-tick residue, 90-tick fire.
No water/oil/other liquid or live prop is overwritten, and no terrain becomes
lava. Shared fire rules provide ground danger and ignition of nearby fuel.
Cold removes the heat immediately, leaving safe crust; water washes it away.
A grounded non-snail footstep cracks cool crust into three cosmetic basalt chips.
Airborne actors do not disturb it. The crust cannot reignite itself without an
external fuel source. No extra whole-map pass or particle gameplay was added.

The optional slag-bank room puts a snail and Ore Bin beyond a short lava trench,
with a Coolant Can on the opposite dry bank. Both ends and the protected central
cross remain open. Whole-footprint/actor-slot checks precede all placement; a
mirrored placement is attempted, otherwise it declines without partial writes.
It is the seventh alternative in the existing maintenance-room selection, costs
two threat and one equipment slot. Incidental snails cost two threat. One drop
roll gives shared Cooked Meat at 30%; distinct sausage/paste items remain pending.

Slots: `label_a` phase, `timer_a` warning/recovery, `counter_a` lunge range,
`point_a` expected current cell, `point_b` committed direction. The c slots remain
available for hearing. Codec checks phase, clock and range. Gameplay version
`0x2026091552` separates peers; snapshot layout remains 50, with new enum values.

Assets use built-in imagegen, preserving originals and alpha. Saved sprites:
`assets/graphics/slag_snail.png`, `slag_snail_tucked.png`, `slag_snail_cooled.png`.
Original output identifiers are `f8fdf115-ba57-4ddd-9d67-db94399225c9`,
`d728dc26-4d8c-43cd-8ebe-d4f809f6ca2c`, and
`40ac437b-a3d9-4129-89ea-b8a22b0492ee` in the thread's generated-image directory.
Seven original offline sounds come from `tools/sound/slag_snail.py`: crawl, tuck,
lunge, impact, cool, crust crack and death, with peaks between 0.12 and 0.29.

Final image prompts:

> One isolated 16x16-style chunky pixel-art enemy sprite for minimalist top-down roguelike Gauche. A squat slag snail facing RIGHT: low rust-orange molten slug foot, two short blunt eye stalks to right, huge dark charcoal basalt spiral shell with ONE muted red-orange crack. Broad readable chunky silhouette, 6 muted colors maximum, sparse warm cream eye pixel, flat opaque pixel clusters, no gradients or noisy texture. Actual transparent alpha background, no scenery, no ground shadow, no text. Orthographic top-down/three-quarter RPG sprite, modest perspective to read shell. Occupy most of square frame with clear transparent margin. No cute face, no cartoon grin, no fire aura. Crisp nearest-neighbor pixel edges.

> Edit this game sprite into its attack-windup pose. Preserve the same basalt spiral shell, same right-facing composition, palette, pixel cluster style and transparent alpha background. Retract BOTH eye stalks and almost all orange slug body inside the shell, leaving just a small tense orange sliver under the lower rim. The shell crouches slightly lower. Keep the single orange shell crack, make it a little more apparent. One isolated sprite, no ground shadow, no text, no scenery. This is the clearly closed/tucked pose of exactly the same snail.

> Edit this exact snail sprite to show a cooled vulnerable pose. Preserve its right-facing layout, basalt spiral shell silhouette, pixel cluster style, size and transparent background. Replace all orange heat in shell crack and body with muted pale slate-blue and gray. Extend the soft pale blue-gray foot a little farther out to the right, lower the two eyestalks so it looks limp and exposed. Three short light-gray cracks on lower shell rim to indicate thermal shock. No snow cloud, no ice cube, no scenery, no aura, no ground shadow, no text. Crisp flat pixel clusters, restrained six colors, exactly one isolated enemy sprite.

Validation: strict release build; temporary direct checks of warning/contact
clock, dodge, shell/recovery damage, chill/wet cancellation, lava/burn immunity,
displacement/heavy-hit interruption, trail expiry/cracking/material preservation,
mid-warning snapshot replay and malformed range rejection. Protected-footprint
rejection changed no game state. Sixty-four generated Industry floors retained
reachable routes and required locks, with 26 snails and nine slag banks. SDL dummy
loaded three sprites/seven sounds; all poses and the bank were inspected in a
static render. No interactive playtest or permanent test suite added.

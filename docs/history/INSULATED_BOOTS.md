# Insulated Boots: personal floor-circuit protection

Implemented Industry regional item 26 of the initial 50. These six-use rubber
overshoes provide four seconds of protection from electricity conducted through
water or wire. Durable gives twelve activations. Steps take twice as long;
refitting during an active window refuses without spending. The final charge
still supplies a full four seconds, and the effect survives stowing or trading
the remaining item. No permanent equipment system was introduced.

## Shared electricity, explicit limits

The existing `ConductedShock` trace identifies each victim's distance from the
initial contact. Zero is direct electrode/arc contact and remains dangerous,
even on a wet tile. Later nodes enter through the floor: the boots prevent that
victim's damage. They do not remove circuit nodes, consume grounding, reduce
other victims' damage or protect a teammate downstream.

This applies to Cable Crawler pulses, Arc Welder sweeps, Arc Torch backwash,
Glass Eel water shocks, Eel Batteries and water/wire branches of Thunder Acorns.
Existing welder overlap handling keeps the minimum contact distance, so a
victim in its direct sweep cannot become immune merely because a second lane
also reaches them through water. Direct dry air arcs remain ordinary damage.
Grounding spikes still sink the complete pulse before personal insulation.

There is no fire/cold resistance, grip, shove resistance or oil/ice traction.
Rubber soles share the equipment slow-step tier with sticky boots/crampons;
chill can compound that tier. Water still puts fire out through the existing
surface rules, independently of this equipment.

## Acquisition and feedback

- Stage-one native Industry reward/shop/cache/workshop membership; ordinary
  rare cross-biome import rules still apply.
- Half of successful cable-trench supply rolls offer boots; half retain a
  Grounding Spike. The optional room's item/threat budgets are unchanged.
- Cable Crawlers have one death roll: 25% Copper Wire, next 15% boots, else empty.

The HUD displays `RUBBER SOLES`, a four-second countdown and the slow-step
tradeoff. Item detail previews show the current and fitted step interval.
The wool chill-resistance label is now `COLD WRAP` so it cannot be confused
with electrical insulation. Six new offline cues cover fitting, two quiet
rubber steps, rejected electrical contact, expiry and the last activation.
Ground footstep sounds and their hearing rules are preserved beneath the
rubber cue. Generator: `tools/sound/insulated_boots.py`.

The new saved `floor_insulation` vital timer is bounded to 240 ticks, serialized
and hashed; death clears it with other vital effects. No enemy counter slots
or client-local cosmetics determine protection. Gameplay compatibility is
`0x2026091567`.

## Art source

Built-in image generator, preserved original RGBA copied to
`assets/graphics/insulated_boots.png`. Prompt:

> Use case: stylized-concept. Asset: one tiny inventory sprite for top-down
> pixel-art roguelike Gauche, a pair of insulated electrician's rubber
> overshoes. Two squat chunky charcoal-blue rubber boots with very thick dull
> mustard-yellow soles and one muted yellow cuff stripe, staggered side-by-side
> angled toward lower right. Minimal logical 16x16 pixel design enlarged
> nearest-neighbor; about six flat muted colors, strong silhouette, large
> simple shapes, no noisy shading. Isolated centered boots filling square
> canvas with small margin, genuine transparent background, no text, no
> lightning icon, no scenery, no frame, no cast shadow. Rubber safety
> equipment rather than medieval armor. One asset only.

## Checks and remaining feedback

Strict release build and temporary direct checks covered actual use/wear and
refusal, downstream ally damage, direct contact vulnerability, water and wire,
real Arc Torch and crawler pulses, grounding, cold/fire limits, last-use and
Durable behavior, exact expiry/movement recovery and snapshot/hash replay.
An out-of-range timer is rejected. All 128 generated Industry floors retained
reachability, required locks and valid matching snapshots; seven had loose
boot supplies. Direct cable-trench construction produced both supply variants.
All six OGGs loaded under SDL dummy audio; original art and a static HUD/room
render were inspected. No interactive playtesting or permanent test suite.
Player feedback is still needed on the duration, movement cost and readability
of direct versus conducted electricity during fights.

# Glow Slag

Implemented 2026-09-15. Industry item 27/50: a reusable thrown light and heat
source with an actual lamp-room use, rather than another narrow cure/feed item.

## Rules and acquisition

- Use throws the actual lump up to six cells, four ticks per cell, for 4 damage.
  It lands recoverable through the existing rock flight/collision/parry path.
  Supported melee/throw modifiers adjust real damage/range/cooldown; no new
  instantaneous ranged hit path or copied replacement item.
- Starts with twenty seconds of heat. The same `loaded` counter decreases in
  inventory, ground storage, machinery cargo and flight. Stowing hides its light
  and exposed heat, but does not stop cooling. Individual lumps are non-stackable
  so their heat stays exact; the earlier cold-stack proposal remains deferred.
- Exposed light has radius five. Shared heat thaws adjacent ice and ground
  contact can ignite spilled fuel. No added burning damage is attached to the
  physical impact. Water and explicit cold quenching extinguish it; reheating
  while standing in water refuses.
- Secondary takes heat from directly ahead. Existing finite heat sources lose
  the amount transferred; lava fills it to twenty seconds. Full, missing and
  incompatible sources refuse without wear. There is no passive recharge from
  its own warm cell and no free heat from electric lamps.
- Furnace moths see the real loose/held light and can siphon its heat, making
  it a lure with a consequence. Heat Siphons can draw the same stored resource.
- Stage-one Industry stock/rewards/caches/workshops supply individual lumps.
  Lamp alcoves choose evenly between a water-filled Steam Lance, two Emergency
  Foam cans, or Glow Slag beside the existing stove, water and moth pair. Supply
  and threat budgets are unchanged; ordinary rare cross-zone import rules apply.

The HUD shows seconds of glow; inspection exposes the reheat binding. The debug
loadout editor can save any heat amount from zero to twenty seconds. Snapshot
validation bounds heat and rejects spare ammo or multiple lumps in one stack.
The item clock already participates in the shared item hash/codec. Gameplay
compatibility is `0x2026091569`.

## Art and sound

Two original RGBA sprites were generated using the built-in image tool and
copied without changing their alpha to `assets/graphics/glow_slag.png` and
`slag_cold.png`. The cold version uses the original as its edit reference.

Hot prompt:

> Use case: stylized-concept. Asset: one tiny inventory/world sprite for
> top-down pixel-art roguelike Teeming. A single fist-sized irregular lump of
> dark charcoal foundry slag with a broad burnt-orange inner crack and three
> small dull amber incandescent facets. Squat chunky angular silhouette,
> simple logical 16x16 pixel design enlarged nearest-neighbor, five flat muted
> colors, no noisy texture. Isolated centered object on genuine transparent
> background, no scenery, no lettering, no halo, no shadow, no frame. One sprite
> only; fill square canvas with small margin.

Cold edit prompt:

> Use case: precise-object-edit. Produce the cooled version of this one game
> sprite. Keep the exact silhouette, scale, positioning, coarse pixel grid and
> transparent background. Change only the hot orange/yellow cracks and facets
> into muted cool ash-gray and dark brown cooled mineral. No luminous color,
> no glow. Same chunky foundry slag stone with broad cooled cracks; make it
> readable against black, no additional objects or effects. Genuine transparency.

Original source files end in `exec-e8d12f2e-ad1f-40cf-b0bc-92b274885759.png`
and `exec-17d84c74-fce7-40d0-bf2d-1deb97ca44c7.png` in the session image archive.
`tools/sound/glow_slag.py` produces four glassy throw/landing, furnace intake and
cooling cues using inharmonic resonances and filtered noise; peaks .14–.23.

## Verification and feedback still needed

Strict release build passed. Temporary direct checks covered delayed throw
damage/recovery with exact heat, snapshot replay during flight, stowed expiry,
finite stove transfer and lava recharge, wet/source/full refusal, ice thawing,
cold/water quench, actual moth search and heat theft, full entity-pool refusal,
invalid snapshot rejection and partial debug loadout normalization. All 128
seeded Industry floors retained reachable required gates and matching snapshot
roundtrips. Lamp-room construction produced all three supply alternatives.
All four sounds loaded under dummy audio; hot/cold sprites and a static room/HUD
render were inspected. No interactive playtest or permanent test suite.

Player feedback remains necessary on visibility, heat duration and how useful
the lure is in a real fight. Cold stacking is not implemented. Other Industry
catalog items, debris, ambient families and the larger game checklist remain open.

# Industrial hammers

Press Hammer and Rubber Mallet bring Industrial regional finds to five of fifty.
They share normal held melee, modifiers, shielding, displacement, terrain, wear,
inspection and snapshot paths. No separate equipment or crafting system.

- Press Hammer: 36-tick overhead windup, 32 damage, 48-tick recovery, 30 condition,
  price 25. Deals double damage to blocking props and dig-power-1 terrain damage.
  Reinforced and unbreakable boundaries retain their existing rules. Real health
  damage interrupts this weapon's windup without spending condition; blocked hits
  do not. Existing cancel, item-switch and incapacitation behavior still applies.
- Rubber Mallet: 9-tick windup, 3 damage, 18-tick recovery, 70 condition, price 9.
  Both tools shove one cell after an unblocked hit. Ordinary occupied destinations
  stop the shove; hard obstacles can crush. Ground contact and existing ice/hazard
  behavior apply at the actual new cell. The mallet's low damage can interrupt a
  worker without reaching its single-hit retaliation threshold; repeated hits can
  still provoke it. A lethal crush counts as a real casualty.
- Mallet boiler taps replace damage, reducing pressure by 40. Below 25 pressure,
  a pending steam tell ends in its ordinary 30-tick recovery without a blast.
  Fuel, health, existing leaks and attached valves are preserved. A sealed outlet
  does not release pressure. Even an empty/sealed vessel costs a swing's wear.
- Wear happens once per resolved swing, including misses, rather than per target.
  Durable/Fragile modify condition; Forest resin repairs these tools through its
  existing damaged-equipment selection. A spent handle cracks and disappears.
- Workfronts have a 25% hammer / 25% mallet / 50% no-tool supply roll. Both also
  enter the existing Industrial regional shop/reward pool. Future slag and folded
  Industrial cover are still catalog targets; current blocking props work now.

Native 16px sprites and five offline generated OGG cues cover hefty/hollow swings,
impacts and a small pressure release. Static `hammer-items` render mode compares
both tools. Snapshot format 45 / gameplay `0x2026091526` requires matching peers.

Validation: release build and static item-card inspection; temporary direct-call
checks cover delayed resolution, damage interruption, wear, shove/crush/shields,
boiler pressure and seals, dig-power boundaries, double cover damage, repair,
Industrial generation and snapshot/hash roundtrips. No autonomous playtest or new
permanent test suite. Combat feel, sound taste and balance await player feedback.

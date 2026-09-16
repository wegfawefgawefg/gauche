# Steam kettle

## Contents and heat

- Reusable, nonstackable tool; 22 gold and 0.75s base cooldown. Arrives empty.
  Use beside or while standing in fresh water to fill one charge. Brine, oil and
  other active liquids cannot become fresh water. A filled kettle pours instead
  of refilling. Natural pools and temporary water patches can supply it.
- A cold charge heats for 90 ticks (1.5s) near the shared warmth sources: flames,
  candles, stoves and Heat Capsule patches. Partial progress cools at one tick per
  tick away from heat. Steam Leech suppression follows the existing warmth query.
- Boiling water stays hot for 1,800 ticks (30s) away from warmth. Nearby heat
  refreshes that time; wading or a submerged loose kettle immediately cools it.
  Cooling preserves its water. Every carried slot and loose item ages normally.
- `loaded` records empty/cold/hot as 0/1/2. `spare` records heating progress or
  remaining hot ticks. This uses existing serialized, hashed integer item state.
  Temperature never uses `flame_ticks` or makes the kettle count as a fire source.

## Spray and presentation

- The normal spray covers the adjacent aimed cell and a three-cell row beyond it.
  Hot water deals 12 blockable damage to actors there, including teammates. Cold
  water deals no damage. Both leave five seconds of water and quench flames.
- Cover is sampled before the action changes anything. The same cell function
  draws the debug world pattern and resolves use. It cannot spray through walls
  or intervening blocking props. The description diagram shows the nominal shape.
- The charge is emptied before damage/reflection, so death loot cannot duplicate
  the water just used. An entirely blocked spray spends nothing. No projectile
  travel is claimed: this is a short, immediate splash, not a fired water bolt.
- Strong and Heavy change hot damage; Agile and Heavy change cooldown. Long adds
  one row. Big adds one row and a wider last row. Empty/cold patterns stay utility
  colored; the empty kettle only indicates its adjacent fill direction.
- Inventory/HUD state reads EMPTY, COLD WATER, HEAT percentage or HOT seconds.
  The small bar tracks heating progress or remaining heat. Detail text states
  the scald damage or dousing action. Three original 16x16 sprites distinguish
  empty, water-filled and hot kettles in inventory, held and dropped views.
- Six offline-generated original sounds cover filling, boiling, tipping, scalding,
  splashing and cooling. Quiet per-cell hisses produce local steam at the affected
  cells; cold splashes make local rings. Neither cosmetic changes simulation.

## Integration and checks

- Ice reward/shop pools and bathhouse equipment supply can produce kettles.
  Ice now has 29 implemented regional items; 18 enemies and 18 debris remain.
- Gameplay protocol FE rejects incompatible peers; snapshot layout remains 34.
  The existing codec fixture includes a hot ground kettle with 731 ticks left.
- Strict game/render/codec builds, existing codec round trip, static inventory and
  room captures, sprite/audio validation and diff checks. No live playtest or new
  simulation test suite. User playtesting owns timing, sound and balance feedback.
- Pressure valves, sealant, Boiler Porter and other planned Ice content remain open.

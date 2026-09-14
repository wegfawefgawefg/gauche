# Coal fuel and iron stoves

## Coal interactions

- Coal Lump: eight-stack item, three gold, 0.5s base cooldown. A live stove in
  the adjacent aimed cell receives up to 1,200 ticks (20s) fuel, capped at 7,200
  ticks (120s). A full stove spends no coal and does not cause a surprise throw.
- A cold stove accepts fuel but stays cold. A lighter, heat capsule or another
  ordinary ignition action is needed to light its fuel. Coal never auto-ignites
  just because it was placed in an inventory or thrown across a room.
- Without an adjacent stove, use throws one physical lump to six tiles at four
  ticks/cell for four damage. Existing recoverable projectile rules handle walls,
  shields, parries, actor hits and landing. Damage occurs on arrival, not at use.
- Strong/Heavy/Agile/Long retain their usual projectile damage/cooldown/reach
  effects. Fuel value stays twenty seconds. All Piercing retains its actor rule.
- Throwing copies one actual item and consumes one held count; landing restores
  that item instance. Coal fragments left by impact are cosmetic, not extra loot.
- Coal joins the Ice shop/reward utility pools and bathhouse supply budgets.

## Stoves and heat

- Stove is a compact blocking 40-HP iron prop. Generated stoves start lit with
  sixty seconds of fuel. Fuel burns down only while lit; cold stoves retain fuel.
- Water/brine and cold-flask quenching snuff it. Refill does not repair damage;
  an empty or broken stove emits no heat/light. Wicks still only refill candles.
- Heat uses the same prop-flame query as candles. Stoves thaw adjacent ice and
  lunch tins, warm pilgrims and give leeches something to feed on. The emitter
  is warm amber, radius four, strength 950; no new global ambient-light layer.
- Iron is not classified as dry plant fuel, so the stove cannot create an endless
  surface fire by burning itself. External oil can still burn around it.
- Bathhouse prop placement can choose stoves, preserving protected routes,
  blocker spacing and spawn clearance. Boiler galleries and Porter machinery
  remain separate, unfinished content.
- Cold/lit body sprites keep the same orientation. A glowing grate signals a lit
  stove. Nonlethal hits use a low iron clang; breaking uses a heavier collapse.
- Four original 16px sprites: coal, cold/lit stove and coal crumbs. Eight original
  offline sounds cover feed, throw, landing, ignition, snuff, burnout, hit and break.
  Burnout/impact scatter local coal crumbs; broken stoves also scatter metal.

## Saved state and validation

Stove growth_ticks stores remaining fuel and variant bit 0 stores lit state.
Existing snapshot fields suffice; decoder bounds include the 120s maximum and
valid flags. Gameplay protocol advances to FD; snapshot layout remains 34.

Strict game/render/codec builds, existing roundtrip with a part-fueled damaged
stove, static stove and coal-detail captures, and asset/audio/file-size checks.
No live playtesting or new test suite. Final fuel economy remains for user feedback.

Ice reaches twenty-eight items and eighteen debris materials; enemy and room
counts remain eighteen and thirteen. Pressure Valve, Sealant, Steam Kettle,
Boiler Porter and other outstanding master tasks are not marked complete.

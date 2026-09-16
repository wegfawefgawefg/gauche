# Recoverable candles and wick supplies

## Items and state

- Candle Stub places a nonblocking 6-HP candle in the adjacent dry buildable cell.
  An occupied cell or live prop prevents placement without spending the item.
  A fresh candle holds 4,800 simulation ticks (80 seconds). Durable doubles HP.
- Only unused full-fuel, undamaged candles of matching attributes stack (four).
  Used candles never merge, even after refilling; recovering one preserves its
  remaining fuel and HP. Inventory displays fuel, condition and stack policy.
- Ordinary pickup/swap and comparison recognize placed candles. Existing loose
  items get pickup priority. Full inventory with no droppable selection leaves
  the candle intact. Failed entity allocation also leaves it intact.
- Recovery snuffs the candle. Placement relights it if fuel remains; an empty
  stump can be placed and refilled, then lit with an ordinary ignition source.
- Wick Spool adds up to 1,800 ticks (30 seconds), capped at 4,800, to the adjacent
  candle. Four portions, eight with Durable. Full candles spend no charge.
  Refilling does not repair damage or ignite a cold wick. Electrical lights and
  living emitters do not accept wick fuel.
- Prices: candle five gold, spool seven. Both join Ice reward/shop utility pools.
  Snow effigies now have their planned 20% candle drop. Effigy Mask remains open.

## World interactions and presentation

- Memorial courts place sparse candles through the existing prop spacing and
  protected-route rules. They can be recovered, damaged or extinguished.
- Candle light uses the prop emitter: radius three, strength 650, warm amber.
  Wetness, exhausted fuel and broken state suppress the flame.
  Cold wicks retain their unused fuel.
- Lit candles feed the shared thermal system: thaw nearby ice and lunch tins,
  supply warmth to pilgrims, expose snow effigies and participate in leech heat
  suppression. Water and brine extinguish them; the candle is not its own fuel
  surface and cannot create a self-sustaining fire patch.
- Five original 16px sprites: cold/lit candle, spool, wax and charred wick.
  Six offline synthesized cues: ignition, snuff, burnout, wax break, linen winding
  and empty spool. Burnout leaves local scraps; broken candles scatter wax/wick.
- Fuel, HP and flags use existing saved Item/Prop fields. Snapshot layout remains
  34; gameplay protocol advances to FA. Cosmetics never affect saved state.

## Validation and remaining work

Strict game/render/codec builds and existing codec roundtrip with damaged,
part-used carried and placed candles. Static `candles` and `candle-items` captures;
asset dimensions and finite audio checks. No live playtest or new test suite.

This slice reached sixteen enemy behaviors, twenty-seven items and seventeen
debris materials. [Candle Keepers and chapels](ICE_KEEPERS.md) subsequently add
the seventeenth enemy and cabinets. Effigy Mask and other content remain open.

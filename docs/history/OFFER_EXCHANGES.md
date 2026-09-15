# Confirmed offers and full-pack exchanges — 2026-09-15

Shops, floor rewards and queued missed rewards now use an explicit local offer
flow. Choose an offer, review it and confirm. A full pack opens **NO ROOM — choose
the item to replace**, comparing the incoming offer with the selected carried
item. Selecting a replaceable slot opens an exchange confirmation showing the
incoming quantity, outgoing item/slot and shop cost/remaining gold. Cancel goes
back to slot selection, then to browsing; cancellation changes no gameplay state.

The final input requests one authoritative transaction. The host checks the offer
identifier, current stock/reward, owner, money, capacity, outgoing kind and drop
eligibility. A replacement first reserves a ground entity for the outgoing item;
failure leaves inventory, coins and stock alone. The exact outgoing item state
is retained and the complete incoming bundle takes the selected slot. Fists and
items with a projectile in flight cannot be replaced. Ordinary purchases that
already fit still use atomic stack merging. Successful acceptance emits a cue.

An offer identifier covers the run, floor, phase, player generation, pending
reward position and offer contents. Stale confirmations cannot consume the next
queued reward or a changed shop offer. Competing shoppers cannot both buy the
same stock. Legacy direct debug choices remain supported; replacement commands
require an identifier. No local dialog state is saved or networked.

## Controls and readability

- **South / Enter** confirms in offer menus; **East / Escape** cancels dialogs
  and inventory panels. The modal prompts show the physical face label from the
  chosen controller icon family, or keyboard keycaps. Attack input and trigger
  state do not confirm offers. Number keys focus cards before confirmation.
- Pack/Compare, Continue, Drop, Equip and slot selection have separate mouse
  regions. Clicking Pack no longer continues, and clicking Equip no longer drops.
- Shop/reward titles use the angled banner. Sold stock, insufficient funds and
  waiting-for-friends states remain visible. Replacement details show quantities
  and the actual slot affected.
- **West / pickup** also operates keys, levers, doors, exits, encounters and
  campfires. Keys at the feet have priority. Loose items retain priority over
  other nearby mechanisms. A locked/waiting mechanism consumes the press so it
  cannot fall through to dropping your held weapon. The HUD states the matching
  action, including TAKE KEY, PULL LEVER and NEED KEY.
- New defaults use **LT for reload**, RT for use, West for pickup/world interaction,
  South/East for menu confirmation/cancellation. The former B-reload/A-interact
  pair is migrated in the stock default profile when both old bindings are found;
  custom profiles remain custom. The control reference includes the menu buttons.
  Keyboard F remains an explicit interaction binding alongside contextual pickup.

Merchant/cart presentation, bespoke shop-arrival sound/music and richer arrival
animation remain open. This milestone does not complete the entire shop/artifact
valuation or shop presentation pass.

## Network and validation

Input packets now carry the replacement slot/kind and offer identifier. Wire
version **14**, gameplay version **0x2026091532**; snapshot layout stays **49**.
All peers must rebuild together. Canonical/correction batches carry at most six
frames: actual packet sizes 1068/1076 bytes, below the relay's 1200-byte limit.
Twenty correction chunks retain the existing 120-frame history coverage; larger
correction histories fall back to a snapshot.

Release game/static renderer builds pass. Temporary focused checks cover staged
confirmation/cancellation, exact replacement and bundle placement, failed funds,
stale/competing buyers, protected/in-flight items, exhausted entity storage,
queued and floor rewards, fixture pickup priority, mouse hit regions, raw modal
button handling, packet validation/size and snapshot/hash round trips. Existing
snapshot-codec and rollback checks pass. A delayed exchange also reconciles through
all twenty correction chunks delivered in reverse order without a fresh snapshot.
Static browse/replacement/confirmation
captures were inspected; no live playtesting or permanent test suite was added.

A related preexisting snapshot problem was found in Pickhand loot: worn pickaxes
were assigned durability despite having a uses counter. They now receive **12
uses**, and dropped-pickaxe snapshots pass validation.

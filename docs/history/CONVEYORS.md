# Conveyors, hand drives and assembly rooms

Industrial floors now include assembly rooms with opposing roller lanes, dry
stationary crossings, a hand-driven run, real coal cargo, a Belt Crank and Brake
Shoes. A two-point gunner allocation prefers a side grate post; safe ordinary
placement is the fallback. Its budget is reserved before incidental encounters.
The former blasting-alcove gunner reservation remains the fallback on a layout
without an assembly room. Protected routes, objective cells and dry crossings
are excluded from belt placement. Required progression does not depend on owning
a crank, keeping a shoe, or defeating machinery.

Each 48-HP conveyor section is an existing eight-byte tile prop, not an entity.
Its variant stores cardinal direction and whether its drive lacks power;
growth_ticks stores remaining brake time. Powered sections advance once per
twenty simulation ticks. Straight hand/brake runs are bounded to thirty-two
sections; bends, power-source changes and destroyed sections split a run. Native
arrows show direction, dim arrows and brass fittings distinguish manual sections,
and a shoe marks a braked section. Roller animation uses render-time selection
from the simulation tick, not extra saved animation state.

Transport preserves real tile positions. It takes a phase-start occupancy map,
reserves empty destinations, then rechecks them before applying each movement.
Queued objects wait behind occupied cells even if the front one moves that beat;
slot order cannot turn a queue into a cascading shove. Blocks stop motion without
crushing or deleting cargo. Entry effects still happen at the actual destination.
Normal motion cooldown and facing remain unchanged. Grounded actors, loose items
and heavy boiler vessels can ride. Flying enemies, gripped/rooted actors, trains,
other anchored fixtures, attached/returning items and sled-linked bodies/cargo
are excluded. Ground items have no actor health, so their eligibility checks the
actual item instead. Their identity, variant, condition and contents survive the
ride. Dedicated cart and sled transport are still pending their carrier rules.

Belt Crank: forty turns, twelve ticks between uses. Holding use advances the
adjacent unpowered run once per successful turn; an empty or blocked mechanism
still uses a turn, but a missing, powered or braked mechanism refuses. A break
physically disconnects sections beyond it. Durable has eighty turns. Brake Shoe:
four successful applications, twenty-four-tick cooldown. Jams a connected run
for 360 ticks. Already-jammed runs refuse. A hit of at least sixteen damage snaps
the brake across the connected run before ordinary prop damage resolves. Both
items enter the implemented Industrial shop/reward stock. Brake-on-cart behavior
awaits the freight/cart slice; trains deliberately ignore the item.

Conveyors are bare metal rather than ignition fuel. Fire on external oil still
works through surface rules. Thin projectiles and beams pass above the rollers
instead of damaging each floor section under their flight; melee and blasts can
sabotage them. Gunner cover scoring still requires an actually blocking prop, so
it does not mistake a conveyor for a protective grate.

Powered belts wake and drag client-local loose debris while retaining collision,
friction, rake/foot impulses and the existing bounded pool. Broken strips produce
new rubber scraps plus washers. Cosmetic hand-crank debris impulses are not yet
implemented. The new roller loop is a local area source with distance falloff;
it requires nearby visible live powered sections and fades when none remain.
Breaking its initially sampled section does not mute other live rollers nearby.
That ambience never generates gameplay hearing or changes simulation hashes.
Actual crank/carry/break sounds participate in the existing hearing rules.

Seven PNGs cover roller phases, manual/braked states, crank, shoe and rubber
scraps. Six mechanical effects and one six-second ambient loop are synthesized
offline. Static `belts` and `belt-items` modes show the lanes and inventory.

Release game and renderer builds pass. Temporary direct-call checks cover power
cadence, front/back slot order, wall stops, flying/grip exclusion, exact loose
cargo, boiler transport, manual drive and disconnection, brake expiry and impact,
item wear/refusal, projectile clearance, local debris/hash independence, and
ambient gating. Sixteen Industrial floor/seed pairs contain powered/manual belts
with required objectives reachable and locked; snapshots roundtrip. Rivet checks
also pass after the changed population reservation and floor-shot clearance.
No live playtest or permanent test suite was added; feel and room balance await
player feedback.

Snapshot 49 and gameplay `0x2026091530` require matching rebuilt peers. This
milestone brings Industrial to eight regional items and six native catalog
debris materials, with one of its twenty ambient asset types implemented. The
larger Industrial catalog, carts, other machines and the fourth biome are still
unfinished.

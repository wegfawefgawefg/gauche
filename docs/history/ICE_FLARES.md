# Signal flares and spent cinders

## Tool and physical flight

Signal Flare costs 10 gold, stacks to three, consumes one on successful launch and
has a 0.75s cooldown. It travels eight cardinal cells at four ticks per cell; Long
reaches twelve. Other attributes are rejected rather than supplying fake effects.
An exhausted entity pool rejects launch without spending the item.

Flight is visible. Solid terrain, blocking props and gates stop it on the near
side; an impassable actor stops it on that actor's cell. There is no instant hit
at the aimed distance. Traveling over a pool does not light the pool. Splashing
water on the projectile can quench it; landing in water extinguishes it immediately.

The fifteen-second fuel timer begins on landing, not at launch. Flight and fuel
run in the physical timer phase, independently of player/enemy action decisions.
A landed flare is nonblocking, emits red light (radius seven, strength 1350) and
remains at its actual tile. Its small flight light is separate. Unused inventory
flares have no light or heat.

## Heat and counterplay

A burning flare participates in the shared exposed-flame query: it can thaw nearby
ice, melt approaching frost puffs and attract existing heat-seeking creatures.
It ignites susceptible oil/props on its cell and gives eligible actors touching
that cell the ordinary five-second weak burn: four HP per second. Remaining on
it refreshes that burn. Friends, its owner and enemies share the rule; shields do
not turn hot ground into safe ground.

A feeding Steam Leech suppresses its heat/contact effect and dims the existing
emitter through the shared leech rules; fuel still runs down. Water, cold-flask
quenching and water-based tools extinguish it. Extinguishing or spending fuel
immediately clears its light and marks it dead for normal cleanup. No collectible
unlimited flare or reusable fuel appears afterward.

## Presentation and integration

Three new 16x16 sprites: unopened red tube, burning tube and spent cinder fragments.
Five original offline sounds: launch, landing, quiet burning hiss, wet extinguishing
and dry burnout. Launch/landing feed the shared hearing system; the soft recurring
hiss does not create repeated enemy investigation events. Sparks are drawn on the
projectile, smoke uses the existing local particle templates.

Spent/doused flares leave four soft cinder pieces in the local debris pool. These
are Ice's twentieth implemented debris material. They use light-piece friction
and ordinary local pushing/wall collision; they produce no heat, light, collision
or gameplay state. They are distinct from the heavy coal chunks and brass scraps.

Whiteout Drummer now drops one item on a single roll: 20% muffling felt, next 10%
signal flare, otherwise nothing. Weather stations alternate flare and Snow Globe
supplies within the equipment budget. Regional shops and rewards include flares.
The detail panel shows burn duration, contact-burn duration, stack and range.
Debug world previews stop the flight lane at actual obstructions/first actors.

All payload, fuel, phase, cell and owner references use saved projectile fields.
Snapshot layout remains 35; gameplay protocol is `0x2026091503`. The existing codec
fixture preserves an active flare with 713 fuel ticks, its item, light and owner.
Decoded flare state validates fuel/travel/range/phase limits and cardinal facing.

## Verification and remaining scope

Strict game/render/codec builds and the existing snapshot check passed. Static
captures of flight, landed red light, cinders and the normal/Long comparison were
inspected. New PNGs and decoded OGG samples passed format/headroom checks. No live
playtest or new test suite. Combat feel and the audio mix still need user feedback.

Ice now has twenty enemy behaviors, thirty-four regional items, twenty debris
materials and twenty ambient cues. Sixteen items, authored special floors and
remaining biome/system tasks remain open; the whole Ice catalog is not complete.

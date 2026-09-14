# Ice implementation notes

## Ice terrain foundation (2026-09-14)

Existing floors 9–12 now choose reservoir, fishing hut, bathhouse, quarry,
observatory, shelter and echo-tunnel roles alongside caches. Native slate,
snow, ice and two-depth water sprites replace tinted forest ground. Snow is
walkable/buildable; reservoir banks, pools and room shapes retain protected dry
paths and the existing gated leaf boundary. Warm shelters use real campfires;
there are no new invisible roof lights. Shared supplies and sparse crates/pots
support the rooms while their regional catalog is implemented.

Four offline-generated snow/ice footing sounds play on grounded steps. Terrain
impacts use the cold wall sprite; the shared crack/contour lighting remains.
TileKind::Snow is appended, handled by existing tile hash/snapshot serialization;
layout stays 27, gameplay compatibility advances to DC. Forest RNG consumption,
terrain selection and room pools stay unchanged. Run order remains forest/fire/ice
until the full four-zone integration. Ice still uses the old five-tick step
penalty; slipping, grit, thawing and the catalog enemies are subsequent work.

Strict game/render builds pass. Static reservoir and generated ice-floor captures
were inspected; the reservoir was adjusted to one basin and a broad southern
bank. Four OGG files decode to finite non-silent samples. No live playtest ran.

## Rime Skater and Grit Pouch (2026-09-14)

Rime Skaters now spawn on reachable ice in reservoirs, quarries and fishing-hut
rooms. They have 50 HP, a 30-tick push-off, five-tick cardinal glide steps and a
12-cell maximum commitment. Their direction is saved before the push. Rough or
gritted landings stop them; walls/props/actors cause a 60-tick recovery and brief
stun. Contact strikes for 18, including another creature or a straw decoy. External
displacement cancels the original lane. They reposition toward visible ice lanes
when not aligned; no body-position interpolation or infinite sliding is involved.
Three native poses and four blade/impact/death cues accompany the behavior.

Ordinary grounded movement on bare ice now takes one extra real cell, stopping
at blocked landings, using the existing bounded oil-slip path. The previous
five-tick ice step penalty is removed. Sticky Boots resist it; living sap/honey
and spent sticky resin also provide traction. Skaters own their glide cadence,
so they do not receive the ordinary extra ice step. Oil keeps its existing slip
rule, including when spilled over grit. Ice slipping has its own short scrape.

Grit Pouch has six uses, price 6 and 30-tick cooldown. It scatters a three-cell
facing strip on clear, dry, ungritted ice. A wholly inapplicable use costs nothing;
a partial strip spends one use. Big covers five cells; Durable supplies twelve
uses. Placed grains stay visibly lit on the ground and stop ice slips/glides.
Water clears them. The empty pouch has a cloth cue rather than a box-breaking
sound. Definition-driven cards show traction area, remaining uses and pattern.
Cold room supplies, occasional rewards, shops and a 20% skater drop provide it.
The later skate-blade drop from the design catalog remains unimplemented.

Surface::gritted is hashed and snapshot-serialized; layout advances to 28 and
gameplay compatibility to DD. The existing codec round-trip includes placed grit
and a partially spent Big pouch and passes. Regional item dispatch now lives in
items/catalog.cpp, with forest and ice definitions kept in their own domains.

Strict game/render builds pass. Static normal/Big comparison cards, the grit
strip and idle/push/glide poses were inspected. Seven new OGG cues decode to
finite non-silent samples. No live playtest or new test suite ran. Ice catalog
completion remains open: this is one of twenty enemies and one of fifty items.

## Frost Bat and Ice Needle (2026-09-14)

Frost Bat has its own entity module and no longer dives or bites using the forest
bat's code. It hangs, inhales for 30 ticks along a committed cardinal lane,
exhales a six-cell frost puff, then relocates toward a nearby wall perch and
rests for 50 ticks. Health is 24. A bounded perch search prefers clear approaches
and firing lanes; route steps remain authoritative cells. Blocked approaches
fall back to rest instead of retrying forever. Hearing still makes it flee.
Displacement during inhale cancels the tell. Three new bat poses include a
pale swollen throat; wing animation never interpolates the entity position.
The old arbitrary bright blue bat light is removed.

Frost puffs are saved projectile entities, moving one cell every six ticks.
They strike the first non-owner actor for 6 damage and two seconds of chill;
shields block both. Walls, closed fixtures and solid props stop the puff, including
when that hit breaks a prop. Exposed fire dissolves it. The existing CHILLED HUD
reports duration and half-speed movement recovery; inventory use stays available.
The shared chill helper rejects dead/anchored actors, flames and Frost Bats.

Ice Needle is a stack-eight recoverable throw: 8 damage, range 7, 30-tick cooldown,
60-tick chill and price 4. A throw removes one from the stack; its real projectile
carries the exact modifier-bearing item until landing. Strong, Agile, Heavy and
Long work through the shared effective pattern. All Piercing can carry it through
actors; parries preserve the existing finite flight deadline. Shielded hits do
not chill. Hot landings melt it before a nearby item cell can relocate it away
from the heat. Other landings restore the physical needle. Heat means lava,
burning surfaces/actors, live campfires and exposed torches/lit sticks, not the
color or brightness of an arbitrary lamp. This is not yet general biome thawing.

Echo tunnels supply needles; Frost Bats drop one on a 15% roll. Ice rewards and
shops can select needles or grit. The formerly unreachable quarry pickaxe supply
now runs inside the quarry branch. Cards show effective damage, chill duration,
stack capacity and pattern; three modifier cards have static capture support.
Eight new offline cues cover inhale, exhale, wings, death, frost dispersal,
melting, needle throw and needle hit, with small local impact particles.

Strict game/render builds and static frost/needle captures pass. All eight OGGs
decode to finite, non-silent samples below clipping. No live playtest or new test
suite was run. Existing fields already serialize/hash the projectile and AI state;
snapshot layout remains 28, gameplay compatibility advances to DE. Ice now has
two implemented catalog enemies and two items; other quotas remain open.

## Bell Diver and air holes (2026-09-14)

Bell Diver now has a bespoke phase machine: submerged swim, bell warning,
emergence, hunting, swing, recovery and sinking. It has 72 HP, 18-tick swimming
steps, a 36-tick bell tell, 24-tick emergence and a 30-tick windup before its
18-damage adjacent strike. The strike holds its target cell; leaving that cell
or displacing the diver out of reach avoids it. After 48 ticks of recovery it
returns to a hole. Hunting is limited to 150 ticks and stays near its entrance.
If every entrance becomes inaccessible it stays exposed and can fight on land.

Underwater routes flood only connected ice/water, with a 256-node, 12-step local
search. There is no teleport between pools. Bubbles render at the true cell below
actors; swimmers do not obstruct walkers or trigger land contacts. A player or
blocking prop over an entrance prevents emergence and sends the diver toward an
available opening. Ordinary blockable attacks pass above a submerged diver;
area damage still reaches it, and Thunder Acorn chains treat it as wet. It never
attacks while submerged. Being shoved off a hole during sinking interrupts the
dive and leaves it exposed. Shared phase/timer/point slots own all behavior.

IceHole is a new shallow, walkable water tile with a pale rim and dark center.
It uses existing wet contact and water-step rules. Reservoir/fishing-hut rooms
place up to two openings in their northern ice, away from protected dry routes.
Ordinary scenery avoids covering them at generation time. Fishing huts and
alternating reservoir rounds select divers from reachable openings; other cold
encounters keep their skater/bat selections. A diver drops 2–5 gold on a 25% roll.
Its proposed Air Bladder equipment/drop remains unimplemented, explicitly recorded
in the catalog rather than substituted with an unrelated item.

Seven source-generated sprites cover the hole and all readable diver phases;
seven new offline sound cues cover bubbles, brass warning, emergence, windup,
strike, dive and death. Rise/dive emit local splash rings. The hole background
matches native reservoir ice. Static phase and reservoir captures were inspected;
strict game/render builds pass, and all seven OGGs decode to finite non-silent
samples below clipping. No live playtest or new test suite ran. Existing enum,
phase and position serialization covers these additions; snapshot layout remains
28 and gameplay compatibility advances to DF. Ice now has three catalog enemies
and two catalog items; the rest of its requirements remain open.

## Air Bladder and floating cargo (2026-09-14)

Air Bladder now spends one of three uses on an outward push, or on a float when
aimed at loose loot in adjacent shallow water. Price is 10, cooldown 45 ticks.
The ordinary pulse reaches the four cardinal neighbors and shoves each eligible
actor one cell, using the existing grip/block/crush rules. Big reaches two cells
along each arm; targets are captured before moving and outer targets go first,
preventing the same actor from being selected again after displacement. Durable
provides six uses. Empty or fully blocked attempts spend nothing.

A float uses the cargo's existing GroundItem entity. Its exact stack, modifier,
condition and generation stay intact. GroundItem label_a, counter_a, timer_a,
point_a and point_b hold the floating flag, remaining range, travel beat, expected
cell and heading. One real cell per 12 ticks, up to sixteen cells, with one final
dry-bank landing. Deep water, solid props, walls, actors and other loose items
stop it. Drying puddles or external displacement end the trip. No body position
interpolation, new item copy, separate physics world or hidden cargo inventory.

The cargo remains available for ordinary pickup. Taking the full stack deflates
the sling; partial pickup leaves the remainder floating. Swapping deflates before
replacing the payload, so the newly dropped item does not unexpectedly sail off.
The world shows a small inflated sling underneath the real item; local splash
rings and quiet paddling accompany the trip. Debug pattern previews distinguish
the outward pulse from the aimed float route. Cards show push reach, float range,
uses and the actual Big cross pattern instead of a misleading zero cast range.

Fishing huts now offer the local bladder. Bell Diver drops use one roll: 20%
bladder, 25% 2–5 gold, otherwise empty. Ice rewards and shop tools include it along
with grit and needles. Two generated sprites and five new offline sounds cover
the bladder, cargo sling, inflation, attachment, deflation, paddling and exhaustion.
Strict game/render builds and static cargo/card captures pass. All five OGGs
decode without non-finite samples or clipping. No live playtest or new test suite
ran. Existing shared fields already serialize/hash the float; snapshot layout
stays 28, gameplay compatibility advances to E0. Ice totals are three enemies
and three items; its remaining catalog and all broader master work stay open.


## Cold Flask and shared thawing (2026-09-14)

Cold Flask throws a visible arcing bottle to five cells, stopping short of hard
cover. Its impact chills exposed actors for three seconds in a cardinal cross,
without direct damage. Shields do not block environmental cold; frost bats and
living embers resist it. It consumes one from a stack of three, costs 10 gold,
and has a 45-tick cooldown. Big extends each arm to two cells; Long throws to
nine. Observatory/shrine equipment, ice rewards and shops now supply it.

The impact captures exposed cells before changing terrain, then quenches the
whole exposed cross before checking remaining heat. Campfires become cold ash,
stop cooking and lose their light. Burning actors and exposed burning sticks go
out. Permanent torches and living embers keep producing heat; light color alone
does not imply heat. Nearby heat also clears chill during actor timers.

Shallow water, springs and diver holes freeze for 480 ticks. Each tile remembers
its original kind; repeated freezing refreshes the timer without losing that
memory. Frozen cells are dry and slippery, so cargo stops floating on them and
grit can be applied. The final two seconds show cracks. Expiry restores the
original water, including a diver opening. Deep water is deliberately excluded.
Terrain replacement cancels a stale freeze instead of restoring water over it.

A shared temperature pass collects actual flames once, then thaws their own
cell and cardinal neighbors. Campfires, exposed torches/lit sticks, burning
actors, embers, surface fire and lava produce heat. Native ice melts to shallow
water; temporary ice restores its remembered source. There is no per-tile scan
through every entity. General heat-fragile walls, snow tools, heat capsules and
stove fuel remain later catalog work.

Three generated sprites cover the bottle, frozen sheet and thawing cracks. Four
new offline cues cover the throw, cold glass burst, quenching hiss and quiet thaw.
Impact frost/rings are local cosmetics. The debug preview uses the same exposed
cells as the actual impact, and inventory cards show the modified cross, travel,
chill duration, freezing duration and stack rules.

Strict game/render builds and the existing snapshot codec check pass. The codec
fixture now includes frozen spring and diver-hole tiles with different remaining
timers; both source kind and timer are serialized and hashed. Snapshot layout
is 29, gameplay compatibility E1. Static pool/card renders were inspected; no
live playtest or new test suite ran. Ice has three enemies and four items; the
remaining catalog and broader master list stay open.

## Heat Capsule and lingering warmth (2026-09-14)

Heat Capsule breaks around its user, warming their cell and four cardinal
neighbors for four seconds. Big extends each arm to two cells. It consumes one
from a stack of four, costs 8 gold and has a 45-tick cooldown. Shelter/bathhouse
supplies, ice rewards and ice shops include it. Cards show the self-centered
cross, actual warm reach and duration; the debug preview shares its exposure
function with the effect. Solid cover stops warmth reaching cells behind it.

Surface warmth has its own deterministic timer, separate from fire, wetness,
chill and lighting. It immediately clears chill and thaws affected ice, then
continues to resist freezing and melt cold projectiles while present. Actors
entering the patch lose chill through the existing temperature check. Warm
water stays water; a restored diver opening remains an opening. Oil/sap or
flammable props can ignite immediately or when introduced later. The patch
itself deals no damage and provides no health or burn immunity. Cold can quench
its fires, but remaining chemical warmth may reignite fuel on the next tick.

A capsule's painted area never expands through neighboring warm cells. Only
actual flames warm their cardinal neighbors, so burning fuel can legitimately
spread heat farther. Three small ochre flakes and faint rising wisps show each
warm cell, fading in its final second. No broad colored overlay or new ambient
light was added. A native foil capsule sprite and two offline sounds provide a
crack and a brief chemical fizz; heat visuals remain local cosmetics.

Game/render builds pass with strict warnings, the existing snapshot codec check
passes with a nonzero warmth timer, and static terrain/card renders were
inspected. Generated audio decodes without non-finite samples or clipping. No
live playtest or new test suite ran. Snapshot layout is 30 and gameplay
compatibility E2. Ice now has three catalog enemies and five catalog items;
remaining enemies, tools, debris, ambience and progression remain open.

## Steam Leech (2026-09-14)

Steam Leech has 38 HP and takes one real cell per 18-tick movement beat. It
searches a nine-cell neighborhood for actual exposed flames or warm terrain,
then follows a bounded route toward its chosen source. Campfires, embers,
burning actors, held or dropped torches/lit sticks, warm capsules, burning
surfaces and lava can attract it. Ordinary lamp color does not imply heat.
It wanders or responds to hearing when no usable source is present.

An adjacent leech shows its mouth during an 18-tick latch, feeds for 120 ticks,
then visibly swells with a 36-tick pressure warning. Its scalding release deals
12 damage in its own cell and the four cardinal neighbors, respecting cover
and normal directional defense. Friends and props are affected too. It then
rests for 90 ticks. Its own fire/lava immunity lets it approach hot sources;
physical attacks, explosions and another leech's scald still damage it.

While feeding/swelling, the source cell's emitted light falls to 25% and its
radiant heating is suppressed. Flame contact remains dangerous and existing
fuel/lifetime clocks keep running. Multiple attached leeches do not multiply
the dimming. The source's real emitter is never overwritten, so it can change
or vanish without a stale brightness value being restored later. Heat and
lighting collect active drained cells once per pass, not once per map tile.

The shared slots hold phase, phase/search timers, source handle, source kind,
remembered terrain source and attachment origin. A shove invalidates that
origin immediately. Moving/stowing/picking up/extinguishing a source also
releases it when the current source is no longer adjacent or valid. Cold,
sleep and stun put the leech into a resting phase; cold releases it before
nearby warmth can erase the chill timer. Killing it ends the drain immediately.
All of those checks use the existing generation-safe handles and saved slots.

Bathhouse and shelter encounters now select leeches; late rounds can pair one
with a frost bat. Bathhouses get an actual campfire, while other cold rooms
retain their existing source placement. Leeches drop a Heat Capsule on a 20%
roll; the proposed sealant drop waits for the actual item. Six generated
sprites cover five body phases and a distinct pale steam puff. Six new offline
sounds cover attachment, sipping, warning, burst, detachment and death. A thin
suction tether and centered breathing pulse communicate the active attachment;
body placement remains at the actual gameplay cell.

Strict game/render builds pass, attached/released static lighting captures and
body poses were inspected, and all source sprites/audio passed format and
clipping checks. No live playtest or new tests ran. No snapshot layout change
was needed: phases, timers, cells and handles already serialize and hash.
Snapshot layout remains 30; gameplay compatibility advances to E3. Ice totals
are four catalog enemies and five catalog items, with remaining requirements open.

## Cold remedies (2026-09-14)

Wool Wrap clears chill and prevents new chill for eight seconds. Applying it
while already wrapped or burning is rejected without spending a wrap. Ignition
removes its protection and sustains the normal weak burn for at least five
seconds; this can coexist with an existing stronger burn. It does not block
fire damage. Three wraps fit a stack, at a base shop price of eight.

Hot Broth clears chill and restores twelve HP over four seconds. Any actual
health damage interrupts its remaining healing, with a wet spill sound;
blocked attacks do not interrupt it. Ice Poultice extinguishes both burns and
restores ten HP over five seconds, but causes three seconds of chill. Wearing
wool prevents that chill. Broth stacks to three at price nine; poultices stack
to four at price seven. Restorative variants increase the total healing by
50%, keeping the healing rate and extending its duration, with 25% longer use
cooldowns. Base use cooldown is 45 ticks.

Herbs, broth and poultices share a saved recovery kind, pending health and
healing beat. A pending recovery cannot be replaced by another; full-health
use requires a symptom that the remedy actually cures. Healing portions at
full health are spent rather than banked. Existing herbs continue healing
through damage as before. Cards show total healing and duration; status rows
show remaining time, rate, remaining HP, interruption and insulation rules.

Shelters stock broth and wool; bathhouses stock poultices. All three enter the
ice reward and shop pools. Three quiet 16px icons and five offline synthesized
cloth, ignition, sip, spill and compress sounds accompany their effects.

Strict game/render builds and the existing snapshot codec check pass, including
nonzero chill protection and a poultice healing beat longer than herbs allow.
Static comparison and status renders were inspected; sprites and decoded audio
passed size, finite-sample and headroom checks. No live playtest or new test
suite ran. Snapshot layout is 31; gameplay compatibility is E4. Ice now has
four catalog enemies and eight catalog items; remaining requirements stay open.

## Ice Mason and temporary cover (2026-09-14)

Ice Masons replace skaters in quarry encounters. Each has 60 HP, a 24-tick
movement beat and one visible carried block. Seeing a threat within six cells
starts a 30-tick placement tell toward that threat. Placement requires an empty,
dry, cool, walkable cell: no burying loot, fixtures, another actor or a living
prop. Failed placement retains the carried block. An adjacent threat instead
gets a committed 24-tick chisel warning and a 12-damage jab, followed by 42 ticks
of recovery; the worker does not chase players just to jab them.

After building, a bounded local search finds a reachable standing position
beside native ice, favoring distance from the last visible threat. Cutting takes
90 ticks. Finishing consumes that floor ice into shallow water and grants one
replacement block. Temporary frozen lids, diver holes, occupied cells and hot
ice are ineligible. Missing or inaccessible sources cause a pause/fallback,
not free replenishment. Movement uses the existing bounded path finder and real
cells. The shared phase, carried count, timers and two points hold all state.
Ordinary noise can distract an idle worker through the hearing c-slots.

Actual damage, sleep or stun cancels cutting, building and jabs. Displacement
invalidates the remembered work origin before completion. Cutting also rechecks
its source throughout the tell. An interrupted carried block remains visible;
the cargo overlay stays below the worker's face in every facing direction.

Placed blocks use the existing six-byte prop storage: 35 HP, blocking collision,
and a ten-second clock in the prop timer slot. Their last two seconds show a
cracked/melting sprite. Actual flames or chemical warmth melt them to a short
water puddle; the worker cannot build on hot cells. Melting leaves the underlying
floor and pre-existing non-water liquids intact. Ice is not flammable prop fuel.
Breaking a block emits eight local blue ice chips, using the established debris
budget, collision and heavy-piece friction. Melting produces no shatter debris.
All cover expires, so even a narrow passage is never sealed permanently.

Eight generated 16px sprites cover worker poses, intact/thawing cover and the
first ice debris material. Nine new offline sounds cover cutting, lifting,
placement, warning, jab, death and ice impacts. Current mason loot is a 25%
chance of 2–4 gold; the planned brick and chisel drops remain open with those
items. Strict game/render builds and the existing codec check pass, including
a saved ice prop timer of 411 ticks. Static poses/cover were inspected and
adjusted to preserve visible faces. Asset format/audio headroom checks pass.
No live playtest or new suite ran. Snapshot layout stays 31; gameplay is E5.
Ice totals: five enemies, eight items, one dedicated debris material; the rest
of the catalog and biome integration remain open.

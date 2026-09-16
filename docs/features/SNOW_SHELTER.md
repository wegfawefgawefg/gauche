# Snow Shelter and low cover

Implemented Ice item 49/50. Two uses, four Durable; other attributes are unsupported.
Value 18 and a 0.75s use cooldown. Use raises two sections: the cell ahead and the
cell to its right relative to facing. Both must be clear, dry and cold before
either changes. Bodies, items, fixtures, living props, heat and unsuitable terrain
reject the whole placement without a charge. The diagram and debug placement
preview use those exact two cells.

Each section is an independent 24-HP SnowWindbreak prop in the existing eight-byte
prop struct. No new entity or status field. The two orientations share rules but
have separate horizontal/vertical sprites. They block walking, ordinary sight,
wind propagation, frost breath, snowballs, flat projectiles and beams. Existing
attacks damage them. A shot that breaks cover still stops there; it does not hit
someone behind the broken section on the same step. Broken sections scatter local
snow-clump debris and stop blocking. There is no loot from player-built walls.

Actual lobbed bombs, flasks, mixtures, firecrackers, prism bombs and echo pebbles
cross low cover during flight. Full walls, other blocking props and closed doors
retain their previous behavior. The final landing step still checks low cover;
the existing landing preview follows the same rule. Snowballs and heavy thrown
ice bricks retain their direct-impact cover behavior. Rockets and ordinary bullets
also retain their collision rules. The optional overhead flag on projectile
obstruction is available for later arcing attacks.

Shared ice-cover melting handles shelter sections too. Flames, heat capsules,
Borrowed Summer, brine and other existing thaw sources can
remove a section, leaving a three-second water surface where liquids allow it.
It uses a wet melt cue rather than a shatter. Shelter sections do not use the
mason block's ten-second expiry; cold walls remain until broken or thawed. Existing
wind/sight queries therefore handle the shelter without an immunity aura. Wind
can still go around the ends when there is an open route.

Shelter-room equipment now alternates the shelter kit, Snow Scoop and Wool Wrap.
Snow caches gain a 10% shelter kit find from their former empty pool; the existing
30% snowball, 20% wool, 15% ammo and 25% gold pools remain. Ice shops/rewards include
it. Three sprites and five offline packing/crunch/melt sounds complete the item.
Setting or breaking cover reaches existing sound listeners.

No snapshot layout change: orientation uses prop.variant (0 horizontal, 1 vertical),
with normal HP/broken state; invalid orientation/zero-live-HP is rejected. Snapshot
format remains 38; gameplay compatibility is 0x2026091518. Cover, flight collision,
loot and temperature behavior are deterministic; debris/audio/rendering stay local.

Validation: strict game/render builds; focused direct checks for atomic placement,
occupied/hot cells, rotations, use count, movement obstruction, snapshot/hash,
frost and snowball cover, breaking-hit protection, independent sections, overhead
bomb/flask/prism travel, full wall/door rejection, warmth/puddles, a committed
drummer squall, corridor wind, attributes and malformed orientation. Static world
and inventory captures inspected using dummy SDL drivers. No autonomous playthrough
or permanent test suite.

Assets: tools/art/snow_shelter.py, tools/sound/snow_shelter.py.
Static render modes: shelter, shelter-items.

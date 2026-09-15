# Folded Bridge

Implemented Ice item 44/50. One nonstacking kit lays three wood sections straight
across shallow or deep water. Stand on dry ground and face a crossing with another
dry bank four cells away. All three water cells must be free of actors, loose
items and intact props. Failed placement changes nothing and spends nothing.

Each section has 30 HP, or 60 with Durable. The sections are dry walkable ground,
with wooden footfalls, visible damage, flammable wood, splash and local wood/rope
debris when destroyed. Each broken section restores its exact original water kind,
including spring and ice-hole water. Surviving sections remain while both original
banks exist; melting either bank collapses the rest on the next prop step.
Bank furniture does not remove structural support, but blocks initial placement.
Magic train rails can replace a section without leaving an invalid bridge prop.

A shallow collapse wets occupants and quenches burning through the shared water
contact rules. A deep collapse uses the existing lethal fall rule for land
creatures, including players; flyers, eels, divers and seals survive. Ground loot
remains in the water. Cold-water rescue, drysuits and slow freezing are still
separate pending work; this milestone does not implement those systems.

Fishing huts can supply a kit; fishing creels have a 10% kit drop, plus the existing
fish/line/bladder drops. Ice reward and shop pools include it at value 24.
Bathhouses and reservoirs now have three-cell pool widths with dry/frozen banks;
the carver still reserves mandatory dry routes first. Bridges are optional tools,
never a newly required key for progression.

Bridge orientation, section index, original water kind and Durable flag fit in the
existing prop variant byte. The tile and prop are saved/hashed together; decoding
rejects orphan bridge tiles, missing planks, invalid water and section indices.
Gameplay compatibility is `0x2026091513`; snapshot layout remains version 37.
Cosmetic fragments and sound playback remain local.

Three new sparse 16px sprites and four offline synthesized sounds cover the kit,
both plank orientations, unfolding, breaking, splash and footsteps. Reproduce
with `tools/art/folded_bridge.py` and `tools/sound/folded_bridge.py`. Static capture
modes: `bridge`, `bridge-broken`, `bridge-fire`, `bridge-items`.

Validation: strict game and render builds; focused direct checks for transactional
placement, all four cardinal supports, traversal, individual collapse, exact water
restoration, shallow quenching, deep falls/flyer survival, lost banks, Durable HP,
flammability, rail replacement and valid/invalid snapshots. Static generation of
four Ice floors at seed 1701 yielded 28 clear spans without advancing simulation.
Dummy-driver world, broken, burning and inventory captures inspected. No autonomous
playthrough and no permanent test suite added.

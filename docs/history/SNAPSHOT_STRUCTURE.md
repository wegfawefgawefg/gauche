# Snapshot serialization responsibilities

The snapshot codec reached the 500-line limit while adding Ice content. Its
existing serialization is now separated by responsibility:

- `src/net/packet.cpp`: byte primitives, input packets, cells and light emitters.
- `src/net/entity_codec.cpp`: entities and their carried/ground items, including
  validation of enemy, projectile and item state.
- `src/net_codec.cpp`: stage, run, entity ordering and cross-entity reservations.

All targets use the same CMake source list. Field order, validation, snapshot
version 35 and the gameplay protocol version remain unchanged.

Validation: strict game, static-render tool and existing codec target builds
passed. A temporary copy of the existing snapshot fixture also linked the
pre-refactor codec under separate names: both produced the same 354,301 bytes,
and the previous reader restored the current writer's snapshot with the same
game hash. The existing round-trip, truncation and input checks passed. No new
test suite or live playtest was added.

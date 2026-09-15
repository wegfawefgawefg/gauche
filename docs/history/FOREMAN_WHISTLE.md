# Foreman's Whistle

The first implemented Industrial regional item lets players borrow the work
crew's call. An idle, awake Pickhand within eight audible cells accepts a fixed
job four cells in the aimed cardinal direction, offset into its own lane. The
order lasts up to four seconds. Workers use their existing route costs and
25-damage, power-1 cuts: this gives no new ability to breach reinforced exits or
outer walls. Six successful calls, 1.5-second cooldown, price 18; Long extends the
job distance to eight cells, Durable gives twelve calls. It does no damage.

Already angry, stunned, rooted, sleeping or committed workers refuse. A foreman
currently sounding an audible order has priority. His next completed audible
call takes linked workers back; walls and closed doors block both kinds of
orders. An existing cut/strike finishes against its committed target, even if a
new real order replaces the job during its windup. Real damage ends the false
order. Dead or stale leader handles cannot stop a player from directing an idle
surviving worker.

A successful call also wakes sleepers and attracts existing hearing enemies.
An inapplicable call spends no use and makes no semantic noise, avoiding a free
unlimited bell. The first accepting worker answers audibly. Three new offline
OGGs provide a short-short-long command, a grunt answer and a cracked final note.
A native 16px brass whistle with a dark cord has matching held/inventory art.

The foreman's single-roll drops are now 25% whistle, next 25% 5–9 gold, otherwise
nothing. A workfront has a one-in-three chance of leaving one among its supplies.
Industrial item rewards and the middle shop slot each have a one-in-four chance
of choosing the implemented regional pool; existing guns/tools remain in their
other branches. No unimplemented catalog entry enters that pool. The debug
loadout selector exposes the item and supported variants automatically.

Shared point_c, label_c and timer_c store the fixed job and its lifetime. These
already participate in hashes and snapshots; the decoder now accepts the new
named attention state. Snapshot 41 and gameplay version 0x2026091522 prevent old
clients from interpreting the new item/behavior.

Validation: Release game and renderer builds; direct-function checks of use
consumption, refusal, lane goals, actual worker response, expiry, damage
interruption, Long/Durable variants, spent removal, audible leader priority and
snapshot/hash round trips. Static inventory render inspected and description
shortened to fit the detail panel. No autonomous playthrough or permanent suite.

# Hearth: finite meals instead of passive regeneration — 2026-09-15

The previous Hearth healed every nearby player once per second, once per owner.
Four owners could restore four HP per second to each clustered player indefinitely.
That undermined food, recovery choices and attrition. This is the first artifact
strength correction; the broader artifact roster, reward-value matching and
acquisition/price/earliest-stage pass remain open.

Hearth now adds **3 HP to the eater and each eligible friend within four cells
when the owner successfully eats a cooked meal**. The supported meals are cooked
meat, fried egg, smoked fish and hot broth. Walls block sharing; smoke does not.
Health caps apply and dead players stay dead. Raw meat, raw eggs, medicines and
poultices do not trigger it. The food keeps its ordinary sound and primary effect.

One consumed portion produces one bonus. Other recipients owning Hearth cannot
retrigger or multiply it. Standing idle, food cooking, meals eaten by someone
without Hearth, failed uses and gradual recovery ticks provide no bonus. Full-health
owners may spend food to help an injured friend; when nobody can benefit, ordinary
full-health consumption is rejected. Broth retains its existing symptom/recovery
rules, including rejection while another recovery is active.

Examples at low enough health:

- Cooked Meat: 18 normal + 3 Hearth HP to its owner, 3 to each eligible friend.
- Fried Egg: 12 normal + 3; Smoked Fish: 14 normal + 3.
- Hot Broth: 3 immediate Hearth HP, then the normal interrupted-by-damage recovery.
  Its later healing ticks do not share additional HP.

This remains useful alone, rewards preparing/carrying meals and permits deliberate
co-op sharing. Even with four players, each meal adds at most twelve total bonus
HP, paid for by the finite food portion. Human feedback will determine whether
three HP and the current acquisition frequency feel worthwhile.

The artifact catalog, owned-effect summary and qualifying item detail panels
show the new behavior. The old per-tick aura was removed. The gameplay helper
lives in `src/artifacts/hearth.cpp`; it adds no saved fields or cosmetic simulation
state. Gameplay version is **0x2026091534**, wire **14**, snapshot layout **49**.
All peers need the same rebuilt gameplay version.

Release game and static renderer builds pass. Temporary direct-function checks
cover all four meals, consumption/cooldown, healthy-host sharing, full-party refusal,
range/walls/death, raw-food/medicine exclusions, multiple owners, blocked recovery,
health caps, no passive healing, and deterministic continuation from snapshots.
The existing Hearth assertion was updated to its new contract. Static inventory
and reward captures were inspected. No autonomous playthrough or large new test
suite was added.

# Conditions, terrain and objective feedback — 2026-09-15

Pending work linked from MASTER_TASKS.md. This is the detailed specification,
not another completion checklist. Nothing below is claimed implemented merely
because it is recorded. Numbers are initial design targets for user playtesting.

## Confirmed gaps and existing behavior

- Water on a campfire failed during playtesting. `pour_surface` extinguishes
  surface fire, candles and stoves, but does not extinguish Campfire entities.
  Connect water to the actual campfire state: heat, light, cooking, contact
  burning and flame emission must agree, with extinguishing sound and smoke.
  Check fixture interaction priority so cooking cannot consume the water action.
- Splint currently clears stun and grants three seconds of stun resistance. It
  can be used while stunned, but the observed stuns are too short for convenient
  reactive use. Give it a useful lasting-injury role.
- Owl already calls `feed_on_bird_seed`, but attack, recovery, hearing, fear and
  return-to-perch phases can take priority. Feeding requires visible reachable
  scattered seed within eight tiles. Investigate the reported ignored bait;
  make intentional lure priorities and eligibility consistent and understandable.
- Ordinary exit logic currently has Key/Switch objective kinds and one
  `run.has_key` boolean. Either interaction sets that flag. This cannot express
  two independently required keys or a key plus a lever.
- Ice currently grants an immediate extra cardinal step, not decelerating
  multi-step momentum. Natural surface currents are not implemented yet.

## Conditions, injuries and unusual remedies

Develop roughly 50 distinct, interesting buffs, debuffs and lasting defects.
Avoid padding the catalog with anonymous percentage variations. Include timed,
curable, conditionally curable and genuinely permanent effects; communicate
duration, severity and actual consequences in status and item information.

Candidate interactions requested by the user:

- Broken leg slows walking; broken arm slows item actions. Splints repair
  appropriate injuries. Decide severity and stacking explicitly.
- Strong Arm improves melee and throwing distance; Weak Arm does the inverse.
- Lost eye impairs aiming, potentially with misses. Explore carefully: preserve
  clear cardinal targeting and communicate impairment instead of unexplained
  failed hits. This particular implementation is a proposal, not a settled rule.
- Military repair supplies offer advanced cures.
- Ice Body repairs bones, turns the actor blue with blue sparkles, but doubles
  incoming fire damage. The tradeoff must remain visible after applying it.
- Spicy forest berries can grant about ten seconds of cold resistance and thaw
  the user. Fire-monster meat could grant Warm Bones. Early-biome supplies can
  remain useful later instead of becoming obsolete by zone number.

Keep shared condition state, ticking and derived movement/action/damage queries.
Items and enemies may explicitly apply or remove conditions in bespoke use/step
functions. Existing Ice Poultice already clears burning in its use function;
this straightforward style is wanted. No effect scripting framework is required.

For every added mechanic, specify several causes, counters and uses in each
relevant biome. Prefer interactions across terrain, enemies, food, equipment and
allies. Avoid a single enemy inflicting a defect with exactly one obscure cure.
Permanent defects need deliberate counterplay and useful tradeoffs too.

## Ice momentum and cold water

- A shove or step onto slippery ice can initiate several cardinal steps, with
  increasing time between steps until stopping. Pushed enemies can travel farther.
  Flying actors do not slide. Traction, obstacles and immunity are explicit.
- Actor rendering stays on authoritative cells. Smooth only the camera; do not
  restore apparent positions that lag hit detection, footsteps or item origins.
- Cold shallow puddles cause a modest cold damage-over-time effect. Distinguish
  them from ordinary forest water rather than making all water unexpectedly lethal.
- Deep icy water can trap an actor in a bobbing ice cube, unable to move and
  losing about two HP per second. The user's preferred direction is a rescue
  window, rather than unavoidable immediate death.
- Resistance and appropriate items can prevent freezing or free a trapped actor.
  Allow switching to and using rescue items while immobilized. Consider ally
  rescue, safe shore access and clear warnings when integrating these hazards.
- Drysuit/wetsuit: explore protection active only while holding Use. It can
  prevent cold-water damage or release the cube while maintained. Include squeaky
  footsteps and a goggle-shaped viewing cutout during use. Other consumables can
  still supply timed resistance without holding a button.
- Connect ice momentum, currents, Stillwater Bell and other planned ice tools
  through actual shared terrain rules, not disconnected item-specific imitations.

## Barriers, intelligence and excavation crews

Add movement-blocking barriers that permit shots. Separate movement obstruction,
projectile obstruction and material breakability deliberately.

Smart melee enemies seek a reachable attack-adjacent cell; ranged enemies seek
a reachable firing position. When obstruction prevents either, appropriate
enemies can break or dig it. Ordinary forest animals generally do not perform
this strategic demolition.

Industrial excavation team:

- A foreman whistles orders. Workers assemble perpendicular to the ordered
  direction, chant "hup hup", advance and pickaxe through breakable obstacles.
- Small incidental hits need not interrupt work. A sufficiently strong hit
  makes that worker retaliate; attacking the foreman or killing two crew members
  turns the whole team hostile.
- Pursuers can dig shortcuts and be difficult to escape. Compare estimated
  walking time with demolition time and route length; avoid always digging or
  always following the longest open corridor. Bound route search work.
- Preserve the unbreakable map boundary. Futile hacking is acceptable, breaching
  it is not. Coordinate shared leader/order state through existing entity handles,
  counters and points, checking generations when leaders or targets disappear.

Simpler ice scientist/miner: chips a site for a while, says "hmm", moves to
another site. Possible drops include a surveying map or telescope for temporary
zoom-out, and a deliberately silly microscope that zooms far in while used.

## Zoom and developer access

Normal zoom around 2.0 is part of game balance. Maps/telescopes may temporarily
provide approximately 1.0 zoom, either while used or for a short duration.
Describe the cost and duration. Microscope zoom-in is an intentional item effect.

Add one top-level developer-mode flag governing ImGui, debug overrides and free
zoom adjustment. Leave it ON during development; shipping configuration turns
it OFF. Legitimate item-driven camera effects continue working without dev mode.

## Objectives, escape pressure and floor modifiers

Support separately identified requirements: two keys, a key plus a lever, and
other combinations. Keep generated requirements obtainable before their gates,
provide readable objective information, and preserve intended tool shortcuts.

Additional objective and encounter candidates:

- Exit button starts a roughly one-minute bomb countdown: escape before the
  terminal explosion.
- Cutting reactor power opens the exit gate but triggers a spreading radiation
  and fire disaster. Walls transmit it faster than floors. Use a bounded,
  deterministic work budget per simulation tick, with saved propagation state;
  render-frame timing must not decide spread or peer outcomes.
- Spider nests as room fragments or complete encounters, with small spiders,
  large ones and a mother. Burnable webs can gate an early exit, but generation
  must guarantee an accessible ignition source or another valid solution.
- A slow, lethal pursuer appears after a floor time limit, initially around two
  minutes. Latest user preference: recurring pressure on every ordinary level.
  Decide unique-floor exceptions and how this fits existing scripted encounters.
  Warn clearly before arrival and make its movement readable.
- Explore around ten floor-modifier ideas per biome. This is a tentative design
  pool, not a commitment to ship forty modifiers. Announce active modifiers below
  the stage banner. Do not add complexity solely to hit a count.

Netplay needs a pause-policy option: whole-team pause or a local menu while play
continues. Team pause freezes authoritative time, including the pursuer clock,
reactor spread and status durations. Local menu pause must not imply the world
has stopped. Define timer behavior in rewards, shops and level transitions.

## Integration discipline

First address reported interaction failures, then implement connected groups of
mechanics with their causes, remedies, sounds, visual cues, loot and room uses.
Conditions, momentum, crew orders, objectives and hazard propagation are gameplay
state: preserve deterministic simulation and snapshots. Sparkles, goggles,
bobbing presentation and sound rendering are local cosmetics. User owns live
playtesting; use builds and focused asset/render inspection as appropriate.

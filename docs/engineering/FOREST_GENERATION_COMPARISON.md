# Forest generation comparison

2026-09-16: baseline `forest-gen-baseline-2026-09-16` (`9d1559e`) versus gameplay revision `71d7ec6`. Both Release builds use identical `src/debug/forest_audit.cpp` measurement code, copied into the detached baseline checkout and linked in place of its main entry point. New report-only fields are unavailable on the baseline.

Sixteen independent seeds (1–16) per stage, 64 ordinary generated floors per build; no simulation or playthrough. Automatic whole-floor uniques are excluded. All required-route/exit-lock checks passed; current component reports never reached their cap.

| Floor | Mean mobile fighters, old → current | Fighter proximity, old → current | Ordinary rooms without fighters, old → current |
| --- | --- | --- | --- |
| 1-1 | 9.9 → 31.8 | 31.7% → 57.5% | 34/76 → 2/81 |
| 1-2 | 11.4 → 48.7 | 28.1% → 63.8% | 51/106 → 5/108 |
| 1-3 | 16.6 → 64.4 | 35.1% → 62.9% | 40/106 → 7/109 |
| 1-4 | 17.8 → 73.6 | 34.4% → 70.0% | 59/137 → 4/156 |

Fighter proximity is the fraction of spawn-reachable walkable cells within eight walking steps of a mobile fighter, territorial specialist or boss. It ignores living-actor/door occupancy and does not measure line of sight, aggro range or difficulty. Worm bodies count once per head. Bears/mimics/ants/gnomes, passive life and enemy sources are separate categories. Ordinary room counts exclude reserved habitats and entrance/exit/shrine/secret rooms.

The mean largest connected area outside fighter proximity fell from 945–1,394 cells to 376–496 across the stages, but individual current floors still have quiet components over 1,000 cells. Inspect these in play; sleeping habitats and traversal space do not automatically need hostile infill. Forced ordinary-layout seeds differ from viewer Automatic seeds.

Specialists declined from means of 3.2/3.5/5.6/6.0 to 0.2/1.1/3.5/2.2; enemy sources from 0.6/1.2/1.2/1.9 to 0.0/0.4/0.2/0.8. Passive life averages 7.2/8.8/8.5/10.5 now. More mobile fighters alone do not certify lively ecology or varied combat; review specialist/source exposure on later floors alongside human feedback.

Landmark appearances across the 64 floors: giant trees 8→11, timber groves 15→14, snake habitats 26→14, spider caves 24→26, bear dens 20→17, root mazes 11→15. New built-feature presence: open sectors 40/64, rivers 35/64, ant colonies 16/64, mushroom districts 21/64. Old Growth appeared on all 16 sampled fourth floors. These are sample frequencies, not guaranteed rates or independent per-run exposure estimates.

Generation alone took a median 3.6 ms baseline versus 10.6 ms current, maxima 8.7/21.3 ms on this machine. Single local runs without snapshot capture; neither frame-time measurements nor a controlled performance benchmark.

Repeat current measurements with `./build-release/gauche --audit-forest > /tmp/forest.csv`. CSV also includes walkable area, loose items and accessible props. `interactive_props` excludes leaves/twigs/ferns/tall grass; it is a rough clutter proxy, not a count of distinct meaningful interactions. New feature columns indicate presence on a floor, not individual sector/household counts. Timing is intentionally nondeterministic; generation uses fixed seeds.

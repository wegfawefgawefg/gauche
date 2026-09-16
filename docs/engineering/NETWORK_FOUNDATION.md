# Internet playtest foundation — 2026-09-15

Network heartbeats, hello retries, snapshot requests and disconnect detection
now use monotonic milliseconds. Polling at 144 Hz or uncapped no longer shortens
the six-second disconnect timeout. The lobby uses elapsed join time too.
Existing direct/lossy checks now advance an explicit virtual clock rather than
assuming every socket poll represents a simulation tick.

Snapshots send at most 32 fragments per batch, with 16 ms between batches and
500 ms between full retry passes. Repeated resync requests no longer replace an
active transfer. Late fragments from an older transfer cannot reset newer receive
progress. These changes fix failures exposed by the existing checks while
removing render-frame timing assumptions.

`services/roomd` builds the pinned Gubsy directory, punch and relay directly,
without linking SDL or compiling the game engine. No Gubsy source was modified.
This makes the small VPS service independent of the desktop build requirements.

Validation: strict game build; existing direct session/four-player/reconnect and
lossy-session checks passed. Headless service built and a short local `/health`
check confirmed directory, punch and relay enabled; the process then exited.
No live gameplay, public internet session or deployed service was tested.

Room browser/party integration, authenticated game transport adapters, public
deployment, desync capture and friendly-fire lobby settings remain pending.
An old local Vultr API credential returned HTTP 401; no instance was created.

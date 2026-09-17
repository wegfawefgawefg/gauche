# Local multiplayer workshop

Run from the Gauche repository:

```sh
./scripts/multiplayer.sh
```

This builds once, opens your controllable host plus three random-input bots,
creates a public online room, and joins all three bots through the room service.
The host starts when all four are ready. All-dead runs restart after two seconds;
the launcher uses entrance respawns. It does not move or aim your player for you.

Bots choose rewards automatically, exchanging a droppable item when their pack
is full. They try one affordable shop purchase, then ready up. The fist and
weapons currently in flight are never discarded to make room.

On i3 the default puts your game on workspace 3 on the primary display and
stacks three full-width bot windows top-to-bottom on workspace 4 on the second
display. Bars are excluded from the usable area. The windows float without borders. It only moves
its own uniquely named windows and never kills existing games or clears old logs.
Closing your game or pressing Ctrl-C in the launcher stops its bots. Sessions
have a 30-minute limit by default; use `--seconds 3600` for an hour.

## Layouts and connection scenarios

```sh
# Four quadrants on one display instead of the default two-monitor layout.
./scripts/multiplayer.sh --layout quad

# Your game is visible; three bot clients have no window, renderer or audio.
./scripts/multiplayer.sh --layout headless

# Put bots into a room you have already created in the normal game.
./scripts/multiplayer.sh --layout headless --join ABC123

# Wait for a public room by this exact host name, then join it with three bots.
./scripts/multiplayer.sh --layout headless --follow-host 'Yuki Bonaparte'

# Exercise the VPS relay instead of attempting a direct connection first.
./scripts/multiplayer.sh --force-relay

# Record granular frame timings in each visible client's profile.csv.
./scripts/multiplayer.sh --profile

# Skip compilation when the current binary is already built.
./scripts/multiplayer.sh --no-build

# Inspect monitor/workspace selection without launching anything.
./scripts/multiplayer.sh --layout split --dry-run
```

`--bots 1` or `--bots 2` uses fewer bots. `--name NAME` sets the launcher's human
name. `--output DisplayPort-0`, `--bot-output DisplayPort-1`, `--workspace 3` and
`--bot-workspace 4` override placement. Without i3, ordinary window placement is
used. `--service URL` selects another compatible room server.

Each process gets isolated XDG data/config directories and its own player identity
under `~/.local/state/gauche/multiplayer/session-*/`. Console output goes into
`human.log` / `bot-N.log`; network logs and recovery captures live beneath each
profile's `data/gauche/Gauche/netlogs`. Profiles/logs remain after exit for diagnosis.
`layout.json` records requested and actual window positions. The launcher checks
that placement settles instead of silently ignoring i3 errors. Debug windows keep
a fixed 16:9 canvas with letterboxing, even when resized into a narrow shape.
In split/headless layouts the human canvas uses the primary display's resolution
(1920×1080 on a Full HD display; that is also the fallback without i3).
Visible bots and quad-layout windows keep their cheaper 640×360 canvas.

Network logs include progress every five seconds, confirmed/host/local ticks,
timeline revision, measured RTT, prediction lead, byte totals, and recovery count.
Recovery events name the cause (hash mismatch, expired history, or changed baseline);
the `.grpl` capture preserves the starting world, inputs, and expected/actual hashes.
Keep the entire session directory when reporting a problem, including the host.
Visible bots render at 30 FPS while simulation remains 60 Hz. Bots generate moves,
aiming, use/release, inventory selection, pickup, interaction, reload and reward
choices through ordinary player inputs. They are stress inputs, not intelligent
level-solving AI; dead bots obey the lobby respawn rule.

The headless path is also available directly:

```sh
XDG_DATA_HOME=/tmp/gauche-bot-one ./build-release/gauche --headless \
  --join-room ABC123 --player-name 'Bot One' --bot-seed 10 --seconds 300
```

Give each simultaneous process a distinct XDG data directory. Headless mode can
join an existing room or follow a named host; it does not create a server or
control the human host. A named-host search selects the first matching non-full
public room, so use a distinctive host name when several friends are testing.

## Validation boundaries

The game builds with strict warnings. The four-player UDP regression advances
600 simulation ticks with three clients, 10% dropped packets, reordered packets,
roughly 350 ms round-trip latency, two clients polling at 30 Hz, and one deliberately
lagging client. All clients match the host at tick 600 with zero snapshot recoveries.
Existing rollback and direct-session checks also pass. This is a bounded transport
check, not an autonomous level playthrough.

An isolated Xvfb/i3 desktop with the same landscape + portrait monitor geometry
verified four placeholder windows at their exact requested positions; the user's
live desktop was not rearranged. Live gameplay still needs human feedback.

The wire protocol changed to version 13. Rebuild and restart **all** peers together;
the room service does not need an update for this client/host protocol change.

### Joining a running game and connection reports

Rooms accept newcomers during play (up to four reserved player identities), as
well as in the lobby. Joining transfers the current world and spawns the newcomer
near that floor's entrance; active sealed encounters place them inside instead.
Reward/shop joins skip the current selection so they cannot hold up the party.
The default **Next Floor** death policy revives dead players at full health on
floor transition. **No Respawn** remains an explicit host choice.

Join attempts send at most four small signed diagnostic summaries to roomd's
existing `punch_probe_result` journal event. They contain build revision, connection
stage, reply/rejection counts and clock mismatch magnitude, never credentials or
full snapshots. Reports are best-effort UDP: if outbound UDP is entirely blocked,
only local logs survive. The host UI does not yet retrieve these reports.
Local `netlogs/session-*.log` files now name rejected control replies; the joining
screen shows the specific failure, including authenticated clock mismatches.
No room-server upgrade is required for these reports.

Room connections use the service's HTTP Date as their packet timestamp reference,
advanced with local monotonic time and refreshed on room heartbeats. On the public
service this arrives through verified HTTPS. NTP is not required for the game
handshake; packet authentication and the existing age window remain enforced.
Services without a Date header retain the local-clock fallback. HTTPS certificate
validation still requires a reasonably correct system date.

Run Over / Run Cleared waits for host-confirmed state on clients. A correction or
new floor that returns the party to play dismisses an old terminal screen.
Leaving sends a departure packet; abrupt closes fall back to the six-second
heartbeat timeout. The host removes the world body and keeps character state
privately for the same identity to reconnect, rather than leaving a visible dummy.
Normal deaths scatter carried items and gold; pit/deep-water deaths lose cargo.
The built-in fist remains, and Next Floor respawns do not restore lost items.

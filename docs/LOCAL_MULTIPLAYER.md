# Local multiplayer workshop

Run from the Gauche repository:

```sh
./scripts/multiplayer.sh
```

This builds once, opens your controllable host plus three random-input bots,
creates a public online room, and joins all three bots through the room service.
The host starts when all four are ready. All-dead runs restart after two seconds;
the launcher uses entrance respawns. It does not move or aim your player for you.

On i3 the default is workspace 3 on the primary display, in four equal quadrants
(960×540 on a 1920×1080 display). The windows float without borders. It only moves
its own uniquely named windows and never kills existing games or clears old logs.
Closing your game or pressing Ctrl-C in the launcher stops its bots. Sessions
have a 30-minute limit by default; use `--seconds 3600` for an hour.

## Layouts and connection scenarios

```sh
# Your game on workspace 3; three bot windows on workspace 4, second display.
./scripts/multiplayer.sh --layout split

# Your game is visible; three bot clients have no window, renderer or audio.
./scripts/multiplayer.sh --layout headless

# Put bots into a room you have already created in the normal game.
./scripts/multiplayer.sh --layout headless --join ABC123

# Wait for a public room by this exact host name, then join it with three bots.
./scripts/multiplayer.sh --layout headless --follow-host 'Yuki Bonaparte'

# Exercise the VPS relay instead of attempting a direct connection first.
./scripts/multiplayer.sh --force-relay

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

The game builds with strict warnings. A bounded dummy-video host and three real
headless clients joined through the deployed relay, received distinct player
slots and snapshots, readied, and exited successfully. The host stayed in its
party during that check: no autonomous gameplay was run. Host/Join menu renders
were inspected; i3 placement was checked with the launcher's dry-run against the
actual two-monitor configuration. Live desktop arrangement and random gameplay
are for the user's playtesting.

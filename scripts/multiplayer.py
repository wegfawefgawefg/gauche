#!/usr/bin/env python3
"""Launch an isolated human + bots room session. Ctrl-C stops only its own children."""
import argparse
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time

from multiplayer_layout import i3, arrange

ROOT = Path(__file__).resolve().parents[1]


def window_options(role, layout, primary):
    width, height = 960, 540
    render_width, render_height = 640, 360
    if role == 'human' and layout != 'quad':
        rect = primary['rect'] if primary else {'width': 1920, 'height': 1080}
        width, height = rect['width'], rect['height']
        # Keep the game's 16:9 view, rendered at the human display's resolution.
        render_height = min(height, width * 9 // 16)
        render_width = render_height * 16 // 9
    return ['--window-width', str(width), '--window-height', str(height),
            '--render-width', str(render_width), '--render-height', str(render_height)]


def stop_children(children):
    for child in children:
        if child.poll() is None:
            os.killpg(child.pid, signal.SIGTERM)
    until = time.monotonic() + 6
    while any(p.poll() is None for p in children) and time.monotonic() < until:
        time.sleep(.1)
    for child in children:
        if child.poll() is None:
            os.killpg(child.pid, signal.SIGKILL)
        child.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--layout', choices=['quad', 'split', 'headless'], nargs='?', const='split', default='split',
                        help='quad: four windows; split: human on 3, bots on 4; headless: human window only')
    parser.add_argument('--join', metavar='CODE', help='launch bots into an existing room, without a human host')
    parser.add_argument('--follow-host', metavar='NAME', help='headless bots wait for a public room hosted by this name')
    parser.add_argument('--bots', type=int, default=3)
    parser.add_argument('--max-players', type=int, help='host admission limit (default at least 16)')
    parser.add_argument('--name', default='Local Tester', help='human host display name')
    parser.add_argument('--seconds', type=int, default=1800, help='bounded lifetime, default 30 minutes')
    parser.add_argument('--workspace', default='3', help='human/quad workspace')
    parser.add_argument('--bot-workspace', default='4')
    parser.add_argument('--output', help='human/quad output (default primary display)')
    parser.add_argument('--bot-output', help='split bot output (default second display)')
    parser.add_argument('--service', default='https://45.77.123.14')
    parser.add_argument('--force-relay', action='store_true')
    parser.add_argument('--no-build', action='store_true')
    parser.add_argument('--profile', action='store_true', help='capture 1800 frames per visible client to its session profile.csv')
    parser.add_argument('--dry-run', action='store_true', help='show plan without building, starting games or changing workspaces')
    args = parser.parse_args()
    if args.bots < 1:
        parser.error('--bots must be positive')
    if args.layout == 'quad' and args.bots > 3:
        parser.error('quad fits four windows; use split or headless for more bots')
    capacity = args.max_players if args.max_players is not None else max(16, args.bots+1)
    if capacity < args.bots+1 and not (args.join or args.follow_host):
        parser.error('--max-players must fit the host and all bots')
    if args.seconds <= 0:
        parser.error('--seconds must be positive')
    if args.join and args.follow_host:
        parser.error('choose --join or --follow-host')
    if args.follow_host and args.layout != 'headless':
        parser.error('--follow-host requires --layout headless')
    for name in [args.workspace, args.bot_workspace]:
        if not name.isdecimal():
            parser.error('workspace names must be numbers')
    binary = ROOT / ('build-debug' if os.environ.get('GAUCHE_PRESET') == 'dev' else 'build-release') / 'gauche'
    outputs = []
    try:
        outputs = [o for o in i3() if o.get('active')]
    except (FileNotFoundError, subprocess.CalledProcessError, json.JSONDecodeError):
        print('i3 unavailable; games will use ordinary desktop window placement.')
    primary = next((o for o in outputs if o['name'] == args.output), None) if args.output else next((o for o in outputs if o.get('primary')), None)
    primary = primary or (outputs[0] if outputs else None)
    secondary = next((o for o in outputs if o['name'] == args.bot_output), None) if args.bot_output else next((o for o in outputs if o != primary), primary)
    human = not args.join and not args.follow_host
    print(f'Layout: {args.layout}; human: {human}; bots: {args.bots}; lifetime: {args.seconds}s')
    print(f'Primary: {primary["name"] if primary else "desktop"}; workspace {args.workspace}')
    if args.layout == 'split':
        print(f'Bots: {secondary["name"] if secondary else "desktop"}; workspace {args.bot_workspace}')
    if human:
        print('Human video: ' + ' '.join(window_options('human', args.layout, primary)))
    if args.dry_run:
        return 0
    if not args.no_build:
        subprocess.run([str(ROOT / 'scripts/build.sh')], cwd=ROOT, check=True)
    if not binary.exists():
        parser.error(f'binary missing: {binary}')
    base = Path(os.environ.get('XDG_STATE_HOME', str(Path.home()/'.local/state'))) / 'gauche/multiplayer'
    base.mkdir(parents=True, exist_ok=True)
    run = Path(tempfile.mkdtemp(prefix='session-', dir=base))
    print(f'Profiles and logs: {run}', flush=True)
    children, logs, titles = [], [], {}
    token = run.name
    common = ['--seconds', str(args.seconds), '--room-service', args.service]
    if args.force_relay:
        common += ['--force-relay']

    def launch(role, extra, headless=False):
        profile = run / role
        profile.mkdir()
        env = os.environ.copy()
        # SDL_GetPrefPath and Gubsy both stay inside this process's XDG data directory.
        env['XDG_DATA_HOME'] = str(profile / 'data')
        env['XDG_CONFIG_HOME'] = str(profile / 'config')
        if not headless and env.get('DISPLAY'):
            env['SDL_VIDEODRIVER'] = 'x11'
        if role != 'human':
            env['SDL_AUDIODRIVER'] = 'dummy'
        title = f'Gauche {token} {role}'
        command = [str(binary), *common, *extra]
        if headless:
            command += ['--headless']
        else:
            command += ['--window-title', title, *window_options(role, args.layout, primary)]
            if args.profile:
                command += ['--profile-csv', str(profile / 'profile.csv')]
        log = open(run / f'{role}.log', 'w')
        logs.append(log)
        child = subprocess.Popen(command, cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
        children.append(child)
        if not headless:
            titles[title] = role
        return child

    try:
        host = None
        code = args.join
        if human:
            code_path = run / 'room-code'
            host = launch('human', ['--host-room', f'Local multiplayer {token}', '--player-name', args.name,
                                   '--room-file', str(code_path), '--auto-start', str(args.bots + 1),
                                   '--auto-restart', '--death', 'entrance', '--max-players', str(capacity)])
            until = time.monotonic() + 25
            while time.monotonic() < until and not code_path.exists() and host.poll() is None:
                time.sleep(.1)
            code = code_path.read_text().strip() if code_path.exists() else ''
            if len(code) != 6:
                print((run/'human.log').read_text()[-3000:], file=sys.stderr)
                raise RuntimeError('Host did not publish a room; logs are preserved above.')
            print(f'Room: {code}. The human stays under your control; bots ready automatically.', flush=True)
        for index in range(args.bots):
            join = ['--follow-host', args.follow_host] if args.follow_host else ['--join-room', code]
            launch(f'bot-{index+1}', [*join, '--player-name', f'Bot {index+1}', '--bot', '--bot-seed', str(101 + index)], args.layout == 'headless')
        if outputs and titles:
            arrange(titles, args, primary, secondary, human, run / 'layout.json')
        print('Ctrl-C stops this session only. Closing the human game also stops its bots.', flush=True)
        until = time.monotonic() + args.seconds + 5
        while time.monotonic() < until and any(p.poll() is None for p in children):
            if host and host.poll() is not None:
                break
            time.sleep(.25)
    except KeyboardInterrupt:
        pass
    finally:
        stop_children(children)
        for log in logs:
            log.close()
    print(f'Logs retained: {run}')
    return 0


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (RuntimeError, subprocess.CalledProcessError) as error:
        print(error, file=sys.stderr)
        raise SystemExit(1)

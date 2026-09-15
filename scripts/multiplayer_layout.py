"""Place only this launcher's windows; verify i3's actual rectangles."""
import json
import subprocess
import time


def i3(message=None):
    args = ['i3-msg', '-t', 'get_outputs'] if message is None else ['i3-msg', message]
    result = json.loads(subprocess.check_output(args, text=True))
    if message is not None:
        for reply in result:
            if not reply.get('success'):
                raise RuntimeError(f'i3 rejected {message!r}: {reply.get("error", reply)}')
    return result


def windows():
    tree = json.loads(subprocess.check_output(['i3-msg', '-t', 'get_tree'], text=True))
    result = []
    def walk(node, workspace=None):
        if node.get('type') == 'workspace':
            workspace = node['name']
        if node.get('window'):
            node['workspace_name'] = workspace
            result.append(node)
        for child in node.get('nodes', []) + node.get('floating_nodes', []):
            walk(child, workspace)
    walk(tree)
    return result


def arrange(titles, args, primary, secondary, human, report_path):
    deadline = time.monotonic() + 25
    matched = {}
    while time.monotonic() < deadline:
        matched = {n.get('name'): n for n in windows() if n.get('name') in titles}
        if len(matched) == len(titles): break
        time.sleep(.2)
    if len(matched) != len(titles):
        report_path.write_text(json.dumps({'expected_titles': list(titles), 'found_titles': list(matched)}, indent=2))
        raise RuntimeError(f'Game windows did not appear; see {report_path}')
    # Populate before switching: i3 deletes empty workspaces as soon as focus leaves.
    for title, node in matched.items():
        workspace = args.bot_workspace if args.layout == 'split' and titles[title] != 'human' else args.workspace
        i3(f'[con_id={node["id"]}] move container to workspace number {workspace}')
    assignments = [(args.workspace, primary)]
    if args.layout == 'split':
        assignments.append((args.bot_workspace, secondary))
    for workspace, output in assignments:
        i3(f'workspace number {workspace}')
        i3('move workspace to output ' + json.dumps(output['name']))
    # Workspace rectangles exclude bars/docks, unlike output rectangles.
    workspaces = json.loads(subprocess.check_output(['i3-msg', '-t', 'get_workspaces'], text=True))
    available = {w['name']: w['rect'] for w in workspaces}
    expected = {}
    for title, role in titles.items():
        split_bot = args.layout == 'split' and role != 'human'
        workspace = args.bot_workspace if split_bot else args.workspace
        rect = available[workspace]
        x, y, w, h = (rect[k] for k in ('x', 'y', 'width', 'height'))
        if args.layout == 'quad':
            index = 0 if role == 'human' else int(role.split('-')[1]) - (0 if human else 1)
            w //= 2; h //= 2; x += (index % 2)*w; y += (index // 2)*h
        elif split_bot:
            index = int(role.split('-')[1])-1
            top, bottom = h*index//args.bots, h*(index+1)//args.bots
            y += top; h = bottom-top
        expected[title] = {'workspace': workspace, 'rect': dict(x=x, y=y, width=w, height=h)}
    stable_since = None
    last = {}
    deadline = time.monotonic() + 25
    while time.monotonic() < deadline:
        seen = {}
        all_correct = True
        for node in windows():
            title = node.get('name', '')
            if title not in expected:
                continue
            goal = expected[title]
            seen[title] = dict(rect=node['rect'], floating=node.get('floating'), workspace=node['workspace_name'])
            correct = (node.get('floating') in ('user_on', 'auto_on') and
                       node['rect'] == goal['rect'] and node['workspace_name'] == goal['workspace'] and
                       node.get('fullscreen_mode', 0) == 0)
            if correct:
                continue
            all_correct = False
            target = f'[con_id={node["id"]}]'
            r = goal['rect']
            for command in ['fullscreen disable', 'floating enable', 'border pixel 0',
                            f'move container to workspace number {goal["workspace"]}',
                            f'resize set width {r["width"]} px height {r["height"]} px',
                            f'move position {r["x"]} px {r["y"]} px']:
                i3(f'{target} {command}')
        last = seen
        if all_correct and len(seen) == len(expected):
            stable_since = stable_since or time.monotonic()
            if time.monotonic() - stable_since >= 2:
                break
        else:
            stable_since = None
        time.sleep(.2)
    report_path.write_text(json.dumps(dict(expected=expected, actual=last), indent=2)+'\n')
    if stable_since is None:
        raise RuntimeError(f'Window placement did not settle; see {report_path}')
    print(f'Window placement verified: {report_path}', flush=True)
    i3(f'workspace number {args.workspace}')
    for node in windows():
        if titles.get(node.get('name')) == 'human':
            i3(f'[con_id={node["id"]}] focus')

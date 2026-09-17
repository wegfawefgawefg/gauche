#!/usr/bin/env python3
"""Content-addressed, bounded asset bundles for static Pages hosting."""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'build-web/site'
if OUT.exists():
    shutil.rmtree(OUT)
OUT.mkdir(parents=True, exist_ok=True)
for source in (ROOT / 'web').iterdir():
    if source.is_file():
        shutil.copy2(source, OUT / source.name)


def write_hashed(name, data):
    path = Path(name)
    name = f'{path.stem}.{hashlib.sha256(data).hexdigest()[:16]}{path.suffix}'
    (OUT / name).write_bytes(data)
    return name


wasm = write_hashed('teeming.wasm', (ROOT/'build-web/teeming.wasm').read_bytes())
js = (ROOT/'build-web/teeming.js').read_text().replace('teeming.wasm', wasm)
module = write_hashed('teeming.js', js.encode())
files = []
for directory, virtual in [(ROOT/'assets', '/teeming/assets'),
                           (ROOT/'build-web/_deps/gubsy-src/src/assets', '/teeming/assets/gubsy-engine')]:
    for path in sorted(directory.rglob('*')):
        if not path.is_file():
            continue
        rel = path.relative_to(directory)
        if rel.parts[0] == 'music' and str(rel) not in ('music/title.ogg', 'music/playing.ogg'):
            continue
        files.append((virtual+'/'+str(rel), path))
chunks, entries, data = [], [], bytearray()


def flush():
    if not data:
        return
    chunks.append({'file': write_hashed('assets.bin', data), 'entries': list(entries), 'bytes':len(data)})
    data.clear()
    entries.clear()


for virtual, source in files:
    content = source.read_bytes()
    if len(data)+len(content) > 8*1024*1024:
        flush()
    entries.append([virtual, len(data), len(content)])
    data.extend(content)
flush()
manifest = {'module':module, 'wasm':wasm, 'assets':chunks,
            'revision':subprocess.check_output(['git','rev-parse','--short','HEAD'],cwd=ROOT,text=True).strip()}
(OUT/'manifest.json').write_text(json.dumps(manifest,separators=(',',':')))
for path in OUT.iterdir():
    if path.is_file() and path.stat().st_size > 25*1024*1024:
        raise SystemExit(f'Pages asset limit exceeded: {path.name}')
print(f'Packaged {len(files)} assets in {len(chunks)} bundles; Wasm {(OUT/wasm).stat().st_size/1024/1024:.1f} MiB')

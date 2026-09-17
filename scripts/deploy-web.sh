#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"
if [[ -n "$(git status --porcelain)" ]]; then
    echo 'Commit the reviewed build before deploying.' >&2
    exit 1
fi
# Use Cloudflare credentials supplied to the shell or Wrangler's existing login.
# Build/test first: npm ci; scripts/build-web.sh; npm run test:web.
revision="$(git rev-parse HEAD)"
python3 - "$revision" <<'PY'
import json,sys
from pathlib import Path
manifest=json.loads(Path('build-web/site/manifest.json').read_text())
if not sys.argv[1].startswith(manifest['revision']):
    raise SystemExit('Browser package is from a different commit; rebuild first.')
PY
npx --yes wrangler@4.110.0 pages deploy build-web/site --project-name teeming --branch main \
    --commit-hash "$revision" --commit-message "$(git log -1 --format=%s)" --commit-dirty=false

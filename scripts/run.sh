#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
preset="${GAUCHE_PRESET:-release}"

"$repo_root/scripts/build.sh"

case "$preset" in
    dev) binary="$repo_root/build-debug/gauche" ;;
    release) binary="$repo_root/build-release/gauche" ;;
    *) echo "Unknown Gauche preset: $preset" >&2; exit 2 ;;
esac

case "${OS:-}:$(uname -s)" in
    Windows_NT:*|*:MINGW*|*:MSYS*|*:CYGWIN*) binary="${binary}.exe" ;;
esac

if [[ -n "${DISPLAY:-}" && -z "${SDL_VIDEODRIVER:-}" ]]; then
    export SDL_VIDEODRIVER=x11
fi

exec "$binary" "$@"

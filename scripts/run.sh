#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
preset="${TEEMING_PRESET:-${GAUCHE_PRESET:-release}}"

"$repo_root/scripts/build.sh"

case "$preset" in
    dev) binary="$repo_root/build-debug/teeming" ;;
    release) binary="$repo_root/build-release/teeming" ;;
    *) echo "Unknown Teeming preset: $preset" >&2; exit 2 ;;
esac

case "${OS:-}:$(uname -s)" in
    Windows_NT:*|*:MINGW*|*:MSYS*|*:CYGWIN*) binary="${binary}.exe" ;;
esac

if [[ -n "${DISPLAY:-}" && -z "${SDL_VIDEODRIVER:-}" ]]; then
    export SDL_VIDEODRIVER=x11
fi

exec "$binary" "$@"

#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
sdk_root="${EMSDK:-$repo_root/../native-gb/native-gb-web/.cache/emsdk}"
if [[ ! -f "$sdk_root/emsdk_env.sh" ]]; then
    echo "Set EMSDK to an installed Emscripten 6.0.3 SDK." >&2
    exit 1
fi
source "$sdk_root/emsdk_env.sh" >/dev/null
emcmake cmake -S "$repo_root" -B "$repo_root/build-web" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGAUCHE_WARN_AS_ERROR=OFF
cmake --build "$repo_root/build-web" --target gauche --parallel "${TEEMING_BUILD_JOBS:-8}"
python3 "$repo_root/scripts/package-web.py"

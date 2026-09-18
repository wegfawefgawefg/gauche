#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$repo_root/scripts/build-jobs.sh"
teeming_configure_build_jobs
sdk_root="${EMSDK:-$repo_root/../native-gb/native-gb-web/.cache/emsdk}"
if [[ ! -f "$sdk_root/emsdk_env.sh" ]]; then
    echo "Set EMSDK to an installed Emscripten 6.0.3 SDK." >&2
    exit 1
fi
source "$sdk_root/emsdk_env.sh" >/dev/null
emcmake cmake -S "$repo_root" -B "$repo_root/build-web" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGAUCHE_WARN_AS_ERROR=OFF
cmake --build "$repo_root/build-web" --target gauche --parallel "$CMAKE_BUILD_PARALLEL_LEVEL"
python3 "$repo_root/scripts/package-web.py"

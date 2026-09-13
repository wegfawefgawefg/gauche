#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
preset="${GAUCHE_PRESET:-release}"

case "$preset" in
    dev) build_dir="$repo_root/build-debug"; build_type=Debug ;;
    release) build_dir="$repo_root/build-release"; build_type=Release ;;
    *) echo "Unknown Gauche preset: $preset" >&2; exit 2 ;;
esac

cmake -S "$repo_root" -B "$build_dir" -DCMAKE_BUILD_TYPE="$build_type"
cmake --build "$build_dir" --target gauche --parallel

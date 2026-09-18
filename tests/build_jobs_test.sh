#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$repo_root/scripts/build-jobs.sh"
check() {
    local expected="$1"; shift
    local actual
    actual="$(teeming_choose_build_jobs "$@")"
    [[ "$actual" == "$expected" ]] || { echo "Expected $expected workers, got $actual for $*" >&2; exit 1; }
}
# CPU threads, total KiB, currently available KiB.
check 1 16 16777216 4194304  # Busy 16 GiB desktop: do not use all threads.
check 4 16 16777216 12582912 # Idle 16 GiB desktop: RAM is the constraint.
check 2 4 67108864 50331648  # Plenty of RAM: leave CPU capacity for the desktop.
check 1 8 8388608 3145728
check 1 32 0 0             # Unknown memory must not mean unlimited.
check 1 1 67108864 50331648
check 16 32 67108864 50331648
(
    export CMAKE_BUILD_PARALLEL_LEVEL=3 TEEMING_BUILD_JOBS=2
    teeming_configure_build_jobs
    [[ "$CMAKE_BUILD_PARALLEL_LEVEL" == 2 ]]
    unset TEEMING_BUILD_JOBS
    teeming_configure_build_jobs
    [[ "$CMAKE_BUILD_PARALLEL_LEVEL" == 2 ]]
)
for invalid in 0 -1 unlimited 2.5; do
    if (export TEEMING_BUILD_JOBS="$invalid"; teeming_configure_build_jobs) 2>/dev/null; then
        echo "Accepted invalid worker count: $invalid" >&2; exit 1
    fi
done
# Intercept CMake rather than starting compilers: check both configure inheritance
# and the explicit build argument, even when launched from inside scripts/.
probe_dir="$(mktemp -d)"
trap 'rm -rf "$probe_dir"' EXIT
cat > "$probe_dir/cmake" <<'FAKE'
#!/usr/bin/env bash
printf 'jobs=%s %s\n' "${CMAKE_BUILD_PARALLEL_LEVEL:-unset}" "$*" >> "$BUILD_JOBS_TEST_LOG"
FAKE
chmod +x "$probe_dir/cmake"
export BUILD_JOBS_TEST_LOG="$probe_dir/cmake.log"
(cd "$repo_root/scripts"; PATH="$probe_dir:$PATH" TEEMING_BUILD_JOBS=3 ./build.sh)
[[ "$(wc -l < "$BUILD_JOBS_TEST_LOG")" == 2 ]]
rg -q '^jobs=3 -S ' "$BUILD_JOBS_TEST_LOG"
rg -q '^jobs=3 --build .* --parallel 3$' "$BUILD_JOBS_TEST_LOG"
echo 'PASS RAM/CPU budget, low-memory fallback, overrides, and bounded CMake invocation'

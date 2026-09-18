#!/usr/bin/env bash
# Shared native/web build budget. Source this before configuring dependencies.

teeming_choose_build_jobs() {
    local cpus="$1" total_kib="$2" available_kib="$3"
    local cpu_jobs=$(( (cpus + 1) / 2 ))
    local reserve_kib=$(( total_kib / 4 ))
    local worker_kib=2097152 # 2 GiB per compiler; an estimate, not a memory limit.
    (( reserve_kib < 2097152 )) && reserve_kib=2097152
    local memory_jobs=$(( (available_kib - reserve_kib) / worker_kib ))
    (( memory_jobs < 1 )) && memory_jobs=1
    (( cpu_jobs < 1 )) && cpu_jobs=1
    if (( memory_jobs < cpu_jobs )); then
        printf '%s\n' "$memory_jobs"
    else
        printf '%s\n' "$cpu_jobs"
    fi
}

teeming_configure_build_jobs() {
    local override="${TEEMING_BUILD_JOBS:-${CMAKE_BUILD_PARALLEL_LEVEL:-}}"
    if [[ -n "$override" ]]; then
        if [[ ! "$override" =~ ^[1-9][0-9]{0,3}$ ]]; then
            echo 'Build workers must be a positive integer (1–9999); use TEEMING_BUILD_JOBS=1 for a serial build.' >&2
            return 2
        fi
        export CMAKE_BUILD_PARALLEL_LEVEL="$override"
        echo "Build workers: $override (explicit override)" >&2
        return
    fi

    local cpus=1 total_kib=0 available_kib=0
    if command -v nproc >/dev/null 2>&1; then
        cpus="$(nproc)"
    elif command -v getconf >/dev/null 2>&1; then
        cpus="$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)"
    fi
    [[ "$cpus" =~ ^[1-9][0-9]*$ ]] || cpus=1
    if [[ -r /proc/meminfo ]]; then
        read -r total_kib available_kib < <(awk '
            /^MemTotal:/ {total=$2}
            /^MemAvailable:/ {available=$2}
            END {printf "%.0f %.0f\n",total,available}' /proc/meminfo)
    fi
    # Unknown memory availability deliberately falls back to a single worker.
    export CMAKE_BUILD_PARALLEL_LEVEL
    CMAKE_BUILD_PARALLEL_LEVEL="$(teeming_choose_build_jobs "$cpus" "$total_kib" "$available_kib")"
    echo "Build workers: $CMAKE_BUILD_PARALLEL_LEVEL (auto; $cpus CPU threads, $((available_kib / 1024)) MiB RAM available)" >&2
}

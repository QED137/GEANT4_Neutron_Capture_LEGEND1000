#!/usr/bin/env bash
# run_docker.sh -- build the project and run the water-vs-Gd comparison
# entirely inside the Geant4 container (no local Geant4 needed).
#
# usage (from the repo root):
#   ./build.sh               # one-time image build (~30 min)
#   ./run_docker.sh          # builds the app + runs both macros + summary
#
# Optional GUI (interactive vis.mac) -- see README "Running in Docker".
set -euo pipefail

IMAGE=g4gd:11.2.2
PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

exec docker run --rm -it \
  --mount "type=bind,source=$PROJECT_DIR,destination=$PROJECT_DIR" \
  -e PROJECT_DIR="$PROJECT_DIR" \
  -w "$PROJECT_DIR" \
  "$IMAGE" bash -c '
    set -e
    CACHE="$PROJECT_DIR/build/CMakeCache.txt"
    if [ -f "$CACHE" ]; then
      CMAKE_SOURCE_DIR="$(sed -n "s#^CMAKE_HOME_DIRECTORY:INTERNAL=##p" "$CACHE")"
      if [ "$CMAKE_SOURCE_DIR" != "$PROJECT_DIR" ]; then
        echo "Removing stale CMake cache from: $CMAKE_SOURCE_DIR"
        rm -f "$CACHE"
        rm -rf "$PROJECT_DIR/build/CMakeFiles"
      fi
    fi
    cmake -S "$PROJECT_DIR" -B "$PROJECT_DIR/build"
    cmake --build "$PROJECT_DIR/build" --parallel "$(nproc)"
    "$PROJECT_DIR/build/neu" "$PROJECT_DIR/build/macros/run_water.mac" > "$PROJECT_DIR/log_water.txt" 2>&1
    "$PROJECT_DIR/build/neu" "$PROJECT_DIR/build/macros/run_gd.mac"    > "$PROJECT_DIR/log_gd.txt"    2>&1
    grep -A2 "Neutron captures" "$PROJECT_DIR/log_water.txt" "$PROJECT_DIR/log_gd.txt"
  '

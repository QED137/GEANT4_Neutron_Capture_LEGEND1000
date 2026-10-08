#!/usr/bin/env bash
# Run concentration and neutron-energy scans in the Geant4 Docker image.
set -euo pipefail

IMAGE=g4gd:11.2.2
PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
EVENTS="${1:-10000}"

# Keep host-visible build and scan outputs owned by the invoking user.
docker run --rm \
  --mount "type=bind,source=$PROJECT_DIR,destination=$PROJECT_DIR" \
  "$IMAGE" chown -R "$(id -u):$(id -g)" "$PROJECT_DIR/build"

exec docker run --rm -i \
  --user "$(id -u):$(id -g)" \
  --mount "type=bind,source=$PROJECT_DIR,destination=$PROJECT_DIR" \
  -e PROJECT_DIR="$PROJECT_DIR" \
  -w "$PROJECT_DIR" \
  "$IMAGE" bash -lc '
    set -e
    cmake -S "$PROJECT_DIR" -B "$PROJECT_DIR/build"
    cmake --build "$PROJECT_DIR/build" --parallel "$(nproc)"
    python3 "$PROJECT_DIR/scripts/run_scans.py" \
      "$PROJECT_DIR/build/neu" --events "'"$EVENTS"'" \
      --output "$PROJECT_DIR/scan_results"
  '

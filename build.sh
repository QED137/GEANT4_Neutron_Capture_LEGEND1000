#!/usr/bin/env bash
# build.sh -- one-time build of the Geant4 image (needs ~10 GB free disk,
# internet access, and 20-40 minutes depending on CPU).
set -euo pipefail
PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
docker build -t g4gd:11.2.2 "$PROJECT_DIR"

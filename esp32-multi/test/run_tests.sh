#!/usr/bin/env bash
# Build and run the host-side unit tests for the scooter_control library.
# No PlatformIO / hardware required — just a C++ compiler.
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(dirname "$here")"
lib="$root/lib/scooter_control"
out="$(mktemp -d)"
trap 'rm -rf "$out"' EXIT

CXX="${CXX:-g++}"
"$CXX" -std=c++14 -Wall -Wextra -Werror -O2 \
  -I "$lib" \
  "$here/test_scooter/test_main.cpp" \
  "$lib/scooter_protocol.cpp" \
  "$lib/scooter_controller.cpp" \
  -o "$out/test_scooter"

"$out/test_scooter"

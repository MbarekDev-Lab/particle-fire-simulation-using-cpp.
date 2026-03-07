#!/usr/bin/env bash
# ============================================================================
# run_valgrind.sh — Run the smoke test under Valgrind (optional)
#
# Usage:  ./tools/run_valgrind.sh
#
# Requires: valgrind (Linux only — not available on macOS ARM)
#           sudo apt install valgrind   # Debian/Ubuntu
#
# This is optional — the smoke test already validates RAII cleanup,
# but Valgrind provides deeper leak detection.
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

if ! command -v valgrind &>/dev/null; then
    echo "Valgrind not found. Install with:"
    echo "  sudo apt install valgrind   # Debian/Ubuntu"
    echo "  brew install valgrind       # macOS (Intel only)"
    echo ""
    echo "Note: Valgrind is not available on Apple Silicon (M1/M2/M3)."
    echo "Skipping — this is expected on macOS ARM."
    exit 0
fi

# Build first
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR" --target smoke-test

mkdir -p "$BUILD_DIR/out"
cd "$BUILD_DIR"

echo "=== Running smoke-test under Valgrind ==="
valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --error-exitcode=1 \
    ./smoke-test

echo "=== Valgrind: no leaks detected ==="

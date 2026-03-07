#!/usr/bin/env bash
# ============================================================================
# run_smoke_test.sh — Build and run the headless smoke test
#
# Usage:  ./tools/run_smoke_test.sh
# Exit:   0 on success, non-zero on failure
#
# This script:
#   1. Creates a build directory (if needed)
#   2. Configures CMake
#   3. Builds the smoke-test target
#   4. Creates the out/ directory for PPM output
#   5. Runs the smoke test
#   6. Verifies out/frame0.ppm was created
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

echo "=== Particle Fire Smoke Test Runner ==="
echo "Project: $PROJECT_DIR"
echo ""

# Step 1: Configure
echo "--- Configuring CMake ---"
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug

# Step 2: Build smoke-test target
echo ""
echo "--- Building smoke-test ---"
cmake --build "$BUILD_DIR" --target smoke-test -- -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

# Step 3: Create output directory
mkdir -p "$BUILD_DIR/out"

# Step 4: Run the test
echo ""
echo "--- Running smoke-test ---"
cd "$BUILD_DIR"
./smoke-test

# Step 5: Verify output file exists
if [[ -f "$BUILD_DIR/out/frame0.ppm" ]]; then
    FILE_SIZE=$(wc -c < "$BUILD_DIR/out/frame0.ppm" | tr -d ' ')
    echo ""
    echo "--- Output file: out/frame0.ppm ($FILE_SIZE bytes) ---"
    echo "=== SMOKE TEST PASSED ==="
    exit 0
else
    echo ""
    echo "ERROR: out/frame0.ppm was not created"
    exit 1
fi

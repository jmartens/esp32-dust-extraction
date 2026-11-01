#!/usr/bin/env bash
# Builds the Linux host tests with gcov instrumentation, runs them and writes
# coverage reports to firmware/test/coverage. The exit code is the test result;
# report generation never masks a test failure.
set -uo pipefail

cd "$(dirname "$0")" || exit 1
repo_root="$(cd ../.. && pwd)" || exit 1
out_dir="$PWD/coverage"
# BUILD_DIR lets the Docker wrapper keep build output out of the source tree.
build_dir="${BUILD_DIR:-build}"

idf.py -B "$build_dir" --preview set-target linux || exit 1
idf.py -B "$build_dir" -DOPEN_EXTRACTION_COVERAGE=ON build || exit 1

python -m pip install --quiet gcovr || exit 1

find "$build_dir" -name '*.gcda' -delete
rm -rf "$out_dir"
mkdir -p "$out_dir"

"$build_dir/open_extraction_tests.elf"
test_status=$?

# Production sources only: exclude test code and generated/build output.
python -m gcovr --root "$repo_root" --object-directory "$build_dir" \
    --filter "$repo_root/firmware/components/" \
    --exclude '.*/test/.*' \
    --exclude '.*/build/.*' \
    --print-summary \
    --xml "$out_dir/coverage.xml" \
    --html-details "$out_dir/index.html" \
    --txt "$out_dir/summary.txt"
report_status=$?

if [ "$test_status" -ne 0 ]; then
    echo "Host unit tests failed (exit $test_status)" >&2
    exit "$test_status"
fi
exit "$report_status"

#!/usr/bin/env bash
# Thin wrapper -- see scripts/package_data_release.py for the actual logic
# (moved to Python: grouping file_inventory.txt entries by directory needs
# more than bash 3.2's associative-array-free toolbox comfortably gives us).
#
# Usage: scripts/package_data_release.sh <release-tag>
set -euo pipefail
exec python3 "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/package_data_release.py" "$@"

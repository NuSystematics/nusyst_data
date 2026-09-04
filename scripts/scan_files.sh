#!/usr/bin/env bash
# Thin wrapper -- see scripts/scan_files.py.
#
# Usage: scripts/scan_files.sh
set -euo pipefail
exec python3 "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/scan_files.py" "$@"

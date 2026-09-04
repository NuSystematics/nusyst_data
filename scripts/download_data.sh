#!/usr/bin/env bash
#
# Downloads, checksum-verifies, and extracts this repo's ROOT data files from
# the GitHub Release described in manifest.json. This is the same thing the
# nusystematics CMake install step does automatically when
# nusyst_DOWNLOAD_DATA=ON - this script exists for anyone who wants the data
# on its own, without building nusystematics.
#
# Usage:
#   scripts/download_data.sh [-o DIR] [-d DEST]
#
#   -o DIR    Only fetch the asset(s) for one directory (matches "extract_to"
#             in manifest.json, e.g. QEFix, CCMEC_template, or "." for the
#             loose top-level files). Omit to fetch everything.
#   -d DEST   Destination root to extract into (default: the repo root next
#             to manifest.json, i.e. back into this checkout's own directories).
#
# Requires: curl, shasum, tar, python3 (stdlib only).

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MANIFEST="$REPO_ROOT/manifest.json"
REPO="NuSystematics/nusyst_data"

ONLY_DIR=""
DEST="$REPO_ROOT"
while getopts "o:d:h" opt; do
	case "$opt" in
	o) ONLY_DIR="$OPTARG" ;;
	d) DEST="$OPTARG" ;;
	h)
		grep '^#' "$0" | sed '1d;s/^# \{0,1\}//'
		exit 0
		;;
	*)
		exit 1
		;;
	esac
done

[ -f "$MANIFEST" ] || {
	echo "manifest.json not found at $MANIFEST" >&2
	exit 1
}

sha256_of() { shasum -a 256 "$1" | awk '{print $1}'; }

RELEASE_TAG="$(python3 -c "import json; print(json.load(open('$MANIFEST'))['releaseTag'])")"
echo "Release: $RELEASE_TAG"
[ -n "$ONLY_DIR" ] && echo "Filtering to: $ONLY_DIR"
echo "Destination: $DEST"
echo

# Emit "file<TAB>extract_to<TAB>sha256" lines, one per matching asset.
python3 -c '
import json, sys
m = json.load(open(sys.argv[1]))
only = sys.argv[2]
for a in m["assets"]:
    if only and a["extract_to"] != only:
        continue
    print(a["file"] + "\t" + a["extract_to"] + "\t" + a["sha256"])
' "$MANIFEST" "$ONLY_DIR" |
while IFS=$'\t' read -r file extract_to sha256; do
	url="https://github.com/${REPO}/releases/download/${RELEASE_TAG}/${file}"
	tmp="$(mktemp)"
	trap 'rm -f "$tmp"' EXIT

	echo "==> $file -> $DEST/$extract_to"
	curl -fL --progress-bar -o "$tmp" "$url"

	got_sha="$(sha256_of "$tmp")"
	if [ "$got_sha" != "$sha256" ]; then
		echo "checksum mismatch for $file: expected $sha256, got $got_sha" >&2
		rm -f "$tmp"
		exit 1
	fi

	mkdir -p "$DEST/$extract_to"
	tar -xzf "$tmp" -C "$DEST/$extract_to"
	rm -f "$tmp"
done

echo
echo "Done."

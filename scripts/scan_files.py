#!/usr/bin/env python3
"""Scan every file in the repo and (re)write file_inventory.txt: a plain-text
list of "<path> <Y|N>" pairs deciding whether each file is committed to git
directly (Y) or bundled into a release tarball by
scripts/package_data_release.py (N).

Default suggestion: N for *.root files, Y for everything else. Re-running
this preserves the Y/N choice already recorded for any file that still
exists -- it only adds newly-seen files (with the suggested default) and
drops entries for files that no longer exist on disk. Edit
file_inventory.txt by hand afterward to override any suggestion (e.g. to
route a large .csv/.h5 into the release too, or to keep some small .root
file directly in git), then run:

    scripts/package_data_release.py <release-tag>

Usage: scripts/scan_files.py
"""
import os
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INVENTORY = os.path.join(REPO_ROOT, "file_inventory.txt")

EXCLUDE_DIRS = {".git", "dist", "__pycache__"}
EXCLUDE_FILES = {"file_inventory.txt", ".DS_Store"}

HEADER = [
    '# file_inventory.txt -- one line per file, "<path> <Y|N>".',
    "#   Y = commit this file to git directly.",
    "#   N = bundle this file into a release tarball",
    "#       (see scripts/package_data_release.py).",
    "#",
    "# Regenerate/sync with scripts/scan_files.py: it preserves the Y/N you've",
    "# already set for files that still exist, and only adds/drops entries for",
    "# files that appeared/disappeared on disk. Edit the flags by hand.",
    "",
]


def default_flag(path):
    return "N" if path.endswith(".root") else "Y"


def scan():
    paths = []
    for root, dirs, files in os.walk(REPO_ROOT):
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        for f in files:
            if f in EXCLUDE_FILES:
                continue
            full = os.path.join(root, f)
            rel = os.path.relpath(full, REPO_ROOT)
            paths.append(rel)
    return sorted(paths)


def load_existing():
    existing = {}
    if not os.path.exists(INVENTORY):
        return existing
    with open(INVENTORY) as fh:
        for line in fh:
            line = line.rstrip("\n")
            if not line or line.startswith("#"):
                continue
            path, _, flag = line.rpartition(" ")
            if path:
                existing[path] = flag.strip().upper()
    return existing


def main():
    existing = load_existing()
    current = scan()
    current_set = set(current)

    added = []
    lines = list(HEADER)
    for path in current:
        if path in existing:
            flag = existing[path]
        else:
            flag = default_flag(path)
            added.append((path, flag))
        lines.append("%s %s" % (path, flag))

    removed = [p for p in existing if p not in current_set]

    with open(INVENTORY, "w") as fh:
        fh.write("\n".join(lines) + "\n")

    print("Wrote %s (%d files)" % (INVENTORY, len(current)))
    if added:
        print()
        print("  %d new file(s), added with a suggested default:" % len(added))
        for path, flag in added:
            print("    %s %s" % (path, flag))
    if removed:
        print()
        print("  %d file(s) no longer on disk, dropped from the inventory:" % len(removed))
        for path in removed:
            print("    %s" % path)
    print()
    print("Review/edit file_inventory.txt, then run:")
    print("  scripts/package_data_release.py <release-tag>")


if __name__ == "__main__":
    main()

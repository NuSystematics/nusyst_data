#!/usr/bin/env python3
"""Insert/replace the auto-generated file-list section of a directory's
README.md, between marker comments. Used by package_data_release.sh -- not
meant to be run by hand.

Usage: update_readme_filelist.py <readme_path> <release_tag> <dirname> <file>...
"""
import re
import sys

START = "<!-- nusyst_data:filelist:start -->"
END = "<!-- nusyst_data:filelist:end -->"


def build_block(release_tag, files):
    lines = [
        START,
        "**Files in this release (%s)** — regenerated automatically by "
        "`scripts/package_data_release.sh`; do not edit by hand." % release_tag,
        "",
    ]
    if files:
        lines.append("```")
        lines.extend(files)
        lines.append("```")
    else:
        lines.append("*(none currently — this directory has no files bundled into a release right now.)*")
    lines.append(END)
    return "\n".join(lines)


def main():
    readme_path, release_tag, dirname = sys.argv[1], sys.argv[2], sys.argv[3]
    files = sys.argv[4:]
    block = build_block(release_tag, files)

    try:
        with open(readme_path) as f:
            content = f.read()
    except FileNotFoundError:
        content = "# %s\n\n%s\n" % (dirname, block)
        with open(readme_path, "w") as f:
            f.write(content)
        return

    pattern = re.compile(re.escape(START) + ".*?" + re.escape(END), re.DOTALL)
    if pattern.search(content):
        content = pattern.sub(block, content)
    else:
        content = content.rstrip("\n") + "\n\n" + block + "\n"

    with open(readme_path, "w") as f:
        f.write(content)


if __name__ == "__main__":
    main()

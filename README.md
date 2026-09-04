# nusyst_data

Systematic-reweighting template/data files consumed by
[nusystematics](https://github.com/NuSystematics/nusystematics) at build/install
time.

## Heavy files are not tracked in git

Individual files over GitHub's 100 MB hard push limit, and a git history
that only ever grows since git has no way to drop a superseded version of a
binary blob, is what pushed this repo to move its data files out of git.
Instead, those files are published as tarball assets on a [GitHub
Release](https://github.com/NuSystematics/nusyst_data/releases), and this
repo carries only:

- **[file_inventory.txt](file_inventory.txt)** — every file in the repo,
  one per line, flagged `Y` (committed to git directly) or `N` (bundled into
  a release tarball instead). `scripts/scan_files.py` (re)generates it,
  suggesting `N` for `*.root` and `Y` for everything else; edit the flags by
  hand to override a suggestion — e.g. to route a large `.csv`/`.h5` into
  the release too, or keep some small `.root` file directly in git.
- **[manifest.json](manifest.json)** — which release, and which asset files,
  checksums, and extraction paths make up the current `N` file set.
- **`<directory>/README.md`** in each data directory — what it is, how to
  fetch just that directory's data, and (in an auto-generated section) the
  actual filenames included in that directory's release asset(s). Since
  those files are gitignored (see the auto-generated block at the bottom of
  `.gitignore`), that section is the only place their names are visible
  from the GitHub UI.
- `scripts/scan_files.py` / `scripts/package_data_release.py` /
  `scripts/download_data.sh` — the tooling that builds `file_inventory.txt`,
  builds a release from it, and fetches one, respectively. (`.sh` wrappers
  of the same names call the `.py` ones and are what you actually run.)

## Downloading the data

**If you're building nusystematics** via CMake with `nusyst_DOWNLOAD_DATA=ON`,
this happens automatically at install time — the install step reads
`manifest.json` from this repo and fetches/verifies/extracts each asset.
Nothing to do here.

**To fetch the data on its own** (without building nusystematics), from a
checkout of this repo:

```bash
# everything
scripts/download_data.sh

# just one directory (matches a directory name below, or "." for the
# loose top-level files)
scripts/download_data.sh -o QEFix

# into somewhere other than this checkout
scripts/download_data.sh -d /path/to/dest
```

It downloads each asset listed in `manifest.json`, verifies its sha256, and
extracts it — the same steps the CMake install code performs. Requires
`curl`, `shasum`, `tar`, and `python3` (standard library only).

**By hand**, without the script: `manifest.json` lists, per asset, the
`file` name, the release `releaseTag`, its `sha256`, and where to extract it
(`extract_to`). Each asset lives at:

```
https://github.com/NuSystematics/nusyst_data/releases/download/<releaseTag>/<file>
```

Download it, check its sha256 against `manifest.json`, and untar it into
`<extract_to>`.

## Repository layout

| Directory | Contents | Release-backed data right now? |
|---|---|---|
| [CCMEC_template/](CCMEC_template/) | CC MEC (2p2h) reweighting templates | yes |
| [CCQEReweight/](CCQEReweight/) | CCQE reweighting templates | yes |
| [CCQERPAReweight/](CCQERPAReweight/) | CCQE RPA reweighting templates | yes |
| [FSI/](FSI/) | FSI reweighting templates, plus `CombineMultAndDiff.py` (tracked normally) | yes |
| [PionAbsWeighter/](PionAbsWeighter/) | Pion absorption reweighting templates | yes |
| [QEFix/](QEFix/) | QE fix reweighting templates | yes |
| [QEInterference/](QEInterference/) | QE interference reweighting templates | yes |
| [ValenciaExc2p2hReweighter/](ValenciaExc2p2hReweighter/) | Valencia exclusive 2p2h reweighter config | no — small files, tracked normally in git |
| [weightAMUdistribution/](weightAMUdistribution/) | AMU distribution weighting source/inputs | no — small files, tracked normally in git |

("Release-backed" directories currently have one or more files marked `N`
in [file_inventory.txt](file_inventory.txt), meaning their payload is in the
GitHub Release described by [manifest.json](manifest.json) rather than in
git; see each one's own `README.md`. This is a snapshot, not a fixed
category — which files land where is a per-file decision in
`file_inventory.txt`, and can change any time someone reruns the tooling.)

A handful of files can also live directly at the repo root rather than in a
subdirectory (`extract_to: "."` in `manifest.json`); none currently do, but
`scripts/download_data.sh -o .` would fetch them if some did.

Directories only appear in a checkout once they contain at least one
git-tracked file (`README.md`, or, for a couple of directories, small
non-`.root` source/config files) — git doesn't track empty directories, and
every `N`-flagged file is gitignored.

## Updating the data (maintainers)

```bash
scripts/scan_files.sh                             # (re)generate file_inventory.txt
# review/edit file_inventory.txt's Y/N flags, then:
scripts/package_data_release.sh <release-tag>      # e.g. v01_01_00
gh release create <release-tag> dist/*.tar.gz \
  --repo NuSystematics/nusyst_data \
  --title "<release-tag>" --notes "<release-tag>"
dist/git_add_committed_files.sh                    # stages every "Y" file
git add manifest.json .gitignore file_inventory.txt
git commit -m "Update data release to <release-tag>"
git push
```

`scan_files.sh` walks every file in the repo and (re)writes
`file_inventory.txt`, preserving any Y/N choice you've already made for a
file that still exists and only adding/dropping entries for files that
appeared/disappeared on disk. `package_data_release.sh` then reads that
file: every `N` entry gets grouped by its top-level directory, tarred into
`dist/` (splitting anything over 1.5 GiB into numbered parts to stay under
GitHub's 2 GiB per-asset limit), and reflected in `manifest.json` and that
directory's `README.md`; every `Y` entry is left alone on disk and added to
the generated `dist/git_add_committed_files.sh`. Tarballs are built
byte-for-byte reproducibly, so rerunning with unchanged file content
reproduces the same tarball bytes and sha256 rather than forcing a needless
re-upload. If this adds a brand-new data directory, a minimal `README.md` is
created for it automatically — flesh it out (see an existing directory's
`README.md` for the pattern) and add a row to the table above.

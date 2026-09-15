# CCQEReweight

CCQE reweighting templates.

## The `*.root` files here are not in git

They're marked `N` in [file_inventory.txt](../file_inventory.txt) (bundled
into a release tarball instead of committed) — see
[manifest.json](../manifest.json) at the repo root (`extract_to:
"CCQEReweight"`). The exact list of files currently included is below
(regenerated automatically whenever the release is rebuilt — don't edit that
section by hand).

## Downloading just this directory

From the repo root:

```bash
scripts/download_data.sh -o CCQEReweight
```

This downloads the asset(s) listed in `manifest.json` for this directory,
verifies their sha256, and extracts them here. See the [repo root
README](../README.md) for the full download options (fetching everything,
fetching into a different destination, or doing it by hand with `curl`).

<!-- nusyst_data:filelist:start -->
**Files in this release (v01_00_00)** — regenerated automatically by `scripts/package_data_release.sh`; do not edit by hand.

```
CCQEReweight_templates_all_tunes.root
```
<!-- nusyst_data:filelist:end -->

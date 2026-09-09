# Exploration: release artifact automation

## Current state

The package version remains `0.1` while the stable embedding API and shared
library are `1.6.0`. The repository has no tags, release workflow, release
guide, or explicit version-consistency check. Staged package tests already cover
static/shared consumers and uninstall behavior, but they are not assembled into
a release gate. Automake provides `distcheck`, though its distributable-file
coverage has not been exercised recently.

## Findings

- Matching package and embedding API versions at `1.6.0` gives the first
  automated release an unambiguous identity and keeps shared real filenames
  aligned.
- `make distcheck` is the authoritative source-archive, VPATH build, test,
  install, uninstall, and distclean check; existing package smoke tests add
  external static/shared consumer verification.
- A small script can compare `AC_INIT`, public header macros, runtime version
  source, shared-library filename, and an optional `vX.Y.Z` tag.
- GitHub-hosted runners include `gh`, so a tag workflow can build the checked
  tarball, upload a workflow artifact, and create or update the GitHub Release
  without a third-party release action.

## Decision

Set the project release to 1.6.0, add deterministic version and release gates,
repair distribution manifests until `distcheck` passes, and publish the source
archive plus checksum from `v*` tags.

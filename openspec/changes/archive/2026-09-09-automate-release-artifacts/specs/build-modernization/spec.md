## ADDED Requirements

### Requirement: Source releases are reproducible and checked

The build SHALL produce a versioned source archive through `make distcheck` and
SHALL verify an out-of-tree build, tests, install, uninstall, and distclean from
that archive.

#### Scenario: A distributed source file is missing

- **WHEN** the unpacked archive cannot build or run the regression suite
- **THEN** the release gate fails before publication

### Requirement: Release artifacts are published from tags

GitHub Actions SHALL publish the checked source archive and its SHA-256 checksum
for a tag matching the declared release version.

#### Scenario: A version tag is pushed

- **WHEN** a `vX.Y.Z` tag matches the declared package version
- **THEN** the workflow attaches the archive and checksum to the GitHub Release

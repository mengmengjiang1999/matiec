# Proposal: automate release artifacts

## Why

The installed library now has a stable ABI, but releases cannot be reproduced
or checked from a clean source archive. Version drift would produce misleading
package metadata and shared-library names.

## What changes

- Align the package release, API version, pkg-config metadata, and shared real
  library at 1.6.0.
- Add a version-consistency check with optional tag verification.
- Make `distcheck` and staged install/uninstall/package consumers the release
  acceptance gate.
- Add a tag-triggered GitHub workflow that uploads the source archive and SHA-256
  checksum as workflow and GitHub Release artifacts.
- Add release instructions and update README/status documentation.

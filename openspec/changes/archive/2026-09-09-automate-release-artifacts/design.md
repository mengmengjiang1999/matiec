# Design: reproducible source releases

## Version policy

For this release line, package version and embedding API version are identical.
The checker parses authoritative source files rather than generated output and
also verifies the platform-specific shared real filename. If `RELEASE_TAG` is
set, it must equal `v<package-version>`.

## Local release gate

`make release-check` runs the version checker, `make distcheck`, and the staged
package contract. `distcheck` supplies archive creation, out-of-tree build/test,
install/uninstall, and distclean validation. The package contract additionally
links external consumers against both static and shared installations.

## GitHub workflow

Tags matching `v*` run the same version check and `distcheck` on Ubuntu. The job
generates a SHA-256 checksum, uploads both files using the official artifact
action, then uses the authenticated GitHub CLI to create the release or replace
its artifacts. The workflow has only `contents: write` permission.

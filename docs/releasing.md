# Release process

The package, embedding API, runtime API string, and versioned shared-library
filename use one semantic version. The current release is `1.6.0`.

## Prepare and verify

From a clean checkout with Autotools, Bison, Flex, a C/C++ toolchain, and
pkg-config installed:

```sh
autoreconf --install
./configure
make release-check
```

`release-check` verifies version consistency and runs Automake `distcheck`.
The latter creates the source archive, rebuilds it out of tree, executes the
full regression and API/package consumer suites, checks staged install and
uninstall, and requires a clean distribution tree.

To validate the intended tag locally:

```sh
RELEASE_TAG=v1.6.0 config/check-release-version.sh
```

## Publish

After the release commit is on `main`, create and push the matching annotated
tag (for example `v1.6.0`). The `release-artifacts` workflow reruns `distcheck`,
creates a SHA-256 checksum, stores both as workflow artifacts, and creates or
updates the matching GitHub Release using the repository token.

Do not publish when the version check, `distcheck`, sanitizer matrix, or normal
build workflow is failing. Tag creation is intentionally a maintainer action;
the workflow never invents or pushes a tag.

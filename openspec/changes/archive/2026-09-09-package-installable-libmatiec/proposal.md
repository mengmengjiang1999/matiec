## Why

The public C header and implementation now exist, but external consumers cannot
install and link a single supported library artifact. Packaging must make the
embedding API usable outside the source tree.

## What Changes

- Build and install one self-contained static `libmatiec` archive.
- Install the public C header, compiler IEC library data, and generated-C runtime
  headers.
- Provide `pkg-config` metadata and an install/uninstall smoke test.
- Document supported consumer compilation and runtime data discovery.

## Capabilities

### Modified Capabilities

- `embedding-api`: provide an installable library and consumer metadata.
- `build-modernization`: verify staged installation and external linking.

## Impact

The build gains a portable archive-merging helper, installed data files, and a
generated `matiec.pc`. Existing CLI programs and component archives remain.

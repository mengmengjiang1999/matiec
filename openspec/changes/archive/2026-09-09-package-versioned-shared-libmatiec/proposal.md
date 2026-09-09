## Why

The embedding package is static-only, forcing consumers to absorb the C++
implementation and preventing normal runtime ABI upgrades. The stable C API now
has enough compatibility enforcement to publish a versioned shared library.

## What Changes

- Build and install a versioned `libmatiec` shared library on ELF and Darwin.
- Retain the self-contained static archive for consumers that require it.
- Hide implementation symbols and verify that only the declared C API is exported.
- Exercise installed static and dynamic consumers, including runtime lookup and
  uninstall cleanup.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `embedding-api`: publish the stable C ABI as a platform-versioned shared library.
- `build-modernization`: verify cross-platform shared packaging and consumption.

## Impact

The recursive Automake flags, top-level library packaging, pkg-config metadata,
API/package tests, CI-visible checks, README, and embedding architecture contract
are affected. Existing CLI and static consumers remain supported.

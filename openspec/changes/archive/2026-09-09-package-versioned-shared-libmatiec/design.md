## Context

MATIEC is built from cyclic component archives, including generated parser
objects. The installed archive is flattened by a portable helper. Re-describing
all sources in a new shared target would duplicate the build graph.

## Goals / Non-Goals

**Goals:** retain static linkage, add ELF and Darwin shared artifacts with ABI
major version 1, export only the C API, and verify staged installation.

**Non-Goals:** Windows DLL/import-library support, changing API signatures, or
converting internal archives to public libraries.

## Decisions

Existing component archives will be compiled as PIC with hidden default
visibility. `MATIEC_API` remains the single export annotation. The top-level
build will whole-archive the components into a platform-native shared artifact.

ELF installs `libmatiec.so.1.3.0` with SONAME `libmatiec.so.1`; Darwin installs
`libmatiec.1.3.0.dylib` with install name `libmatiec.1.dylib`. Relative symlinks
provide ABI-major and unversioned linker names. `pkg-config` continues to use
`-lmatiec`, allowing the platform linker to choose shared linkage by default.

The API symbol test will inspect both static and shared artifacts. The staged
package test links one consumer dynamically and another explicitly against the
archive, then checks uninstall removal.

## Risks / Trade-offs

- [Whole-archive exposes internals] -> compile with hidden visibility and compare
  dynamic exports against the API allowlist.
- [Platform linker flags diverge] -> configure explicit ELF/Darwin substitutions
  and run both existing CI hosts.
- [Runtime loader cannot find staged library] -> execute with scoped loader paths.

## Migration Plan

The shared artifact is additive. Consumers already linking the archive can keep
doing so; `pkg-config` consumers will normally select the shared library after
upgrading. Removing the installed package removes all real files and symlinks.

## Open Questions

Windows remains a separate future change.

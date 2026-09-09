## Context

The compiler is split across cyclic static archives, including generated parser
objects and one selected code generator. External callers need a single link
target, while converting the legacy build wholesale to shared libtool objects
would be disproportionately invasive.

## Goals / Non-Goals

**Goals:** a relocatable staged install, one static link artifact, C/C++ header
installation, IEC library data, runtime headers, metadata, and consumer tests.

**Non-Goals:** a shared-library ABI, Windows import libraries, binary releases,
or replacing the existing internal archive layout.

## Decisions

A checked portable shell helper flattens the existing component archives into
`libmatiec.a`, giving the linker one indexed archive with all C-generator
dependencies. The public header installs under `include/matiec` and `matiec.pc`
advertises the library and include paths.

The IEC library text files install under `share/matiec/lib`; generated-C runtime
headers install below its `C` directory. Embedders set that compiler include
directory explicitly, preserving the context API's deterministic configuration.

The smoke test stages `make install`, compiles a C consumer translation unit,
links it with the configured C++ linker and installed archive, executes it, and
checks uninstall cleanup.

## Risks / Trade-offs

Static consumers need a C++ linker because implementation objects use the C++
runtime. A shared-library target is deferred until symbol visibility and release
versioning can be guaranteed across platforms.

## Migration Plan

Source-tree users are unaffected. Installed consumers include `<matiec/api.h>`
and discover flags with `pkg-config matiec`.

## Open Questions

None.

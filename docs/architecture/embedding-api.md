# Versioned embedding API

The stable embedding boundary is the C header `include/matiec/api.h`. It is
valid in C11 and C++ translation units and does not expose STL types, AST nodes,
parser structures, or compiler service layouts.

API version 1 starts with compile-time and runtime version discovery. The packed
numeric form stores major, minor, and patch components so a caller can compare
the header used at build time with the loaded library.

Compatibility follows these rules:

- a major increment may remove symbols, change signatures, or change ownership;
- a minor increment adds backward-compatible functions or enum values;
- a patch increment fixes behavior without intentionally changing the ABI.

Opaque objects added by subsequent changes will be created and destroyed by
matching API functions. Memory remains owned by the side that allocated it
unless a function explicitly documents a borrowed view or transfer.

The existing `matiec::Compiler` and `matiec::CompilationContext` classes
remain supported source-integration interfaces, but their C++ layouts and STL
signatures are not part of the stable binary ABI. The package version reported
by the command-line tools is independent from the embedding API version.

# Exploration: replace namespace spelling bridge

## Current boundary

The grammar and AST already contain namespace, visibility, and USING nodes, but
`Compiler::compile()` first tokenizes and rewrites the complete main source.
That bridge discovers declarations, rewrites qualified and imported references
to length-prefixed identifiers, and performs visibility diagnostics before Flex.
It cannot discover namespace declarations supplied by include resolvers because
those bytes do not exist in the pre-parser input.

## Direction

Use the existing parser prepass as the declaration-discovery pass for the
experimental profile. Grammar actions maintain namespace scope/import state and
register library-element spellings; Flex recognizes qualified identifiers and
asks that context-owned registry for their deterministic internal spelling.
The definitive parse then builds the existing structural namespace AST from the
original source, including virtual and filesystem includes.

## Removal

Delete `namespace_normalizer`, its tests, compiler invocation, and build entries.
Post-parse AST analysis remains authoritative for exported metadata and validates
namespace structure/import targets.

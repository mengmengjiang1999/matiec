## Context

Native method nodes already own their headers, declarations, and bodies, and native
call nodes bind to lowered function calls before stage 3. The remaining declaration
compatibility path is produced by rescanning method text, rewriting identifiers, and
appending synthetic `FUNCTION` source for Bison to parse.

## Goals / Non-Goals

**Goals:** remove appended method source, derive compatibility declarations from the
primary AST, preserve source locations and the current ABI, and keep semantic/C
behavior stable.

**Non-Goals:** direct method-aware type checking or C generation, changing overload
or visibility rules, adding dynamic dispatch, or changing the provisional symbol ABI.

## Decisions

- The normalizer remains a bounded pre-parse validator and metadata provider for this
  increment, but returns the original method source without generated functions.
- A post-parse compatibility pass locates each owner-contained method node, creates a
  function-shaped compatibility AST with a deterministic lowered name, and appends it
  to the library before symbol-table initialization.
- The arena remains the sole node owner. The compatibility function uses a dedicated
  declaration list and non-owning references to native method declaration/body nodes;
  their primary parent links are restored after construction.
- Identifier lowering is structural and limited to the method result name and owner
  fields that are not shadowed by method locals. Tokens retain their source spelling
  so IEC generation can preserve the native method body.
- Compatibility declarations are marked as synthetic so IEC generation omits them;
  C generation and stage 3 continue consuming their established function shape.

## Risks / Trade-offs

- **Shared compatibility views corrupt primary structure** → retain arena ownership
  and primary parent links, and cover native IEC plus sanitizer behavior.
- **Synthetic declarations affect dependency ordering** → append before symbol-table
  initialization and retain the same lowered function ordering expected today.
- **Primary and compatibility trees drift** → derive the compatibility tree only from
  native nodes and validate normalized IEC plus generated-C/runtime behavior together.

## Migration Plan

Create and validate OpenSpec artifacts, add the compatibility-AST construction pass,
remove generated method source, add focused and end-to-end regressions, update docs,
run full and sanitizer validation, archive, commit once, and push to `origin/main`.

Rollback is the single change commit; the external experimental ABI is unchanged.

## Open Questions

None for the existing bounded method subset.

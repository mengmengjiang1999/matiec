## Context

Legacy visitors refer directly to four namespace-scope tables. Passing a new
parameter through every Stage 3 helper and generator in one change would combine
ownership migration with a large interface rewrite.

## Decisions

- Store the four tables in `DeclarationSymbolTables`, owned directly by
  `CompilationContext`.
- Populate the service explicitly from `absyntax_utils_init()`.
- Bind only a pointer to the active service in thread-local storage while legacy
  consumers run. Nested scopes restore the previous pointer.
- Clear tables at the beginning of every compile, including context reuse.
- Keep parser classification tables separate for the generated-frontend change.

## Risks / Trade-offs

Legacy consumers still discover the current table service through an ambient
accessor. The stored entries are nevertheless context-owned and thread-isolated,
so this removes the process-wide data race while allowing explicit consumer
parameters to be introduced incrementally.

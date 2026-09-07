## Context

`library_c`, function, function-block, program, configuration, and resource
declarations carry `enumvalue_symtable_t`. The type is a case-insensitive multimap
because one spelling can identify values from multiple enumerated datatypes.

## Goals / Non-Goals

**Goals:** preserve the exact table semantics in context-owned records, validate
referenced declarations, publish at the completed checker boundary, and retain
legacy behavior.

**Non-Goals:** change ambiguity diagnostics, flatten duplicate entries, or migrate
generator annotations.

## Decisions

- Use `symbol_c::enumvalue_symtable_t` in `EnumerationAnalysisRecord` instead of a
  plain map.
- Visit only the six declaration scopes that own enumeration tables.
- Publish after `enum_declaration_check_c` finishes and materialize immediately.
- Require table targets to belong to the current arena or be immutable shared
  compiler declarations.

## Risks / Trade-offs

Compatibility tables duplicate non-owning entries temporarily. Exact multimap
copying prevents ordering, duplicate, and case-comparison regressions.

## Migration Plan

Implement and test the boundary, run normal and sanitizer suites, validate and
archive the change, then commit and push independently.

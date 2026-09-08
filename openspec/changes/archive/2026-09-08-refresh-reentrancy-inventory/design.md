## Context

Semantic and generator results now use explicit `AnalysisStore` parameters, but
the generated frontend, parser classification tables, legacy declaration lookup,
and active AST arena binding still share state. Existing prose describes pieces
of this boundary without a single removal-oriented inventory.

## Decisions

- Treat generated Flex/Bison state, parser classification tables, declaration
  lookup tables, and the active AST arena binding as four distinct boundaries.
- Exclude immutable canonical types and file-static debug constants because they
  do not contain compilation results.
- Keep the roadmap sequence capability-based and without delivery dates.
- Retain `stage3/TODO` as a maintenance signpost while replacing its completed
  item with the repository's OpenSpec/catalogue tracking rule.

## Risks / Trade-offs

The inventory can drift as implementation moves. A specification scenario now
requires it to be updated in the same change that removes or introduces a
compatibility boundary.

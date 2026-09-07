## Context

The historical grammar contains a fully commented access-variable sketch and the
configuration AST already reserves an `access_declarations` child. The current
experimental normalizer validates a smaller simple-global subset, records metadata,
then replaces its lines with blanks so the legacy parser never sees it.

## Goals / Non-Goals

**Goals:** parse the already validated subset into explicit AST nodes, preserve
source locations, emit it through `iec2iec`, and retain existing CSV behavior.

**Non-Goals:** hierarchical paths, resource access, direct addresses, runtime C
aliases, or claims about unverified normative semantics.

## Decisions

- The validator remains the experimental semantic gate in this increment.
- The grammar accepts only `name : global : type [direction];`, matching the
  validator rather than reviving the broader commented historical grammar.
- C generation ignores access AST nodes because they describe external metadata;
  `ACCESS.csv` remains the supported consumer boundary.
- IEC generation prints the parsed declarations deterministically.

## Risks / Trade-offs

The source is still scanned once by the validator before parsing. Removing that
remaining duplication belongs to later semantic-pass migration.

## Migration Plan

Add AST nodes and grammar, stop deleting validated blocks, add generator visitors,
run positive/negative/profile/output regressions, then archive the change.

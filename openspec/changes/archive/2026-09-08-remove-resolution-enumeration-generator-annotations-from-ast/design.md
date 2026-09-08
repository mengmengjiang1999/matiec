## Context

Datatype, flow, and constant producers now use context-owned working records.
Resolution fields remain on invocation/operator nodes, enumeration multimaps on
scope nodes, and a generic annotation map on every `symbol_c`. Publication
visitors still copy the first two families after their passes.

## Goals / Non-Goals

**Goals:**

- Remove all remaining analysis/generator result storage from AST layouts.
- Preserve producer behavior through live persistent/transient typed records.
- Eliminate resolution and enumeration whole-tree publication passes.

**Non-Goals:**

- Rewriting semantic algorithms or generated output.
- Removing the active AnalysisStore scope; the next change makes dependencies explicit.

## Decisions

1. Add generic resolution and enumeration working APIs to AnalysisStore. Arena
   nodes update canonical entries; helper nodes use transient context tables.
2. Provide short-lived `symbol_c` accessor methods matching legacy field names.
   This bounds the mechanical migration and will be removed with the active-store
   bridge in the explicit-dependencies change.
3. Validate persistent resolution/enumeration records at their pass boundaries
   instead of publishing them through a traversal.
4. Delete the unused generic AST annotation map outright because Stage 4 already
   reads and writes generator records explicitly.

## Risks / Trade-offs

- [Legacy access is missed] → Compile every visitor and search field spellings.
- [Foreign pointers enter live records] → Validate persistent tables before later passes.
- [Transient helpers lose state] → Keep context-owned transient tables and focused reset tests.

## Migration Plan

Add working APIs, migrate field accesses, remove fields and publishers, validate
all configurations, archive, and commit as one reversible change.

## Open Questions

None.

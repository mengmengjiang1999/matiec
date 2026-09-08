## Context

Earlier roadmap changes moved every semantic and generator result out of AST nodes. Transitional node methods and free functions preserve old call shapes by consulting a `thread_local` active analysis store installed around compilation. The store is context-owned, but the dependency at each use site remains implicit.

## Goals / Non-Goals

**Goals:**
- Make analysis reads and writes require an explicit object supplied by the compilation context.
- Preserve typed working and completed-record behavior.
- Remove ambient analysis state without changing compiler output.

**Non-Goals:**
- Redesign parser or AST-arena compatibility scopes.
- Reorder semantic passes or alter IEC language behavior.
- Split the legacy visitors into new pass implementations.

## Decisions

### Use the existing store as the explicit analysis interface

Use `AnalysisStore` references as the explicit dependency for typed flow, constant,
datatype, resolution, enumeration, and generator access. Passes and generators
retain a reference to the context-owned store and use it at every result access;
this avoids introducing a second façade over the already typed store API.

### Keep ownership in CompilationContext

`CompilationContext` remains the sole owner of `AnalysisStore`. Stage entry points
obtain the store and pass it down; no AST node, visitor, or global owns analysis
state.

### Migrate constructor and helper boundaries

Stage 3 visitors and datatype helpers receive analysis explicitly. Stage 4
generator bases receive the existing context-owned store and propagate it to
nested generators. Transitional AST-shaped accessors require an explicit store,
so their call sites remain readable without recovering analysis from ambient
state.

## Risks / Trade-offs

- Legacy visitor code has many result accesses; mechanical migration can miss a helper boundary. Build, sanitizers, focused tests, and characterization tests guard this.
- Constructor signatures become more verbose, but dependencies become inspectable and testable.

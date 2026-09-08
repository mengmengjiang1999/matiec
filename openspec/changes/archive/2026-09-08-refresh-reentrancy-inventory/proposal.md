## Why

The AnalysisStore migration is complete, but reader-facing architecture notes
still mix completed cleanup with the remaining non-reentrant frontend boundary.
Several current OpenSpec capabilities also retain generated placeholder purposes,
and `stage3/TODO` lists a CONSTANT check that is already implemented.

## What Changes

- Publish an exact inventory of remaining process-wide and thread-local compiler
  state and its planned removal order.
- Update the README roadmap to identify reentrancy as the active architecture
  track without presenting it as a scheduled promise.
- Remove the stale Stage 3 TODO and replace placeholder OpenSpec purposes with
  maintained capability descriptions.

## Capabilities

### Modified Capabilities

- `compilation-context`: Require the compatibility-boundary inventory to track
  each remaining mutable shared-state owner and removal milestone.

## Impact

This is a documentation and specification baseline change. It does not alter
compiler behavior, generated output, or the public command line.

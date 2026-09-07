## Why

Flow and constant analysis now publish context-owned results, but datatype
candidate sets still exist only as mutable AST annotations. The next migration
step must establish the Analysis Store as the compilation-lifetime boundary for
candidate results without destabilizing the large legacy narrowing surface.

## What Changes

- Publish candidate datatype vectors into typed Analysis Store records immediately
  after the fill-candidates phase.
- Provide an explicit compatibility materializer for existing narrowing consumers.
- Add focused ownership, preservation, and context-reset tests.
- Correct README and architecture documentation that still says the Analysis Store
  and parser-state work are deferred.

## Capabilities

### Modified Capabilities

- `semantic-pass-pipeline`: datatype candidate results become context-owned records
  at the producer/consumer boundary.
- `compilation-context`: candidate records follow context lifetime and reset rules.

## Impact

Analysis Store API, Stage 3 orchestration/build files, compiler tests, README,
architecture documentation, and the superseded migration decision.

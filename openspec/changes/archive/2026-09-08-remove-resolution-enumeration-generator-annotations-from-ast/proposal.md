## Why

Resolution and enumeration results already have typed store consumers, and
generator metadata is written directly there, but their producer-side AST fields
remain. Removing the final semantic compatibility fields completes AST/result
separation before explicit analysis dependencies are introduced.

## What Changes

- **BREAKING** Remove function/FB resolution, enumeration lookup, and generator
  annotation fields from AST nodes.
- Route resolution and enumeration producers through live persistent or transient
  AnalysisStore working records.
- Remove resolution/enumeration publication traversals and their build wiring.
- Update focused tests, README, and architecture documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `semantic-pass-pipeline`: Resolution and enumeration producers update canonical
  typed records directly, and generator metadata has no AST compatibility map.
- `ast-lifecycle`: AST nodes contain no remaining semantic or generator result fields.

## Impact

AST definitions, AnalysisStore APIs, Stage 3 producer visitors, build inputs,
focused tests, and architecture documentation change. Language and CLI behavior
remain unchanged.

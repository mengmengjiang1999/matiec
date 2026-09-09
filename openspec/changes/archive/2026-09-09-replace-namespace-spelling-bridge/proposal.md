# Proposal: replace namespace spelling bridge

## Why

Rewriting only the main source before parsing makes namespace behavior diverge
between direct input and included input. It also duplicates lexical and grammar
logic outside the authoritative frontend.

## What changes

- Discover namespace declarations and imports through the native parser prepass.
- Resolve qualified and imported identifiers in the native Flex scanner using
  context-owned namespace state.
- Build namespace wrappers and declarations from unmodified input bytes.
- Support namespace declarations and references across virtual/filesystem include
  boundaries.
- Remove the pre-parse source spelling normalizer and its build surface.
- Update regression coverage, feature status, README, and architecture docs.

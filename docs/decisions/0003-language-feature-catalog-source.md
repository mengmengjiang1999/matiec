# ADR 0003: The checked C++ catalogue is the validation source

## Status

Accepted

## Decision

`compiler/language_feature_catalog.cc` is the machine-validated source for stable
feature identifiers, implementation states, evidence classifications, activation
boundaries, test links, and explicit coverage gaps.

`docs/standards/iec61131-3-evolution.zh-CN.md` remains the human-oriented summary.
It explains the evidence policy and roadmap, while code tests reject malformed or
duplicate catalogue records. OpenSpec defines the required fields and change
discipline; it does not duplicate every catalogue row.

The matrix is deliberately curated instead of generated for now. Generating it
would require adding a build-time documentation tool for a small, still-evolving
dataset. If the catalogue grows enough to justify generation, the generator must
consume the checked catalogue or a single replacement data file and keep the same
validation rules.

## Consequences

- A language implementation change updates its catalogue record and tests together.
- An implemented or partially implemented entry must name maintained tests or mark
  an explicit coverage gap.
- Standards evidence and repository implementation evidence remain independent.
- The user-facing matrix may summarize records, but must not contradict their IDs,
  status, activation, or evidence classification.

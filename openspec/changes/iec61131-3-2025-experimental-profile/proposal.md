## Why

MATIEC currently describes a language rooted in the IEC 61131-3 second-edition final draft, while the latest published edition is IEC 61131-3:2025. This experimental project will evolve toward publicly documented modern features without purchasing or redistributing the copyrighted standard, so it needs explicit evidence, compatibility, and non-conformance boundaries before language behavior changes.

## What Changes

- Define named legacy and experimental language profiles instead of mixing historical MATIEC extensions with later-edition behavior.
- Preserve the current accepted language as the default legacy profile during migration.
- Add a traceable feature catalogue that records implementation status, evidence level, dialect, tests, and known uncertainty.
- Establish `iec61131-3:2025-experimental` as a non-conformance claim: it may contain only publicly confirmed features, cross-vendor behavior identified as provisional, and explicitly named MATIEC extensions.
- Retain IL as a legacy capability until authoritative public material establishes its precise fourth-edition status and a migration policy is implemented.
- Use separate follow-up OpenSpec changes for UTF-8 strings, completed references, namespaces, object-oriented constructs, `VAR_ACCESS`, standard-library updates, and other verified deltas.
- Extend regression requirements so every language-profile change has positive, negative, compatibility, and generated-output coverage where applicable.
- Document that neither the IEC publication nor copied normative tables or grammar text may be committed to this repository.

## Capabilities

### New Capabilities

- `language-standard-profiles`: Selection, naming, compatibility behavior, and user-visible claims for legacy and experimental language profiles.
- `language-conformance-traceability`: Evidence classifications and a maintained mapping from language features to implementation, tests, and unresolved standard questions.

### Modified Capabilities

- `regression-validation`: Language changes must be tested in their intended profile and must demonstrate that the legacy profile remains compatible.

## Impact

- Affects compiler options, CLI parsing, diagnostics, lexer/parser feature gates, standard-library selection, tests, README, and the user manual in later implementation tasks.
- Adds planning and traceability documentation immediately; it does not yet add or claim support for an unverified IEC 61131-3:2025 syntax construct.
- Future language changes will be delivered as smaller OpenSpec changes and pushed only to the fork's `origin`, not to upstream.

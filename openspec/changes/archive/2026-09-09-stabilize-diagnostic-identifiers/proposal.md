# Proposal: machine-readable diagnostics

## Why

IDEs and services need stable filtering and navigation fields. English messages
are presentation text and cannot serve as a durable protocol.

## What changes

- Add stable diagnostic codes and compiler phase classification.
- Add explicit range and byte-offset validity fields.
- Make the size-tagged diagnostic getter compatible with the prior prefix.
- Categorize source, parser, semantic, generation, cancellation, and resource
  failures without requiring message parsing.
- Update tests, ABI fixtures, README, and embedding documentation.

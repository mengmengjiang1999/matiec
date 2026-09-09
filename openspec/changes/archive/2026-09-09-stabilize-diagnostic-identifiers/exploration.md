# Exploration: stable diagnostic identifiers

## Current state

Diagnostics expose severity, English text, and optional line/column endpoints.
Hosts must parse text to classify failures, cannot distinguish compiler phases,
and cannot tell an absent range from zero-valued coordinates without inference.
The size tag is present, but retrieval currently rejects the previous structure
size, preventing additive tail evolution.

## Decision

Extend the diagnostic tail in API 1.6.0 with a stable code, phase, explicit
range-valid bit, byte offsets, and offset-valid bit. Preserve the complete 1.2
prefix: retrieval accepts that prefix and only fills extension fields when the
caller's size reaches them. The compiler sets a current phase around source,
parser, semantic, and generation work; generic stable codes are derived from
phase and severity, while infrastructure failures use specific codes.

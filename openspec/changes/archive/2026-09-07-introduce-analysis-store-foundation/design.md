## Context

AST annotations are convenient but make derived state impossible to reset without
destroying syntax. The architecture decision in ADR 0002 calls for an
identity-keyed store owned by each compilation context before individual passes
are migrated.

## Goals / Non-Goals

**Goals:**

- Give every compilation context a separate analysis store.
- Distinguish a missing record from a present but invalid record.
- Validate keys and symbol references against the owning AST arena.
- Provide typed tables for the planned migration sequence.

**Non-Goals:**

- Move any existing AST annotation in this foundation change.
- Change semantic algorithms or generated code.
- Make globally canonical datatype objects context-owned.

## Decisions

### Use typed records behind one context-owned store

Each analysis domain has explicit record and entry types. Callers cannot mix
flow, constant, datatype, resolution, enumeration, or generator entries even
though all tables use AST node identity as their key.

### Validate arena membership on writes

The store accepts a record only when its key and every non-null symbol reference
are owned by the associated arena. Rejected writes leave the store unchanged.
This catches cross-context leakage at its boundary.

### Represent invalidity separately from absence

Lookup returns null for missing records. Stored entries carry a valid/invalid
status so a pass can record a completed negative result without making it look
unevaluated.

### Reset between compile invocations

`Compiler::compile` clears the store before parsing. This permits safe reuse of a
context while leaving AST destruction policy unchanged.

## Risks / Trade-offs

- Including legacy constant-value representation in the foundation temporarily
  exposes an AST type in the store API. The constant migration can remove the AST
  field without changing record semantics.
- Arena validation excludes process-owned symbols. Later datatype migration must
  explicitly model canonical types instead of weakening the identity boundary.

## Migration Plan

Land the unused foundation with focused tests, then migrate flow and constant
analysis in separate reversible changes.

## Context

OO elements have broad cross-vendor evidence but exact normative behavior remains
to be verified. Generated C needs a stable and documented ABI.

## Decisions

- Freeze no grammar or ABI until an evidence matrix and object model are reviewed.
- Land declarations, type rules, and dispatch in independently testable increments.
- Keep all new syntax out of legacy unless a separate compatibility decision is made.
- The first increment is a MATIEC provisional method subset, enabled only by
  `iec61131-3:2025-experimental`:
  - `METHOD [PUBLIC] name : return_type ... END_METHOD` may appear directly in a
    `FUNCTION_BLOCK`; no standalone `CLASS` construct is introduced.
  - A method receives its owning function-block instance by reference, may read and
    write owner fields, and uses legacy function typing for parameters and returns.
  - Calls use `instance.method(arguments)` and are statically dispatched from the
    declared instance type. Interfaces, inheritance, overrides, overloads,
    properties, `THIS`, `SUPER`, and non-public access are rejected or unsupported.
  - Method locals retain legacy function-local lifetime. The owner instance remains
    caller-owned and is never copied by method dispatch.
  - C lowering uses one length-prefixed function symbol per owner/method and passes
    the instance as the final hidden `VAR_IN_OUT` parameter, after the method's
    ordinary interface parameters. This is an unstable
    experimental ABI but deterministic and collision-resistant.

## Open Questions

- Normative visibility, overriding, overloads, interface conformance, inheritance,
  layout, and dynamic dispatch rules remain open for later increments.

## Evidence review (2026-09-06)

The evidence matrix in `docs/standards/object-oriented-evidence.md` confirms the
Edition 3 capability family and cross-vendor method spelling. It does not establish
a complete Edition 4 object model. The bounded static method subset above is
therefore explicitly project-defined rather than presented as normative IEC text.

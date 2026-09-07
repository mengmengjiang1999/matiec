## Context

`enum_declaration_check_c` populates enum lookup tables on scope AST nodes and
`publish_enumeration_analysis()` copies the completed tables into arena-checked
typed records. Stage 3 currently calls the inverse materializer immediately
after publication. Repository-wide consumer inspection shows that subsequent
passes build their own declaration-derived working indexes and do not read the
scope AST tables, so this copy-back is redundant production work.

## Goals / Non-Goals

**Goals:**

- Make the typed enumeration records the authoritative completed-pass output.
- Stop copying completed tables back to AST fields in production.
- Verify typed multimap lookup independently of AST compatibility state.

**Non-Goals:**

- Rewrite the enumeration declaration producer in this change.
- Remove AST fields or the optional compatibility adapter.
- Change enum ambiguity, casing, or declaration semantics.

## Decisions

- Publication remains immediately after enumeration checking. This preserves
  pass ordering and captures exactly the tables validated by the producer.
- The production materializer call is removed rather than replaced by another
  consumer adapter because there are no downstream production reads to migrate.
- The existing materializer remains as an explicitly invoked compatibility API;
  focused tests first prove direct record access with an empty AST table, then
  continue to cover the adapter.

## Risks / Trade-offs

- [A hidden consumer relies on AST tables] → repository-wide search plus normal,
  ASan, UBSan, CLI, initialization, syntax, and characterization suites cover the
  production pipeline.
- [Store and AST state diverge after publication] → the enumeration pass is the
  declared completion boundary; later mutation of producer scratch state is not
  supported.

## Migration Plan

Remove the Stage 3 materializer call, strengthen the focused test, update status
documentation, run all gates, and archive the change. Re-adding the single call
is the rollback path.

## Open Questions

None.

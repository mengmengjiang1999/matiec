# Proposal: eliminate parser compatibility access

## Why

Generated frontend entry points name their parser state, but compatibility
helpers can still silently select it from thread-local storage. That obscures
dependencies and prevents same-thread nested parser sessions from being reasoned
about locally.

## What changes

- Make parser/scanner helpers explicitly accept `ParserState`.
- Use the Bison/Flex `parser_state` parameter for options, state flags, symbol
  tables, cancellation, and retained scanner strings.
- Remove `ActiveParserStateScope`, `active_parser_state()`, and optional parser
  selector access.
- Split remaining legacy ownership bindings by resource and document the boundary.
- Add source-level architecture gates and keep behavioral regressions green.

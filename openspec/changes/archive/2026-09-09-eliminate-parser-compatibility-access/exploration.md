# Exploration: eliminate parser compatibility access

## Current state

Bison already receives `ParserState` through `%parse-param` and Flex receives it
through `%lex-param`, but handwritten helpers still recover that state through a
thread-local `active_parser_state()` selector. The same selector also indirectly
binds AST ownership, runtime options, and semantic declaration tables.

## Direction

- Pass `ParserState&` to every parser/scanner state and token-classification
  helper and use the generated function parameter directly in grammar actions.
- Remove the process-visible active `ParserState` selector and its nesting scope.
- Preserve the legacy AST/runtime/semantic APIs behind narrowly typed bindings;
  they may select only the resource they own and cannot expose parser state.
- Strengthen the architecture audit so hidden parser-state recovery cannot return.

This separation is intentionally completed before converting Flex itself to a
reentrant scanner in the next change.

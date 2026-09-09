# Design: eliminate parser compatibility access

## Explicit frontend dependencies

All helpers called from Flex or Bison receive the invocation's `ParserState&`.
Grammar symbol-table aliases expand against that named parameter rather than a
thread-local accessor. Scanner allocation and cancellation likewise use it.

## Narrow legacy bindings

Legacy AST constructors and downstream utilities cannot yet accept a compilation
context without changing the public AST constructor surface. They therefore bind
only `AstArena`, `DeclarationSymbolTables`, or `runtime_options_t`, respectively.
No one can use these bindings to discover or replace a parser invocation.

## Enforcement

The architecture regression rejects all active-parser selector declarations and
requires explicit state at representative scanner/helper call sites.

# Design: make Flex scanner fully reentrant

## Scanner ownership

`ParserState` holds only the opaque handle for its currently active scanner.
The scanner's Flex extra object owns all handwritten mutable lexer state. Parse
setup constructs both objects before input is attached; reset closes include
frames, frees tracking/body buffers, destroys the Flex handle, and clears the
opaque handle even on exceptions.

## Generated interface

Bison receives both `ParserState&` and the scanner handle. Its lexer call passes
both to the reentrant `yylex`, and grammar callbacks continue to use
`ParserState` while scanner control helpers resolve that state's own handle.

## Build enforcement

The generated frontend checker continues rejecting generated `exit`, `_Exit`,
or `abort` calls. Flex declaration rewriting is deleted: generated state must be
reentrant by construction. Architecture tests require `%option reentrant` and
reject handwritten Flex `thread_local` declarations.

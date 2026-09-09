# Exploration: make Flex scanner fully reentrant

## Current boundary

The Bison parser is pure and receives `ParserState` explicitly, but Flex still
generates a non-reentrant scanner. A post-generation Perl rewrite changes Flex
globals to `thread_local`, while handwritten filename, include stack, source
tracking, and body lookahead state are also thread-local.

This isolates separate threads but aliases a nested compilation on the same
thread. The temporary `current_lexer_parser_state` pointer used by `YY_INPUT`
also means scanner input is selected implicitly.

## Direction

Use Flex's reentrant mode and one scanner handle per parse session. Move every
handwritten mutable scanner field into the scanner's extra state, pass the
handle through Bison, and make public scanner helpers obtain the handle from
their explicit `ParserState`. Retain the generated-code safety check only for
process termination; it must no longer rewrite Flex storage.

## Verification

Add a synchronous nested-compilation regression through the virtual include
resolver, preserve parallel compilation tests, and audit generated/scanner
sources for legacy Flex thread-local state.

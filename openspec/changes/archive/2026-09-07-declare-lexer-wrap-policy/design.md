## Context

`stage1_2/iec_flex.ll` defines `int yywrap(void)`. Autoconf therefore must not
require or search for an external `yywrap` provider when selecting Flex/Lex.

## Decision

Call `AC_PROG_LEX([noyywrap])`. This describes link ownership to Autoconf; it does
not add Flex's `%option noyywrap` and does not remove MATIEC's scanner callback.

`compilation_abort.cc` is compiler support used by both executables and tests. It
belongs to `libcompiler.a`; keeping another top-level compilation of a source below
the recursive `compiler` directory makes that directory's cleanup own dependency
files still referenced by its parent when `subdir-objects` is enabled.

## Compatibility

Autoconf's macro retains compatibility with the older no-argument behavior, while
current releases recognize `noyywrap` and suppress the obsolete-form warning.

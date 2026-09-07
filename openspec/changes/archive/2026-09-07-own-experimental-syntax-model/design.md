## Context

Four bounded experimental recognizers already return source ranges and structured
records in addition to lowered source. Those records currently disappear after
`Compiler::compile`, while the context owns all other per-compilation services.

## Goals / Non-Goals

**Goals:**

- Give structured experimental syntax one explicit per-compilation owner.
- Keep normalizers as producers and avoid new mutable global state.
- Reset metadata deterministically when a context is compiled again.

**Non-Goals:**

- Replace source lowering with native Flex/Bison productions in this change.
- Define new namespace, method, access-variable, or library semantics.

## Decisions

The shared header contains only declaration value types and the aggregate model.
Each normalizer result retains its transformed source and embeds the relevant
declaration vector. The compiler moves those vectors into the context after all
recognizers succeed. The context clears the aggregate at the start of every
compile, including legacy-profile compilation and failed attempts.

## Risks / Trade-offs

The structured declarations are still side metadata rather than main `symbol_c`
nodes. This is intentional: it creates a stable handoff for later native parser
work without mixing a grammar rewrite into an ownership-only change.

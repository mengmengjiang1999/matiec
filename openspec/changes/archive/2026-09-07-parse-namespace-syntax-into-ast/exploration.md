# Exploration

The experimental namespace normalizer already validates a bounded provisional
grammar, resolves names, and records context-owned metadata. It currently blanks
`NAMESPACE`, `END_NAMESPACE`, and `USING` before invoking the legacy parser, so
the main AST and `iec2iec` cannot represent the accepted source structure.

The parser's `library` is a list of top-level declarations. Semantic visitors
mostly recurse through reference/list nodes, but dependency ordering rebuilds the
library and would flatten or discard a newly introduced wrapper unless it handles
that node explicitly. The C generator also performs three direct passes over the
library, while the IEC generator can emit a wrapper directly.

The safest incremental boundary is to retain the existing validated name lowering
for lookup and ABI stability, parse the preserved structural syntax into native AST
nodes, teach dependency ordering to retain that structure, and make both generators
consume it. This does not claim unverified fourth-edition semantics or remove the
remaining pre-parse resolver.

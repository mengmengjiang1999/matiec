## REMOVED Requirements

### Requirement: Generator annotations are context-owned

**Reason**: This post-generation publication contract and its compatibility
materialization scenario were superseded by the live typed-record contract in
`Stage 4 exchanges generator annotations through the context`.

**Migration**: Stage 4 producers and consumers use live generator records through
the output-bound Analysis Store. No post-generation publisher or AST materializer
is used.

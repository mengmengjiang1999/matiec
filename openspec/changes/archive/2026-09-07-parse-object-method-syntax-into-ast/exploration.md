# Exploration

The experimental object-method normalizer recognizes methods directly inside a
function block, rejects non-public visibility and unsupported OO keywords, records
method metadata, rewrites owner-field references into hidden `VAR_IN_OUT`
parameters, rewrites statically typed instance calls, removes every `METHOD` block,
and appends deterministic lowered legacy functions. The main parser therefore
never sees method ownership or boundaries, and `iec2iec` emits only the lowered
function.

The accepted subset has a method return type, leading function-style parameter and
local declaration blocks, and an ST/IL body. Owner fields remain owned by the
calling FB instance and are passed after explicit parameters. The current ABI name
is a case-insensitive, length-prefixed owner-and-method identifier. Method overloads,
non-public visibility, include-spanning declarations, and dynamic OO semantics are
outside the supported boundary.

The primary FB AST currently stores only its name, variable declarations, and
execution body. Stage 3 and C generation explicitly visit those fields, while IEC
generation prints the FB directly. Dependency ordering retains each FB node as a
unit, so a method list owned by the FB survives `-p` without becoming a separate
POU. The safest incremental migration is to preserve validated method source,
parse it into a dedicated FB-owned AST list, keep compatibility lowering for
semantic analysis and C generation, and make IEC generation print only the native
method nodes inside the FB. This moves structural authority into the main AST
without changing the provisional behavior or ABI.

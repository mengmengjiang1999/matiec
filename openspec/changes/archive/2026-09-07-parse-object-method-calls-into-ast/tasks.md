## 1. Native method-call syntax

- [x] 1.1 Add an experimental native method-invocation AST node and parser rules
- [x] 1.2 Stop rewriting method calls in source text and expose bounded binding metadata

## 2. Compatibility pipeline

- [x] 2.1 Add a pre-stage-3 AST pass that binds native method calls and constructs compatibility invocations
- [x] 2.2 Delegate datatype analysis and C generation while preserving native IEC output

## 3. Verification and documentation

- [x] 3.1 Add AST, normalized IEC, semantic, C, runtime, negative, and legacy regressions
- [x] 3.2 Update architecture, method semantics, user documentation, and the feature catalogue
- [x] 3.3 Run focused and full tests plus sanitizer and strict OpenSpec validation
- [x] 3.4 Archive the change, create one commit, and push it to `origin/main`

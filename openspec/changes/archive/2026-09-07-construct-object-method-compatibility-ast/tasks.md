## 1. Compatibility declaration AST

- [x] 1.1 Stop appending lowered method functions to normalized source
- [x] 1.2 Construct compatibility function AST nodes from native methods
- [x] 1.3 Attach compatibility declarations before symbol-table initialization

## 2. Semantic and generator integration

- [x] 2.1 Preserve method result, local, parameter, and owner-field binding semantics
- [x] 2.2 Keep lowered C ABI stable and omit synthetic declarations from IEC output

## 3. Verification and delivery

- [x] 3.1 Add focused AST/source and end-to-end semantic/C/runtime regressions
- [x] 3.2 Update architecture, method semantics, user documentation, and feature catalogue
- [x] 3.3 Run focused, full, sanitizer, and strict OpenSpec validation
- [x] 3.4 Archive the change, create one commit, and push it to `origin/main`

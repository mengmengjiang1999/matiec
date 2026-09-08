## 1. Explicit analysis API

- [x] 1.1 Make the existing typed analysis API require an explicit store
- [x] 1.2 Remove active analysis scope and thread-local state

## 2. Pipeline migration

- [x] 2.1 Pass analysis explicitly through Stage 3 visitors and helpers
- [x] 2.2 Pass analysis explicitly through Stage 4 generator components
- [x] 2.3 Require an explicit store at AST-shaped result accessors and verify no ambient access remains

## 3. Verification and documentation

- [x] 3.1 Update focused analysis and sequential-compilation tests
- [x] 3.2 Synchronize README, architecture inventory, and decision records
- [x] 3.3 Run strict OpenSpec, normal, ASan, UBSan, and characterization validation

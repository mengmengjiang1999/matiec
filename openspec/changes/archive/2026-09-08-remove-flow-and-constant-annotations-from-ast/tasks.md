## 1. Context-owned working state

- [x] 1.1 Add persistent/transient constant working-record APIs and lifecycle cleanup
- [x] 1.2 Remove flow accessor fallbacks to IL AST vectors

## 2. AST and pipeline migration

- [x] 2.1 Migrate constant producers and helper nodes to context-owned working records
- [x] 2.2 Remove flow and constant fields from AST definitions and update temporary flow helpers
- [x] 2.3 Remove the constant publication traversal and its Stage 3 build wiring

## 3. Verification and documentation

- [x] 3.1 Update focused flow/constant tests and add transient/reset coverage
- [x] 3.2 Synchronize README, architecture documentation, ownership inventory, and ADR status
- [x] 3.3 Run OpenSpec validation plus normal, ASan, UBSan, and characterization tests

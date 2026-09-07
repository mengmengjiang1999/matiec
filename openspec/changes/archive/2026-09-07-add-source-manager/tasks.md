## 1. Source abstraction

- [x] 1.1 Add a context-owned `SourceManager` for file and memory input
- [x] 1.2 Add focused unit tests for source identity and owned bytes

## 2. Parser integration

- [x] 2.1 Add replayable memory-source entry points to the lexer/parser boundary
- [x] 2.2 Route memory source through `LegacyGlobalStateAdapter`
- [x] 2.3 Remove named temporary-source creation from `Compiler`

## 3. Validation and documentation

- [x] 3.1 Add end-to-end valid, invalid, experimental, and pre-parsing memory tests
- [x] 3.2 Update embedding and architecture documentation
- [x] 3.3 Run OpenSpec strict validation and all compiler regressions
- [x] 3.4 Archive the completed change and push its validated commit to `origin/main`

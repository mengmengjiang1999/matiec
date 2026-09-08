## 1. Parser-owned allocation target

- [x] 1.1 Attach the context AST arena to `ParserState`
- [x] 1.2 Route legacy node and retained-string ownership through the active parser session

## 2. Remove compatibility binding

- [x] 2.1 Make `AstArena::make()` explicitly attach AST-derived objects
- [x] 2.2 Remove `ActiveAstArenaScope` and `active_ast_arena()`
- [x] 2.3 Convert repository callers and focused tests

## 3. Verification and documentation

- [x] 3.1 Extend lifecycle and parser-isolation coverage
- [x] 3.2 Update README and architecture inventories
- [x] 3.3 Run normal and sanitizer suites plus strict OpenSpec validation

## 4. Completion

- [x] 4.1 Archive the OpenSpec change
- [x] 4.2 Commit and push the completed change

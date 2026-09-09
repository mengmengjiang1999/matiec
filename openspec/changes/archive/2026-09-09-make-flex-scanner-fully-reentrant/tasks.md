## 1. Reentrant scanner

- [x] 1.1 Generate Flex in reentrant mode and pass its handle through Bison
- [x] 1.2 Move handwritten mutable lexer state into the scanner session
- [x] 1.3 Make setup, include, reset, and error paths own the correct scanner

## 2. Regression coverage

- [x] 2.1 Add same-thread nested compilation coverage
- [x] 2.2 Remove generation rewrites and strengthen architecture gates
- [x] 2.3 Update README and architecture documentation

## 3. Verification and completion

- [x] 3.1 Run normal and sanitizer-relevant regressions
- [x] 3.2 Run strict OpenSpec validation
- [x] 3.3 Archive, commit, and push the change

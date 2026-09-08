## 1. Public parallel API

- [x] 1.1 Add bounded ordered batch compilation to `Compiler`
- [x] 1.2 Reject duplicate mutable contexts before worker execution
- [x] 1.3 Propagate unexpected worker exceptions after joining the batch

## 2. End-to-end coverage

- [x] 2.1 Add concurrent full-pipeline success and failure regression coverage
- [x] 2.2 Verify diagnostic, AST, parser-table, and generated-output isolation
- [x] 2.3 Register the threaded regression in the build and distribution metadata

## 3. Documentation and verification

- [x] 3.1 Document the embedding API, concurrency contract, and output-path rule
- [x] 3.2 Update README and architecture roadmap to supported status
- [x] 3.3 Run normal and sanitizer suites plus strict OpenSpec validation

## 4. Completion

- [x] 4.1 Archive the OpenSpec change
- [x] 4.2 Commit and push the completed change

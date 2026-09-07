## 1. Parser and AST registration

- [x] 1.1 Add a profile-aware parser symbol-registration option for experimental `ASSERT`
- [x] 1.2 Replace the source normalizer with post-parse AST discovery and metadata
- [x] 1.3 Construct the compiler-owned BOOL-to-VOID function declaration before semantic analysis
- [x] 1.4 Preserve user-defined `ASSERT` ownership and sequential compilation isolation

## 2. Verification and documentation

- [x] 2.1 Replace normalizer unit tests with top-level AST registry coverage
- [x] 2.2 Verify legacy/experimental CLI behavior, generated C, invalid expression diagnostics, and original source locations
- [x] 2.3 Update architecture, standards inventory, evolution tracking, and feature evidence
- [x] 2.4 Run the full regression suite under normal, AddressSanitizer, and UndefinedBehaviorSanitizer builds
- [x] 2.5 Validate and archive the OpenSpec change

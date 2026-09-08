## 1. Repair clean Linux builds

- [x] 1.1 Give the shared stage 4 implementation one parent-directory archive
- [x] 1.2 Link the shared archive before each generator provider
- [x] 1.3 Order circular static-library providers for GNU ld
- [x] 1.4 Reproduce the clean parallel GCC failure and verify the corrected target layout
- [x] 1.5 Declare compiler unit-test AST dependencies exposed by UBSan RTTI

## 2. Repair hosted sanitizer jobs

- [x] 2.1 Run the self-bootstrapping sanitizer script directly from the workflow
- [x] 2.2 Remove stale dependency directories from isolated sanitizer copies
- [x] 2.3 Run address/leak and undefined-behavior regressions

## 3. Completion

- [x] 3.1 Validate and archive the OpenSpec change
- [x] 3.2 Commit and push for hosted-check verification

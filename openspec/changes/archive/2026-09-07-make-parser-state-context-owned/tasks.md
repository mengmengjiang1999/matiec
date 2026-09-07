## 1. Parser state ownership

- [x] 1.1 Add context-owned runtime options and transient parser controls
- [x] 1.2 Replace the process-wide runtime option object with a scoped binding
- [x] 1.3 Move stage 1/2 transition callbacks to the active context state
- [x] 1.4 Reset and restore parser state at adapter boundaries

## 2. Verification

- [x] 2.1 Add context isolation, nested scope, and transient reset tests
- [x] 2.2 Run normal regression and sanitizer suites
- [x] 2.3 Validate and archive the OpenSpec change

## ADDED Requirements

### Requirement: ThreadSanitizer gates concurrent regressions

Linux continuous integration SHALL build and run the regression suite with
ThreadSanitizer in addition to address and undefined-behavior sanitizers.

#### Scenario: A data race is introduced

- **WHEN** a concurrent regression triggers ThreadSanitizer
- **THEN** the dedicated sanitizer check fails

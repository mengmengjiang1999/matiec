## MODIFIED Requirements

### Requirement: Cross-environment validation

The project SHALL validate clean builds and tests on supported GCC/Linux and
Clang/macOS environments, and hosted validation SHALL run independent address,
leak, and undefined-behavior sanitizer jobs.

#### Scenario: A change is validated

- **WHEN** the automated validation workflows run for a push or pull request
- **THEN** GCC/Linux, Clang/macOS, ASan with leak detection, and UBSan execute the
  maintained regression entry points and report independent statuses

#### Scenario: A sanitizer job stalls

- **WHEN** a hosted sanitizer build or regression exceeds its configured limit
- **THEN** the workflow terminates that job and reports a failure

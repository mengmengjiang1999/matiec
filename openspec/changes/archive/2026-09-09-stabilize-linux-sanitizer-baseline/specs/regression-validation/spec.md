## MODIFIED Requirements

### Requirement: Cross-environment validation

The project SHALL validate clean builds and tests on supported GCC/Linux and
Clang/macOS environments, and hosted validation SHALL run independent address,
leak, undefined-behavior, and thread sanitizer jobs without known sanitizer
findings.

#### Scenario: A change is validated

- **WHEN** the automated validation workflows run for a push or pull request from
  a clean checkout
- **THEN** supported compiler and sanitizer jobs execute the maintained regression
  entry points and report independent successful statuses without leaks or races

#### Scenario: A sanitizer job stalls

- **WHEN** a hosted sanitizer build or regression exceeds its configured limit
- **THEN** the workflow terminates that job and reports a failure

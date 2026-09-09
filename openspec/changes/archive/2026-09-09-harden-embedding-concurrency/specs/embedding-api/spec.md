## ADDED Requirements

### Requirement: Callback failures are context-local

The embedding adapter SHALL contain resolver and output callback failures within
their owning compilation and SHALL NOT stop independent batch jobs.

#### Scenario: A callback throws

- **WHEN** a C++ host callback throws while another context compiles
- **THEN** the owning result fails with a diagnostic and the independent result completes normally

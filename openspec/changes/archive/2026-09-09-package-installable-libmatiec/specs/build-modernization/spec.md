## ADDED Requirements

### Requirement: Installed embedding consumer smoke test

The build SHALL include a regression that stages installation, compiles and
links an external consumer using only installed paths, runs it, and verifies the
corresponding uninstall operation.

#### Scenario: Install smoke test runs

- **WHEN** the packaging regression executes after the library is built
- **THEN** installation, C compilation, C++ linking, execution, and uninstall
  checks all succeed in an isolated staging directory

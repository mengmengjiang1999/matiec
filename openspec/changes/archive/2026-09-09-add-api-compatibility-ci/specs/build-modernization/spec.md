## ADDED Requirements

### Requirement: Dedicated API compatibility CI gate

GitHub Actions SHALL run the public header, symbol allowlist, and installed
consumer contract as a separately named Linux job on pushes and pull requests.

#### Scenario: API compatibility regresses

- **WHEN** any header, symbol, or installation contract check fails
- **THEN** the dedicated API compatibility job fails independently of the main
  compiler regression job

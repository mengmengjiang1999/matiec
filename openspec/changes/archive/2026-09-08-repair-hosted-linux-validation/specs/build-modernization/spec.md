## MODIFIED Requirements

### Requirement: Reproducible clean build

The project SHALL support regeneration, configuration, compilation, and regression
testing from a clean checkout using documented tool versions and commands, with an
explicit lexer callback ownership policy.

#### Scenario: Linux clean build

- **WHEN** a supported Linux environment follows the documented bootstrap commands
  with parallel compilation enabled
- **THEN** both compiler executables build and the regression suite passes without
  relying on pre-existing objects or dependency directories

#### Scenario: Build files discover the lexer

- **WHEN** Autoconf checks for Flex or a compatible lexer generator
- **THEN** it uses the scanner's own `yywrap()` implementation without requiring
  an external lexer support library

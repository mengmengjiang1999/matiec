## ADDED Requirements

### Requirement: Profile library declarations are AST-owned

Profile-owned library declarations SHALL enter semantic analysis and generation
as explicit compiler-owned AST nodes. They SHALL NOT be prepended, appended, or
otherwise injected into user source text.

#### Scenario: Experimental ASSERT is compiled

- **WHEN** source invokes the profile-owned `ASSERT` function
- **THEN** generation consumes its AST declaration while diagnostics retain positions in the original source

#### Scenario: Source does not use a profile library function

- **WHEN** experimental source contains no call requiring a profile-owned declaration
- **THEN** no unused compiler-owned declaration is added to the compilation unit

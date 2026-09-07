## MODIFIED Requirements

### Requirement: Explicit language and warning policy

The build SHALL declare its C and C++ language modes, distinguish project warnings
from generated Flex/Bison warnings, and use explicit subdirectory object placement
for source files below the current makefile directory.

#### Scenario: Handwritten code emits a promoted warning

- **WHEN** supported compilers build handwritten project sources
- **THEN** configured high-confidence warnings fail validation while approved
  generated-code warnings remain scoped to generated targets

#### Scenario: Build files are regenerated

- **WHEN** a developer runs `autoreconf --install`
- **THEN** Automake does not warn that subdirectory source object placement may
  change in a future release

#### Scenario: A test uses a compiler service

- **WHEN** the service implementation is already provided by `libcompiler.a`
- **THEN** the test target links that library instead of compiling a duplicate
  implementation object

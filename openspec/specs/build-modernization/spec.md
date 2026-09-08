# build-modernization Specification

## Purpose
TBD - created by archiving change refactor-compiler-architecture. Update Purpose after archive.
## Requirements
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

### Requirement: Repository version metadata
Compiler version output SHALL derive revision metadata from Git when available and SHALL remain usable from source archives without Git metadata.

#### Scenario: Build occurs in a Git checkout
- **WHEN** the compiler is built from a Git worktree
- **THEN** version output contains the corresponding abbreviated revision

#### Scenario: Build occurs from an archive
- **WHEN** Git metadata is unavailable
- **THEN** the build succeeds and version output uses a documented archive fallback

### Requirement: Build frontend compatibility
Autotools SHALL remain functional until any additional build frontend builds equivalent binaries and runs the same regression suites.

#### Scenario: A secondary build frontend is introduced
- **WHEN** the secondary frontend is used in validation
- **THEN** it builds both executables and invokes the shared regression entry point with equivalent results

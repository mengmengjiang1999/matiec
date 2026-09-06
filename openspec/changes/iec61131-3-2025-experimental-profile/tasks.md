## 1. Baseline and Decision Record

- [x] 1.1 Record the experimental-profile decision, evidence levels, claim boundaries, copyright policy, and public sources
- [x] 1.2 Inventory currently implemented language families, activation switches, repository evidence, and unresolved 2025 status
- [x] 1.3 Link the standards evolution record from the main README

## 2. Language Profile Model

- [x] 2.1 Add a typed language-profile value to `CompilerOptions` with legacy as the default
- [x] 2.2 Parse and validate explicit legacy and `iec61131-3:2025-experimental` CLI profile selections in both compiler tools
- [x] 2.3 Expose the profile through the embedding API without introducing process-global state
- [x] 2.4 Update CLI help and diagnostics so the experimental profile cannot be mistaken for a conformance claim
- [x] 2.5 Keep existing extension switches distinct from the selected profile and document their precedence

## 3. Feature Catalogue

- [x] 3.1 Define stable feature identifiers and typed implementation/evidence states
- [x] 3.2 Catalogue the current lexer, datatype, POU, ST, IL, SFC, configuration, reference, and extension capabilities
- [x] 3.3 Associate implemented catalogue entries with maintained test paths or explicit coverage gaps
- [x] 3.4 Add a validation mechanism that rejects missing required catalogue fields and unknown evidence states
- [x] 3.5 Decide whether the checked catalogue or OpenSpec becomes the generated human-readable matrix source and record the decision

## 4. Profile Regression Coverage

- [x] 4.1 Add CLI tests for omitted, valid, and invalid profile selections
- [x] 4.2 Characterize representative ST, IL, SFC, configuration, and reference inputs under the legacy profile
- [x] 4.3 Verify the initially feature-neutral experimental profile preserves representative acceptance, diagnostics intent, and generated output
- [x] 4.4 Add a reusable test pattern requiring experimental acceptance and legacy compatibility or rejection for later features
- [x] 4.5 Run the complete GCC/Linux, Clang/macOS, generated-C, sequential-compilation, and sanitizer checks

## 5. Follow-up Language Changes

- [x] 5.1 Create a focused OpenSpec change for UTF-8 source validation, string representation, functions, runtime behavior, and tests
- [x] 5.2 Create separate scoped changes for reference completion, namespaces, object-oriented elements, `VAR_ACCESS`, and standard-library deltas as evidence permits
- [x] 5.3 Record unresolved Annex B questions without assigning behavior until authoritative public evidence becomes available
- [x] 5.4 Define the legacy IL migration policy only after its precise 2025 status is publicly verified (deferred by ADR 0004 until that gate is met)

## 6. Completion

- [x] 6.1 Update the README, user manual, and standards evolution matrix with the implemented profile interface
- [x] 6.2 Confirm no documentation claims complete or certified IEC 61131-3:2025 conformance
- [ ] 6.3 Sync completed delta specifications into the main OpenSpec specifications and archive the change
- [ ] 6.4 Commit validated increments and push only to the fork's `origin`

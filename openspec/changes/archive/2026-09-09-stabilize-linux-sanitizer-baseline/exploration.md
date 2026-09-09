# Exploration: stabilize Linux sanitizer baseline

## Observed failures

The release-candidate commit passes the normal GCC/Linux and Clang/macOS jobs,
but the Linux sanitizer workflow reports two frontend defects:

- LeakSanitizer observes the standard-library filename allocation escaping when
  cancellation or an embedding callback throws through `stage2__`.
- ThreadSanitizer observes concurrent libstdc++ locale-cache access while each
  compilation constructs the case-insensitive `VAR_ACCESS` rejection regex.

## Chosen direction

Give the filename allocation scope-bound ownership so every return and exception
path releases it. Replace the locale-dependent regex with an ASCII identifier
scanner that preserves the existing leading-whitespace, case-insensitive keyword
semantics and identifier-boundary behavior.

## Validation

Extend the normalizer unit test with mixed-case and near-match cases, run the
normal suite and all sanitizer entry points locally, then require both hosted
workflows to pass before tagging the release baseline.

## Why

Semantic and generator results now live in `AnalysisStore`, but legacy node accessors still discover that store through a `thread_local` active scope. This hides pass dependencies, prevents analysis helpers from declaring what they consume, and leaves correctness dependent on ambient process state.

## What Changes

- Replace active-store-backed result access with an explicit `AnalysisStore` dependency.
- Pass analysis access into Stage 3 producers/consumers and Stage 4 generators through constructors and helper signatures.
- Remove `ActiveAnalysisStoreScope`, `active_analysis_store()`, and the analysis `thread_local` pointer.
- Update focused tests, architecture documentation, and README to describe the final explicit-dependency architecture.

## Impact

- Affected specs: `semantic-pass-pipeline`, `code-generation-boundaries`, `compilation-context`
- Affected code: compiler analysis storage, Stage 3 semantic visitors/helpers, Stage 4 generators, focused analysis tests, documentation

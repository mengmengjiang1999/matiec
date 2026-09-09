## Why

The embedding library is now installable, but accidental symbol removal, header
incompatibility, or packaging regressions could still merge unnoticed. A focused
CI contract should protect the public boundary independently of compiler tests.

## What Changes

- Add a reviewed allowlist of exported `matiec_*` symbols.
- Compile the public header and complete function signatures as C11 and C++17.
- Add a focused `check-api` target covering symbols, headers, and installed use.
- Run the API contract in a dedicated GitHub Actions job.
- Document ABI review and version-update expectations.

## Capabilities

### Modified Capabilities

- `embedding-api`: enforce the published symbol and language-compatibility
  contract.
- `build-modernization`: add a dedicated API compatibility CI gate.

## Impact

No runtime behavior changes. CI gains one Linux job and maintainers gain an
explicit review point whenever public functions change.

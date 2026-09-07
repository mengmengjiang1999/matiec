## Context

`tests/sanitizers/runtests` already creates a temporary source copy, rebuilds with
one sanitizer, runs `make check`, prints failure logs, and removes the copy. CI can
reuse this entry point without duplicating build flags.

## Decisions

- Use a two-entry Ubuntu matrix so ASan and UBSan failures are reported separately.
- Trigger on pushes, pull requests, and manual dispatches, matching the normal CI
  workflow's continuous validation role.
- Keep each job bounded by a timeout and use the same bootstrap dependencies as
  the GCC Linux job.
- Enable leak detection by default where supported, while Apple Clang retains
  address checks without its unsupported LeakSanitizer option.
- Preserve the legacy visitor return contract for datatype bit widths while
  encoding and decoding through `uintptr_t`, avoiding null-pointer arithmetic.

## Risks / Trade-offs

Sanitizer jobs add hosted runtime because each entry performs a clean build. The
separation is retained because it provides precise status and avoids combining
incompatible runtime options.

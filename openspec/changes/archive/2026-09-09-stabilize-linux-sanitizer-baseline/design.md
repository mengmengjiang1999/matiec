# Design: stabilize Linux sanitizer baseline

## Exception-safe parser setup

`stage2__` owns its constructed library filename for the whole two-pass parse.
Represent that ownership with `std::unique_ptr<char, free>` and pass a borrowed
pointer to existing parser helpers. This covers ordinary returns and exceptions
without duplicating cleanup branches.

## Locale-free legacy keyword detection

The legacy-profile guard only needs to recognize `VAR_ACCESS` as the first token
on a line. An ASCII case-folding comparison after spaces and tabs, followed by an
IEC identifier-boundary check, is deterministic and has no shared locale state.

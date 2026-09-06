# Language profile regression pattern

Every later language-feature change must add one compact fixture that is run in
both profiles.

Before the experimental profile owns syntax, the fixture must compile with an
omitted profile, `--std=legacy`, and
`--std=iec61131-3:2025-experimental`; diagnostics and generated output must match.

After a feature is assigned only to the experimental profile, its tests must show:

1. positive parsing, semantic analysis, and output or runtime behavior under the
   experimental profile;
2. the deliberately chosen legacy result: rejection with a stable diagnostic, or
   documented compatibility acceptance;
3. no change to representative feature-neutral legacy output;
4. a feature-catalogue update naming the maintained test path.

The shared CLI regression helper in `tests/cli/runtests` is the starting point for
profile-neutral fixtures. A feature-specific suite should replace equality with
explicit expected results when the two profiles intentionally diverge.

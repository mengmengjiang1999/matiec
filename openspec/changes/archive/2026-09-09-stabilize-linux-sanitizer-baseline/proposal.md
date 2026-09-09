# Proposal: stabilize Linux sanitizer baseline

## Why

The documented release checks are not a reliable baseline while Linux ASan and
TSan fail on supported embedding workloads.

## What changes

- Make the stage-2 standard-library filename allocation exception-safe.
- Remove locale-dependent regex construction from the concurrent legacy-profile
  access-variable guard.
- Add regression coverage for exact keyword recognition and near matches.
- Require the hosted sanitizer workflow to pass before freezing `v1.6.0`.

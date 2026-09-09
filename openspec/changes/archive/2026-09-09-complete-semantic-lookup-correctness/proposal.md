# Proposal: Complete semantic lookup correctness

## Why

Stale lookup state and incomplete declaration results can misclassify later queries,
while parameter and duplicate-declaration checks leave gaps that are difficult to
detect from individual language tests.

## What changes

- Reset every declaration lookup query completely and return full FB specs.
- Make invalid selector queries fail safely rather than dereference a missing name.
- Enforce input direction for implicit FB calls and writable IN_OUT/OUT arguments.
- Add a regression matrix spanning structures, arrays, FB members, parameter
  directions, repeated lookup, and duplicate declarations.
- Update semantic-pipeline and testing documentation.

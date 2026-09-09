# Exploration

Semantic declaration lookup still carries several historical shortcuts. A reused
`search_var_instance_decl_c` can retain the previous type, FB lookup returns only a
split type token instead of its complete specification, and some parameter checks
do not verify direction or writable arguments. Duplicate declarations are checked
in several disconnected visitors with uneven coverage.

The safest increment is to make lookup queries self-contained, return complete FB
specifications, enforce parameter direction/writability at the shared call checks,
and add a source-level regression matrix for scalar, structure, array, FB-member,
and IN/OUT/IN_OUT behavior.

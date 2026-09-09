# Design

`search_var_instance_decl_c` remains a scoped visitor, but each public query resets
all transient result fields. Declaration visitors return the complete specification
node consistently, leaving base-type resolution to the existing shared helper.

Parameter checks use `function_param_iterator_c` as the declaration authority.
Implicit FB operators accept only input parameters. Call-site lvalue validation is
kept in the lvalue pass so formal and non-formal calls share one rule.

The regression matrix uses compilable IEC fixtures for positive cases and focused
negative fixtures for duplicate declarations, invalid direction, and non-writable
IN_OUT arguments.

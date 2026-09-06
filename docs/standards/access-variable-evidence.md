# Access-variable evidence boundary

This note records the public evidence used for the first `VAR_ACCESS` increment.
It does not reproduce or substitute for IEC 61131-3.

## Public evidence

- Beckhoff's public compiler-error reference gives the declaration shape
  `identifier : access-path : type access-mode`, requires a valid instance path,
  and rejects address assignments on access declarations:
  <https://infosys.beckhoff.com/content/1033/tcplccontrol/925418891.html>
- The OPC Foundation's public IEC 61131-3 information-model mapping preserves
  read/write access metadata and requires constants to be read-only:
  <https://reference.opcfoundation.org/specs/OPC-30000/9.3>
- MATIEC's historical, disabled grammar records `VAR_ACCESS`, typed paths, and the
  `READ_ONLY`/`READ_WRITE` words in `stage1_2/iec_bison.yy`. This is implementation
  history, not independent standards evidence.

Together these sources support a bounded, cross-vendor-provisional implementation
of typed access aliases and direction metadata. They do not establish every path,
visibility, communication-service, or IEC 61131-3:2025 rule.

## Selected increment

The experimental profile accepts configuration-level access declarations whose
target is a simple `VAR_GLOBAL` name declared in the same configuration. The target
must resolve, its named type must match, duplicate exported names are rejected, the
omitted direction defaults to `READ_ONLY`, and a constant cannot be exported as
`READ_WRITE`.

Resource-qualified, program-qualified, function-block, structured, array-element,
and directly represented paths are deferred. This narrower implementation is
deliberate: the current compiler has no complete cross-configuration hierarchical
symbol resolver.

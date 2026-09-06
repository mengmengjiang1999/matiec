# Object-oriented evidence and bounded subset

## Public evidence

Public, legally accessible material establishes the following capability family:

| Capability | Evidence | Classification |
| --- | --- | --- |
| `INTERFACE`, interface methods, `EXTENDS`, `IMPLEMENTS` | Siemens Edition 3 compliance matrix, Tables 51–53 | `official-public` for Edition 3 feature existence |
| `METHOD ... END_METHOD` below a function block | Siemens matrix and Beckhoff Object Method documentation | `cross-vendor-provisional` for syntax details |
| Method access to owner variables | Beckhoff Object Method documentation | vendor-verified behavior |
| Method variables have call-local lifetime | Beckhoff Object Method documentation | vendor-verified behavior |
| Function-block inheritance and override | Siemens matrix and Beckhoff inheritance documentation | `cross-vendor-provisional` for capability only |

Sources:

- Siemens IEC 61131-3 Edition 3 compliance matrix:
  <https://cache.industry.siemens.com/dl/files/748/109476748/att_845621/v1/IEC_61131_compliance_en_US.pdf>
- Beckhoff Object Method:
  <https://infosys.beckhoff.com/content/1033/tc3_plc_intro/2530307467.html>
- Beckhoff Object Function Block:
  <https://infosys.beckhoff.com/content/1033/tc3_plc_intro/2530279563.html>
- Beckhoff Object Interface:
  <https://infosys.beckhoff.com/content/1033/tc3_plc_intro/4256428299.html>
- Beckhoff inheritance principle:
  <https://infosys.beckhoff.com/content/1033/tc3_plc_intro/3537661579.html>

## First bounded increment

The first increment implements only public methods directly owned by a function
block. Calls are statically dispatched using the declared function-block type and
pass the existing owner instance by reference as a final hidden parameter. Method parameters, local variables,
return types, expressions, and statements reuse the legacy function rules.

The following remain outside the increment: `CLASS`, `INTERFACE`, `EXTENDS`,
`IMPLEMENTS`, `OVERRIDE`, `ABSTRACT`, `FINAL`, properties, method overloads,
`THIS`, `SUPER`, private/protected/internal methods, and interface/base references.

## Evidence boundary

The sources do not expose the complete IEC 61131-3:2025 object model, normative
typing rules, layout, vtable rules, or ABI. The selected method lowering is therefore
a documented MATIEC provisional behavior under
`iec61131-3:2025-experimental`, not an IEC conformance claim.

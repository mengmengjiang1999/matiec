# Namespace evidence matrix

## Confirmed capability family

Public, legally accessible material supports the existence of the following
IEC 61131-3 namespace capabilities:

| Capability | Evidence | Classification |
| --- | --- | --- |
| Namespace declaration and usage | IEC 61131-3:2025 public table of contents lists namespace declaration and usage sections | `official-public` |
| `USING` namespace directive | IEC 61131-3:2025 public table of contents and Siemens Edition 3 compliance matrix | `official-public` for existence |
| Public and `INTERNAL` namespaces | Siemens Edition 3 compliance matrix, Table 64 feature list | `cross-vendor-provisional` for detailed behavior |
| Lexically nested and fully qualified declarations | Siemens Edition 3 compliance matrix, Table 65 feature list | `cross-vendor-provisional` |
| `NAMESPACE A.B ... END_NAMESPACE` spelling | Public SIMATIC AX examples | `implementation-verified` for SIMATIC AX only |
| Dot-qualified library/GVL names | CODESYS and TwinCAT documentation | Vendor extension behavior; not IEC evidence |

Sources:

- IEC 61131-3:2025 public product/contents material:
  <https://webstore.iec.ch/en/publication/68533>
- Siemens IEC 61131-3 Edition 3 compliance matrix:
  <https://cache.industry.siemens.com/dl/files/748/109476748/att_845621/v1/IEC_61131_compliance_en_US.pdf>
- Public SIMATIC AX namespace example:
  <https://simatic-ax.github.io/axlp_introduction_to_st/>
- CODESYS library namespace documentation:
  <https://content.helpme-codesys.com/en/LibDevSummary/namespaces.html>
- TwinCAT GVL namespace documentation:
  <https://infosys.beckhoff.com/content/1033/tc3_plc_intro/2529197963.html>

## Rules that remain unresolved

The available public sources do not provide enough consistent detail to freeze:

- the complete declaration grammar and all legal declaration members;
- exact `USING` placement, transitivity, ordering, and duplicate behavior;
- unqualified lookup order and shadowing between local, namespace, and global names;
- ambiguity diagnostics when multiple imports expose the same identifier;
- `INTERNAL` visibility across files, nested namespaces, libraries, and applications;
- namespace reopening, aliasing, collision, and case-folding rules;
- linkage spelling and ABI identity for qualified POUs and types;
- Edition 4 changes relative to Edition 3.

## Provisional implementation decision

The project owner explicitly chose the second path on 2026-09-06: implement a
separately documented MATIEC provisional contract under
`iec61131-3:2025-experimental`. The resulting rules are implementation behavior,
not alleged IEC requirements. They are specified in `namespace-semantics.md` and
may change if authoritative, legally usable evidence later contradicts them.

# Standard-library implementation inventory

Snapshot generated from the repository on 2026-09-06. Counts are implementation
facts, not IEC conformance totals.

| Layer | Source | Current contents |
| --- | --- | --- |
| IEC declarations | `lib/standard_functions.txt` | 1,751 generated overload declarations |
| Declaration generator | `lib/create_standard_function_txt.sh` | source for legacy overload generation |
| C function runtime | `lib/C/iec_std_functions.h` | 92 inline functions and 72 generated-operation macros |
| Standard FB declarations | `lib/standard_FB.txt` plus included files | edge, bistable, counter, timer, RTC and utility FB families |
| String declarations | `lib/standard_functions.txt` | 65 overload declarations across `LEN`, `LEFT`, `RIGHT`, `MID`, `CONCAT`, `INSERT`, `DELETE`, `REPLACE`, and `FIND` |
| Experimental additions | `compiler/modern_library_normalizer.cc` | profile-owned `ASSERT(IN : BOOL) : VOID` source contract |

The checked legacy library does not declare `ASSERT`, `IS_VALID`,
`TO_BIG_ENDIAN`, `TO_LITTLE_ENDIAN`, `FROM_BIG_ENDIAN`, or
`FROM_LITTLE_ENDIAN`. The last five appear in public Edition 3 compliance
material, but are not added by this change because their full overload and ABI
rules have not been reviewed.

The declaration generator and C header remain the legacy source of truth. Modern
entries live in a separate profile-aware front-end layer so selecting the
experimental profile cannot alter legacy overload resolution or generated symbols.

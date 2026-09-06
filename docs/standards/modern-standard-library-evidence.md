# Modern standard-library evidence boundary

## Sources

- The official IEC 61131-3:2025 product page states that Edition 4 includes UTF-8
  strings and associated functions:
  <https://webstore.iec.ch/en/publication/68533>
- The official public preview lists the relevant function tables, including
  character-string functions, validation functions, and `Function ASSERT`:
  <https://www.plcopen.org/download_file/force/421f2ec3-d126-4620-8bf6-b8759ab90d54/342/>
- A public technical comparison by IEC 61131 specialist Stefan Henneken describes
  the Edition 4 source contract as `ASSERT` with one `BOOL` input and no return
  value. It also records that failure notification and the switch from development
  to released behavior are implementation-defined:
  <https://stefanhenneken.net/2025/06/10/iec-61131-3-vergleich-edition-3-und-edition-4/>

The IEC source confirms the function's presence but the complete normative table is
not in the free preview. The signature is therefore classified as
`cross-vendor-provisional`, not `official-public`.

## Selected increment

Only `ASSERT(IN : BOOL) : VOID` is added. Other names and overloads are not inferred
from table titles or a single vendor library. UTF-8 function work remains blocked on
public signature, indexing, capacity, and error-behavior evidence.

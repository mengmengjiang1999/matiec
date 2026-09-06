# Experimental UTF-8 evidence and behavior

## Public evidence

The IEC product page for IEC 61131-3:2025 identifies “UTF-8 strings and their
associated functions” as an Edition 4 addition:
<https://webstore.iec.ch/en/publication/68533>.

The Unicode Core Specification defines well-formed UTF-8 as encodings of Unicode
scalar values using one to four bytes and excludes overlong forms, surrogate
encodings, and values above U+10FFFF. MATIEC follows those encoding validity
rules for source validation:
<https://www.unicode.org/versions/Unicode17.0.0/core-spec/chapter-3/>.

These public sources do not expose the IEC rules for string indexing, slicing,
normalization, conversions, or the names and signatures of the associated
functions. This implementation deliberately does not invent those rules.

## Experimental MATIEC model

Under `--std=iec61131-3:2025-experimental`:

- the entry source and physical include files must be well-formed UTF-8;
- an optional UTF-8 BOM is accepted only at the start of a source file;
- non-ASCII UTF-8 bytes are accepted inside character string literals;
- identifiers remain ASCII-only;
- a `STRING` stores UTF-8 as encoded bytes in the existing `STRING` ABI;
- `STRING.len` is the encoded-byte count and `STR_MAX_LEN` remains 126 bytes;
- generated C spells non-ASCII bytes as `\xHH`, avoiding dependence on the C
  compiler's source encoding;
- no normalization is performed.

Byte-count length is an explicit experimental MATIEC compatibility decision, not
a claim about IEC 61131-3:2025 indexing semantics. `WSTRING` generation remains
outside this increment because the legacy C backend does not implement it.

## Evidence-gated function set

No new source-visible IEC UTF-8 library function is registered in this change.
The public evidence confirms that associated functions exist but does not provide
their normative names, overloads, parameter types, results, or error behavior.
Each function remains blocked until an authoritative, legally usable source can
support its exact contract.

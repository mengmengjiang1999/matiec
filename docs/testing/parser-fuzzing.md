# Parser fuzzing

The parser harness exercises only local compiler inputs. It performs no network
access and does not target external systems. The first input byte selects one of
five public C API paths; the remaining bytes are treated as IEC source:

| Selector modulo 5 | Boundary |
| --- | --- |
| 0 | in-memory source |
| 1 | temporary filesystem source |
| 2 | virtual include contents |
| 3 | pre-cancelled compilation |
| 4 | source, diagnostic, and output limits |

Run the deterministic libFuzzer smoke gate with:

```sh
tests/fuzz/runtests smoke
```

The isolated runner copies and bootstraps the source tree under a temporary
directory, builds with AddressSanitizer, UndefinedBehaviorSanitizer, and
libFuzzer coverage, and removes the build afterward. `periodic` replaces the
fixed run count with a time budget controlled by `FUZZ_SECONDS`.
When a local Clang installation has no libFuzzer runtime, smoke mode reports the
limitation and replays every seed under AddressSanitizer and
UndefinedBehaviorSanitizer; periodic mode requires a complete libFuzzer toolchain.

For AFL++, install AFL's Clang wrappers and build the standalone form outside the
repository:

```sh
tests/fuzz/build-afl /tmp/matiec-parser-afl
afl-fuzz -i tests/fuzz/corpus/parser -o /tmp/matiec-afl-findings -- \
  /tmp/matiec-parser-afl @@
```

Seeds must stay small, deterministic, and attributable to the project. Minimize
new crash reproducers before promoting them to regression fixtures. Do not store
the fuzzer's queue, statistics, or crash directory under
`tests/fuzz/corpus/parser`; CI uploads failure reproducers separately.

## Current state

Memory-backed primary source is copied into a compilation context, but scanner
include pragmas always search filesystem directories with `fopen`. An embedder
therefore cannot compile a fully virtual source graph.

## Options explored

1. Register every virtual file ahead of compilation. Simple, but forces hosts to
   discover the include graph themselves.
2. Add an on-demand resolver callback returning a borrowed source view which the
   compiler copies before returning. This supports IDE and remote stores without
   allocator coupling.
3. Materialize callback data into temporary files. This preserves scanner code
   but violates hermetic operation and adds cleanup/security concerns.

## Conclusion

Use an authoritative on-demand callback. A configured resolver handles every
include request; a miss or callback error fails compilation without filesystem
fallback. Returned storage remains valid until the next resolver call, and the
compiler copies it before that call. The scanner will consume copied bytes
through its existing buffer and tracking stack, with disk behavior unchanged
when the callback is absent.

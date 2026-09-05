# AST and compilation-lifetime ownership inventory

This note records the ownership rules that exist before the AST arena migration.
It is the checklist for OpenSpec change `refactor-compiler-architecture`, tasks
4.1 through 4.6. The current code relies on process lifetime for much of this
memory; the target is one lifetime boundary per `CompilationContext`.

## Current ownership map

| Allocation | Current producer | Current references | Current release | Migration rule |
| --- | --- | --- | --- | --- |
| AST nodes (`symbol_c` subclasses) | Bison actions in `stage1_2/iec_bison.yy`, Stage 3 annotations, and a few generator helpers | Parent/child fields are non-owning raw pointers; `parent`, `token`, `datatype`, `scope`, candidate datatype vectors, and annotation maps also borrow nodes | Usually none; `symbol_c` has a virtual but empty destructor | Allocate all per-compilation nodes in `AstArena`; node links remain non-owning |
| AST list storage (`list_c::elements`) | `list_c` constructors and `realloc` in `absyntax/absyntax.cc` | Owned only by the containing list node; entries borrow child nodes and token text | None because `list_c` has no destructor | Release the backing buffer when the arena destroys the list; do not recursively delete entries |
| Lexer token text | `strdup()` in `stage1_2/iec_flex.ll` | Passed into `token_c::value` and sometimes retained as source filenames or lookup keys | Only short-lived scratch copies are freed; text retained by tokens is not | Copy token text into arena-owned storage and preserve stable `const char *` addresses |
| Source/include filenames | `strdup()` in lexer include handling | Borrowed by source-location fields on every node created while the file is active | Include-stack bookkeeping is partially freed; filenames intentionally survive | Intern/copy filenames into arena-owned storage before assigning node locations |
| Reordered library root | `remove_forward_dependencies_c::create_new_tree()` | A new `library_c` list points at the original AST children; children are shared, not cloned | None | Allocate the wrapper in the same arena; both original and reordered roots borrow the same children |
| Synthetic semantic nodes | Stage 3 creates `ref_spec_c`, generic types, IL operand lists, pragmas, and other annotations | Stored on AST fields or used by later passes | Usually none | Allocate persistent annotations in the arena; keep pass-local temporaries on the stack |
| Generator-created AST-shaped helpers | C generator and utility visitors create temporary identifiers and type nodes | Generally scoped to a visitor operation, but several escape through cached visitor state | Inconsistent; many are not deleted | Prefer ordinary values/stack objects; use the arena only when a pointer must survive the operation |
| Visitors and helper objects | Stack construction, direct `new`, and lazy singleton accessors | Owned by callers, enclosing visitors, or process-wide singleton pointers | Mixed: some enclosing destructors delete helpers; singleton instances usually live forever | Make stateful visitors stack- or context-owned; immutable canonical type objects may remain static |

## Important aliasing constraints

- AST edge fields are observers. Arena destruction, not individual nodes, will own
  recursive lifetime. Destructors must therefore release only private storage and
  never delete referenced child nodes.
- `remove_forward_dependencies_c` deliberately creates two roots that share every
  child node. Treating either root as a recursive owner would double-delete the
  tree.
- `token_c::value`, list lookup keys, and `first_file`/`last_file` are raw character
  pointers whose addresses must remain stable until compilation teardown.
- Static elementary datatype objects in `get_datatype_info_c` are immutable
  canonical sentinels. They are process-lifetime data, not compilation-owned AST,
  and must never be registered with an arena.
- Semantic fields on `symbol_c` currently mix borrowed canonical types with
  compilation-created nodes. Arena teardown may destroy only allocations that it
  created, without following these pointers.

## Stateful process-lifetime objects to remove

The following are stateful or heap-backed singletons and are unsafe for repeated
in-process compilations unless reset. Task 4.4 should replace them with stack or
context-owned instances:

- `pou_count_c` in `stage3/remove_forward_dependencies.cc`;
- `get_var_name_c`, `get_sizeof_datatype_c`, `get_datatype_id_c`,
  `get_datatype_id_str_c`, `get_struct_info_c`, `search_base_type_c`, and the debug
  printers in `absyntax_utils`;
- `generate_datatypes_aliasid_c` and `analyse_variable_c` in the C generator;
- Stage 3 file-static enum symbol tables and their populating visitors;
- function-local static mutable pass state such as declaration-check caches and
  generator configuration counters.

Immutable tables, constants, and canonical elementary datatype objects are not in
this migration set.

## Migration and verification sequence

1. Add an `AstArena` owned by `CompilationContext`. It records node destructors and
   stable string allocations, and destroys them in reverse allocation order.
2. Add focused tests for node destruction, derived destructors, stable strings,
   and independent arena lifetimes.
3. Route parser node construction and retained lexer strings through the active
   compilation arena. Keep an explicit temporary adapter at the generated-parser
   boundary until parser parameters can carry the context directly.
4. Register the reordered root and persistent Stage 3 synthetic nodes with the
   same arena. Convert generator-only temporary nodes to values or stack objects.
5. Replace stateful singletons, then test invalid-to-valid and differing-option
   compilations in one process.
6. Run ASan with leak detection and UBSan. Any remaining process-lifetime allocation
   must be documented as immutable shared data or migrated before task 4.6 closes.

## Completion invariants

- Destroying one `CompilationContext` releases every AST node, retained token
  string, filename, and reordered wrapper allocated for that compilation.
- Destroying one context cannot invalidate another context or an immutable
  canonical datatype object.
- AST relationships and generated output remain unchanged.
- No parser, semantic pass, or generator relies on a previous compilation's
  visitor state.

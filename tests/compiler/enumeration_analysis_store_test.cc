#include "compiler/compilation_context.hh"
#include "stage3/enumeration_analysis_store.hh"

#include <cassert>
#include <iterator>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  library_c *library = context.ast_arena().make<library_c>();
  integer_c *first = context.ast_arena().make<integer_c>("1");
  integer_c *second = context.ast_arena().make<integer_c>("2");
  library->enumvalue_symtable.insert(std::make_pair("Ready", first));
  library->enumvalue_symtable.insert(std::make_pair("READY", second));

  assert(publish_enumeration_analysis(library, context.analysis()));
  assert(context.analysis().enumeration_size() == 1);
  const matiec::AnalysisEntry<matiec::EnumerationAnalysisRecord> *entry =
      context.analysis().enumeration(library);
  assert(entry != nullptr);
  std::pair<symbol_c::enumvalue_symtable_t::const_iterator,
            symbol_c::enumvalue_symtable_t::const_iterator>
      matches = entry->value.values.equal_range("ready");
  assert(std::distance(matches.first, matches.second) == 2);

  library->enumvalue_symtable.clear();
  materialize_enumeration_analysis(library, context.analysis());
  assert(library->enumvalue_symtable.size() == 2);

  matiec::CompilationContext other;
  matiec::ActiveAstArenaScope other_scope(other.ast_arena());
  integer_c *foreign = other.ast_arena().make<integer_c>("3");
  matiec::EnumerationAnalysisRecord invalid = entry->value;
  invalid.values.insert(std::make_pair("Foreign", foreign));
  assert(!context.analysis().set_enumeration(library, invalid));
  assert(context.analysis().enumeration(library)->value.values.size() == 2);

  context.analysis().clear();
  assert(context.analysis().enumeration_size() == 0);
  return 0;
}

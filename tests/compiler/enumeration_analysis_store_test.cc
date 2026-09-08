#include "compiler/compilation_context.hh"

#include <cassert>
#include <iterator>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  library_c *library = context.ast_arena().make<library_c>();
  integer_c *first = context.ast_arena().make<integer_c>("1");
  integer_c *second = context.ast_arena().make<integer_c>("2");
  library->enumvalue_symtable(context.analysis()).insert(std::make_pair("Ready", first));
  library->enumvalue_symtable(context.analysis()).insert(std::make_pair("READY", second));

  assert(context.analysis().enumeration_size() == 1);
  const matiec::AnalysisEntry<matiec::EnumerationAnalysisRecord> *entry =
      context.analysis().enumeration(library);
  assert(entry != nullptr);
  std::pair<symbol_c::enumvalue_symtable_t::const_iterator,
            symbol_c::enumvalue_symtable_t::const_iterator>
      matches = entry->value.values.equal_range("ready");
  assert(std::distance(matches.first, matches.second) == 2);

  matches = context.analysis().enumeration(library)->value.values.equal_range(
      "READY");
  assert(std::distance(matches.first, matches.second) == 2);
  assert(library->enumvalue_symtable(context.analysis()).size() == 2);

  library_c transient;
  transient.enumvalue_symtable(context.analysis()).insert(std::make_pair("Local", first));
  assert(transient.enumvalue_symtable(context.analysis()).size() == 1);

  matiec::CompilationContext other;
  matiec::ActiveAstArenaScope other_scope(other.ast_arena());
  integer_c *foreign = other.ast_arena().make<integer_c>("3");
  matiec::EnumerationAnalysisRecord invalid = entry->value;
  invalid.values.insert(std::make_pair("Foreign", foreign));
  assert(!context.analysis().set_enumeration(library, invalid));
  assert(context.analysis().enumeration(library)->value.values.size() == 2);

  context.analysis().clear();
  assert(context.analysis().enumeration_size() == 0);
  assert(library->enumvalue_symtable(context.analysis()).empty());
  assert(transient.enumvalue_symtable(context.analysis()).empty());
  return 0;
}

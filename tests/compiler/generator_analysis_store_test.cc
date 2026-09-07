#include "compiler/compilation_context.hh"
#include "stage4/generator_analysis_store.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  integer_c *literal = context.ast_arena().make<integer_c>("42");
  identifier_c *implicit_type =
      context.ast_arena().make<identifier_c>("__implicit_1");
  literal->anotations_map["implicit_type"] = implicit_type;

  assert(publish_generator_analysis(literal, context.analysis()));
  assert(context.analysis().generator_size() == 1);
  const matiec::AnalysisEntry<matiec::GeneratorAnalysisRecord> *entry =
      context.analysis().generator(literal);
  assert(entry != nullptr);
  assert(entry->value.symbols.size() == 1);
  assert(entry->value.symbols.at("implicit_type") == implicit_type);

  literal->anotations_map.clear();
  materialize_generator_analysis(literal, context.analysis());
  assert(literal->anotations_map.at("implicit_type") == implicit_type);

  matiec::CompilationContext other;
  matiec::ActiveAstArenaScope other_scope(other.ast_arena());
  identifier_c *foreign = other.ast_arena().make<identifier_c>("foreign");
  matiec::GeneratorAnalysisRecord invalid = entry->value;
  invalid.symbols["foreign"] = foreign;
  assert(!context.analysis().set_generator(literal, invalid));
  assert(context.analysis().generator(literal)->value.symbols.size() == 1);

  context.analysis().clear();
  assert(context.analysis().generator_size() == 0);
  return 0;
}

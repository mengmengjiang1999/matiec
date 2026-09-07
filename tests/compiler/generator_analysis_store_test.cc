#include "compiler/compilation_context.hh"
#include "stage4/generator_analysis_access.hh"
#include "stage4/generator_analysis_store.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  integer_c *literal = context.ast_arena().make<integer_c>("42");
  identifier_c *implicit_type =
      context.ast_arena().make<identifier_c>("__implicit_1");
  stage4out_c output(context.outputs(), "  ", &context.analysis());
  assert(stage4_set_generator_symbol(output, literal, "implicit_type",
                                     implicit_type));
  assert(stage4_generator_symbol(output, literal, "implicit_type") ==
         implicit_type);
  assert(literal->anotations_map.empty());

  assert(context.analysis().generator_size() == 1);
  const matiec::AnalysisEntry<matiec::GeneratorAnalysisRecord> *entry =
      context.analysis().generator(literal);
  assert(entry != nullptr);
  assert(entry->value.symbols.size() == 1);
  assert(entry->value.symbols.at("implicit_type") == implicit_type);

  materialize_generator_analysis(literal, context.analysis());
  assert(literal->anotations_map.at("implicit_type") == implicit_type);

  stage4out_c detached(context.outputs());
  assert(!stage4_set_generator_symbol(detached, literal, "detached",
                                      implicit_type));
  assert(stage4_generator_symbol(detached, literal, "implicit_type") ==
         nullptr);

  matiec::CompilationContext other;
  matiec::ActiveAstArenaScope other_scope(other.ast_arena());
  identifier_c *foreign = other.ast_arena().make<identifier_c>("foreign");
  matiec::GeneratorAnalysisRecord invalid = entry->value;
  invalid.symbols["foreign"] = foreign;
  assert(!context.analysis().set_generator(literal, invalid));
  assert(!context.analysis().set_generator_symbol(literal, "foreign", foreign));
  assert(context.analysis().generator(literal)->value.symbols.size() == 1);

  context.analysis().clear();
  assert(context.analysis().generator_size() == 0);
  return 0;
}

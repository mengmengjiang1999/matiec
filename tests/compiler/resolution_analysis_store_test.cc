#include "compiler/compilation_context.hh"
#include "stage3/resolution_analysis_store.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  function_invocation_c *call =
      context.ast_arena().make<function_invocation_c>();
  integer_c *first = context.ast_arena().make<integer_c>("1");
  integer_c *second = context.ast_arena().make<integer_c>("2");
  call->candidate_functions.push_back(first);
  call->candidate_functions.push_back(second);
  call->called_function_declaration = second;
  call->extensible_param_count = 3;

  assert(publish_declaration_resolution(call, context.analysis()));
  assert(context.analysis().resolution_size() == 1);
  const matiec::AnalysisEntry<matiec::ResolutionAnalysisRecord> *entry =
      context.analysis().resolution(call);
  assert(entry != nullptr);
  assert(entry->value.candidates.size() == 2);
  assert(entry->value.candidates[0] == first);
  assert(entry->value.candidates[1] == second);
  assert(entry->value.declaration == second);
  assert(entry->value.extensible_parameter_count == 3);

  call->candidate_functions.clear();
  call->called_function_declaration = nullptr;
  call->extensible_param_count = 0;
  materialize_declaration_resolution(call, context.analysis());
  assert(call->candidate_functions.size() == 2);
  assert(call->called_function_declaration == second);
  assert(call->extensible_param_count == 3);

  matiec::CompilationContext other;
  matiec::ActiveAstArenaScope other_scope(other.ast_arena());
  integer_c *foreign = other.ast_arena().make<integer_c>("3");
  matiec::ResolutionAnalysisRecord invalid = entry->value;
  invalid.declaration = foreign;
  assert(!context.analysis().set_resolution(call, invalid));
  assert(context.analysis().resolution(call)->value.declaration == second);

  context.analysis().clear();
  assert(context.analysis().resolution_size() == 0);
  return 0;
}

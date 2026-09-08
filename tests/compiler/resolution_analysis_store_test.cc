#include "compiler/compilation_context.hh"
#include "stage4/resolution_analysis_access.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  function_invocation_c *call =
      context.ast_arena().make<function_invocation_c>();
  integer_c *first = context.ast_arena().make<integer_c>("1");
  integer_c *second = context.ast_arena().make<integer_c>("2");
  call->candidate_functions(context.analysis()).push_back(first);
  call->candidate_functions(context.analysis()).push_back(second);
  call->called_function_declaration(context.analysis()) = second;
  call->extensible_param_count(context.analysis()) = 3;

  assert(context.analysis().resolution_size() == 1);
  const matiec::AnalysisEntry<matiec::ResolutionAnalysisRecord> *entry =
      context.analysis().resolution(call);
  assert(entry != nullptr);
  assert(entry->value.candidates.size() == 2);
  assert(entry->value.candidates[0] == first);
  assert(entry->value.candidates[1] == second);
  assert(entry->value.declaration == second);
  assert(entry->value.extensible_parameter_count == 3);

  stage4out_c output(context.outputs(), "  ", &context.analysis());
  const matiec::ResolutionAnalysisRecord *stored =
      stage4_resolution_record(output, call);
  assert(stored != nullptr);
  assert(stored->candidates.size() == 2);
  assert(stored->declaration == second);
  assert(stored->extensible_parameter_count == 3);
  assert(call->candidate_functions(context.analysis()).size() == 2);
  assert(call->called_function_declaration(context.analysis()) == second);
  assert(call->extensible_param_count(context.analysis()) == 3);

  function_invocation_c transient;
  transient.called_function_declaration(context.analysis()) = first;
  assert(transient.called_function_declaration(context.analysis()) == first);

  stage4out_c detached(context.outputs());
  assert(stage4_resolution_record(detached, call) == nullptr);

  matiec::CompilationContext other;
  matiec::ActiveAstArenaScope other_scope(other.ast_arena());
  integer_c *foreign = other.ast_arena().make<integer_c>("3");
  matiec::ResolutionAnalysisRecord invalid = entry->value;
  invalid.declaration = foreign;
  assert(!context.analysis().set_resolution(call, invalid));
  assert(context.analysis().resolution(call)->value.declaration == second);

  context.analysis().clear();
  assert(context.analysis().resolution_size() == 0);
  assert(call->called_function_declaration(context.analysis()) == nullptr);
  assert(transient.called_function_declaration(context.analysis()) == nullptr);
  return 0;
}

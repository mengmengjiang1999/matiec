#include "compiler/compilation_context.hh"
#include "stage3/flow_control_analysis.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  instruction_list_c *instructions =
      context.ast_arena().make<instruction_list_c>();
  il_instruction_c *first =
      context.ast_arena().make<il_instruction_c>(nullptr, nullptr);
  il_instruction_c *second =
      context.ast_arena().make<il_instruction_c>(nullptr, nullptr);
  instructions->add_element(first);
  instructions->add_element(second);

  flow_control_analysis_c analysis(instructions, context.analysis());
  instructions->accept(analysis);

  const matiec::AnalysisEntry<matiec::FlowAnalysisRecord> *first_flow =
      context.analysis().flow(first);
  const matiec::AnalysisEntry<matiec::FlowAnalysisRecord> *second_flow =
      context.analysis().flow(second);
  assert(first_flow != nullptr);
  assert(second_flow != nullptr);
  assert(first_flow->value.predecessors.empty());
  assert(first_flow->value.successors.size() == 1);
  assert(first_flow->value.successors[0] == second);
  assert(second_flow->value.predecessors.size() == 1);
  assert(second_flow->value.predecessors[0] == first);
  assert(second_flow->value.successors.empty());
  assert(first->prev_il_instruction.empty());
  assert(first->next_il_instruction.empty());
  assert(second->prev_il_instruction.empty());
  assert(second->next_il_instruction.empty());

  context.analysis().clear();
  assert(context.analysis().flow_size() == 0);
  return 0;
}

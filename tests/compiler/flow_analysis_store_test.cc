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

  {
    matiec::ActiveAnalysisStoreScope analysis_scope(context.analysis());
    assert(matiec::analysis_flow_predecessors(first).empty());
    assert(matiec::analysis_flow_successors(first).size() == 1);
    assert(matiec::analysis_flow_successors(first)[0] == second);
    assert(matiec::analysis_flow_predecessors(second).size() == 1);
    assert(matiec::analysis_flow_predecessors(second)[0] == first);
    assert(matiec::analysis_flow_successors(second).empty());

    il_instruction_c transient(nullptr, nullptr);
    transient.prev_il_instruction.push_back(first);
    assert(matiec::analysis_flow_predecessors(&transient).size() == 1);
    assert(matiec::analysis_flow_predecessors(&transient)[0] == first);
  }
  assert(matiec::active_analysis_store() == nullptr);

  context.analysis().clear();
  assert(context.analysis().flow_size() == 0);
  return 0;
}

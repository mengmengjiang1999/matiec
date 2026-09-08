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
  {
    assert(matiec::analysis_flow_predecessors(context.analysis(), first).empty());
    assert(matiec::analysis_flow_successors(context.analysis(), first).size() == 1);
    assert(matiec::analysis_flow_successors(context.analysis(), first)[0] == second);
    assert(matiec::analysis_flow_predecessors(context.analysis(), second).size() == 1);
    assert(matiec::analysis_flow_predecessors(context.analysis(), second)[0] == first);
    assert(matiec::analysis_flow_successors(context.analysis(), second).empty());

    il_instruction_c transient(nullptr, nullptr);
    assert(matiec::analysis_flow_predecessors(context.analysis(), &transient).empty());
    matiec::analysis_flow_predecessors_mut(context.analysis(), &transient).push_back(first);
    assert(matiec::analysis_flow_predecessors(context.analysis(), &transient).size() == 1);
    assert(matiec::analysis_flow_predecessors(context.analysis(), &transient)[0] == first);
  }
  context.analysis().clear();
  assert(context.analysis().flow_size() == 0);
  {
    il_instruction_c transient(nullptr, nullptr);
    assert(matiec::analysis_flow_predecessors(context.analysis(), &transient).empty());
  }
  return 0;
}

#include "compiler/analysis_store.hh"
#include "compiler/compilation_context.hh"

#include <cassert>

namespace {

symbol_c *make_node(matiec::CompilationContext &context) {
  return reinterpret_cast<symbol_c *>(context.ast_arena().make<int>(0));
}

}  // namespace

int main() {
  matiec::CompilationContext first;
  matiec::CompilationContext second;
  symbol_c *first_key = make_node(first);
  symbol_c *first_reference = make_node(first);
  symbol_c *second_key = make_node(second);

  matiec::FlowAnalysisRecord flow;
  flow.successors.push_back(first_reference);
  assert(first.analysis().set_flow(first_key, flow));
  assert(first.analysis().flow(first_key) != nullptr);
  assert(first.analysis().flow(first_key)->status ==
         matiec::AnalysisStatus::valid);
  assert(second.analysis().flow(first_key) == nullptr);

  matiec::FlowAnalysisRecord invalid_flow;
  invalid_flow.successors.push_back(second_key);
  assert(!first.analysis().set_flow(first_key, invalid_flow));
  assert(first.analysis().flow(first_key)->value.successors.size() == 1);

  matiec::ConstantAnalysisRecord constant;
  constant.value._int64.set(42);
  assert(first.analysis().set_constant(
      first_reference, constant, matiec::AnalysisStatus::invalid));
  assert(first.analysis().constant(first_reference) != nullptr);
  assert(first.analysis().constant(first_reference)->status ==
         matiec::AnalysisStatus::invalid);
  assert(first.analysis().constant(second_key) == nullptr);
  assert(first.analysis().size() == 2);

  first.analysis().clear();
  assert(first.analysis().size() == 0);
  assert(first.analysis().flow(first_key) == nullptr);
  return 0;
}

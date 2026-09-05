#include "compiler/semantic_pass.hh"

#include <cassert>
#include <cstring>

int main() {
  using matiec::SemanticPassId;
  using matiec::SemanticPassResult;

  const std::vector<SemanticPassId> &order = matiec::default_semantic_pass_order();
  assert(order.size() == 9);
  assert(order.front() == SemanticPassId::enum_declaration);
  assert(order.back() == SemanticPassId::dependency_ordering);

  const std::vector<SemanticPassId> &type_prerequisites =
      matiec::semantic_pass_prerequisites(SemanticPassId::type_safety);
  assert(type_prerequisites.size() == 2);
  assert(type_prerequisites[0] == SemanticPassId::flow_control);
  assert(type_prerequisites[1] == SemanticPassId::constant_propagation);
  assert(matiec::semantic_pass_prerequisites(SemanticPassId::lvalue).front() ==
         SemanticPassId::type_safety);
  assert(std::strcmp(matiec::semantic_pass_name(SemanticPassId::array_range),
                     "array-range") == 0);

  assert(SemanticPassResult::success(SemanticPassId::flow_control).succeeded());
  assert(!SemanticPassResult::failure(SemanticPassId::type_safety, 2).succeeded());
  assert(SemanticPassResult::skip(SemanticPassId::lvalue).skipped);
  return 0;
}

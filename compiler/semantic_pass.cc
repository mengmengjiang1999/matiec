#include "compiler/semantic_pass.hh"

namespace matiec {

bool SemanticPassResult::succeeded() const {
  return !skipped && error_count == 0;
}

SemanticPassResult SemanticPassResult::success(SemanticPassId pass) {
  return SemanticPassResult{pass, 0, false};
}

SemanticPassResult SemanticPassResult::failure(SemanticPassId pass,
                                                std::size_t errors) {
  return SemanticPassResult{pass, errors, false};
}

SemanticPassResult SemanticPassResult::skip(SemanticPassId pass) {
  return SemanticPassResult{pass, 0, true};
}

const char *semantic_pass_name(SemanticPassId pass) {
  switch (pass) {
    case SemanticPassId::enum_declaration: return "enum-declaration";
    case SemanticPassId::flow_control: return "flow-control";
    case SemanticPassId::constant_propagation: return "constant-propagation";
    case SemanticPassId::declaration_safety: return "declaration-safety";
    case SemanticPassId::type_safety: return "type-safety";
    case SemanticPassId::lvalue: return "lvalue";
    case SemanticPassId::array_range: return "array-range";
    case SemanticPassId::case_elements: return "case-elements";
    case SemanticPassId::dependency_ordering: return "dependency-ordering";
  }
  return "unknown";
}

const std::vector<SemanticPassId> &semantic_pass_prerequisites(
    SemanticPassId pass) {
  static const std::vector<SemanticPassId> none;
  static const std::vector<SemanticPassId> flow{
      SemanticPassId::flow_control};
  static const std::vector<SemanticPassId> flow_and_constants{
      SemanticPassId::flow_control, SemanticPassId::constant_propagation};
  static const std::vector<SemanticPassId> types{SemanticPassId::type_safety};
  static const std::vector<SemanticPassId> constants{
      SemanticPassId::constant_propagation};

  switch (pass) {
    case SemanticPassId::constant_propagation: return flow;
    case SemanticPassId::declaration_safety: return flow_and_constants;
    case SemanticPassId::type_safety: return flow_and_constants;
    case SemanticPassId::lvalue: return types;
    case SemanticPassId::array_range: return constants;
    case SemanticPassId::case_elements: return constants;
    case SemanticPassId::enum_declaration:
    case SemanticPassId::flow_control:
    case SemanticPassId::dependency_ordering:
      return none;
  }
  return none;
}

const std::vector<SemanticPassId> &default_semantic_pass_order() {
  static const std::vector<SemanticPassId> order{
      SemanticPassId::enum_declaration,
      SemanticPassId::flow_control,
      SemanticPassId::constant_propagation,
      SemanticPassId::declaration_safety,
      SemanticPassId::type_safety,
      SemanticPassId::lvalue,
      SemanticPassId::array_range,
      SemanticPassId::case_elements,
      SemanticPassId::dependency_ordering};
  return order;
}

}  // namespace matiec

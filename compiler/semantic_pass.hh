#ifndef MATIEC_COMPILER_SEMANTIC_PASS_HH
#define MATIEC_COMPILER_SEMANTIC_PASS_HH

#include <cstddef>
#include <vector>

namespace matiec {

enum class SemanticPassId {
  enum_declaration,
  flow_control,
  constant_propagation,
  declaration_safety,
  type_safety,
  lvalue,
  array_range,
  case_elements,
  dependency_ordering
};

struct SemanticPassResult {
  SemanticPassId pass = SemanticPassId::enum_declaration;
  std::size_t error_count = 0;
  bool skipped = false;

  bool succeeded() const;
  static SemanticPassResult success(SemanticPassId pass);
  static SemanticPassResult failure(SemanticPassId pass, std::size_t errors);
  static SemanticPassResult skip(SemanticPassId pass);
};

const char *semantic_pass_name(SemanticPassId pass);
const std::vector<SemanticPassId> &semantic_pass_prerequisites(SemanticPassId pass);
const std::vector<SemanticPassId> &default_semantic_pass_order();

}  // namespace matiec

#endif

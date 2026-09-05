#ifndef MATIEC_COMPILER_SEMANTIC_PASS_MANAGER_HH
#define MATIEC_COMPILER_SEMANTIC_PASS_MANAGER_HH

#include <functional>
#include <vector>

#include "semantic_pass.hh"

namespace matiec {

class CompilationContext;

struct SemanticPipelineResult {
  std::vector<SemanticPassResult> passes;
  std::size_t error_count = 0;

  bool succeeded() const;
};

class SemanticPassManager {
 public:
  using PassRunner = std::function<SemanticPassResult(CompilationContext &)>;

  explicit SemanticPassManager(
      const std::vector<SemanticPassId> &order = default_semantic_pass_order());

  void register_pass(SemanticPassId pass, PassRunner runner);
  SemanticPipelineResult run(CompilationContext &context) const;
  SemanticPassResult run_pass(
      SemanticPassId pass, CompilationContext &context,
      const std::vector<SemanticPassId> &completed_prerequisites = {}) const;

 private:
  struct RegisteredPass {
    SemanticPassId pass;
    PassRunner runner;
  };

  const PassRunner *find_runner(SemanticPassId pass) const;
  bool prerequisites_satisfied(
      SemanticPassId pass,
      const std::vector<SemanticPassId> &completed_prerequisites) const;

  std::vector<SemanticPassId> order_;
  std::vector<RegisteredPass> registered_passes_;
};

}  // namespace matiec

#endif

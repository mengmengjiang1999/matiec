#include "semantic_pass_manager.hh"

#include <algorithm>
#include <utility>

namespace matiec {

bool SemanticPipelineResult::succeeded() const {
  return error_count == 0 &&
         std::all_of(passes.begin(), passes.end(),
                     [](const SemanticPassResult &result) {
                       return result.succeeded();
                     });
}

SemanticPassManager::SemanticPassManager(
    const std::vector<SemanticPassId> &order)
    : order_(order) {}

void SemanticPassManager::register_pass(SemanticPassId pass,
                                        PassRunner runner) {
  for (RegisteredPass &registered_pass : registered_passes_) {
    if (registered_pass.pass == pass) {
      registered_pass.runner = std::move(runner);
      return;
    }
  }
  registered_passes_.push_back(RegisteredPass{pass, std::move(runner)});
}

SemanticPipelineResult SemanticPassManager::run(
    CompilationContext &context) const {
  SemanticPipelineResult pipeline;
  pipeline.passes.reserve(order_.size());
  std::vector<SemanticPassId> completed;
  bool failed = false;

  for (SemanticPassId pass : order_) {
    SemanticPassResult result = failed
                                    ? SemanticPassResult::skip(pass)
                                    : run_pass(pass, context, completed);
    pipeline.error_count += result.error_count;
    pipeline.passes.push_back(result);
    if (result.succeeded()) {
      completed.push_back(pass);
    } else {
      failed = true;
    }
  }
  return pipeline;
}

SemanticPassResult SemanticPassManager::run_pass(
    SemanticPassId pass, CompilationContext &context,
    const std::vector<SemanticPassId> &completed_prerequisites) const {
  const PassRunner *runner = find_runner(pass);
  if (runner == nullptr ||
      !prerequisites_satisfied(pass, completed_prerequisites)) {
    return SemanticPassResult::skip(pass);
  }
  return (*runner)(context);
}

const SemanticPassManager::PassRunner *SemanticPassManager::find_runner(
    SemanticPassId pass) const {
  for (const RegisteredPass &registered_pass : registered_passes_) {
    if (registered_pass.pass == pass) return &registered_pass.runner;
  }
  return nullptr;
}

bool SemanticPassManager::prerequisites_satisfied(
    SemanticPassId pass,
    const std::vector<SemanticPassId> &completed_prerequisites) const {
  for (SemanticPassId prerequisite : semantic_pass_prerequisites(pass)) {
    if (std::find(completed_prerequisites.begin(),
                  completed_prerequisites.end(), prerequisite) ==
        completed_prerequisites.end()) {
      return false;
    }
  }
  return true;
}

}  // namespace matiec

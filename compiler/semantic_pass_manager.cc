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

SemanticPipelineResult SemanticPassManager::run() const {
  SemanticPipelineResult pipeline;
  pipeline.passes.reserve(order_.size());

  for (SemanticPassId pass : order_) {
    const PassRunner *runner = find_runner(pass);
    SemanticPassResult result = runner == nullptr
                                    ? SemanticPassResult::skip(pass)
                                    : (*runner)();
    pipeline.error_count += result.error_count;
    pipeline.passes.push_back(result);
  }
  return pipeline;
}

const SemanticPassManager::PassRunner *SemanticPassManager::find_runner(
    SemanticPassId pass) const {
  for (const RegisteredPass &registered_pass : registered_passes_) {
    if (registered_pass.pass == pass) return &registered_pass.runner;
  }
  return nullptr;
}

}  // namespace matiec

#include "compiler/semantic_pass.hh"
#include "compiler/semantic_pass_manager.hh"
#include "compiler/compilation_context.hh"

#include <cassert>
#include <cstring>
#include <vector>

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

  matiec::CompilationContext context;
  std::vector<SemanticPassId> executed;
  const std::vector<SemanticPassId> focused_order{
      SemanticPassId::flow_control,
      SemanticPassId::constant_propagation,
      SemanticPassId::declaration_safety};
  matiec::SemanticPassManager ordered_manager(focused_order);
  for (SemanticPassId pass : focused_order) {
    ordered_manager.register_pass(
        pass, [pass, &executed](matiec::CompilationContext &) {
          executed.push_back(pass);
          return SemanticPassResult::success(pass);
        });
  }
  assert(ordered_manager.run(context).succeeded());
  assert(executed == focused_order);

  executed.clear();
  matiec::SemanticPassManager failing_manager(focused_order);
  failing_manager.register_pass(
      SemanticPassId::flow_control,
      [&executed](matiec::CompilationContext &) {
        executed.push_back(SemanticPassId::flow_control);
        return SemanticPassResult::failure(SemanticPassId::flow_control, 1);
      });
  failing_manager.register_pass(
      SemanticPassId::constant_propagation,
      [&executed](matiec::CompilationContext &) {
        executed.push_back(SemanticPassId::constant_propagation);
        return SemanticPassResult::success(
            SemanticPassId::constant_propagation);
      });
  const matiec::SemanticPipelineResult failed = failing_manager.run(context);
  assert(!failed.succeeded());
  assert(failed.error_count == 1);
  assert(executed.size() == 1);
  assert(failed.passes[1].skipped);
  assert(failed.passes[2].skipped);

  int isolated_runs = 0;
  matiec::SemanticPassManager isolated_manager;
  isolated_manager.register_pass(
      SemanticPassId::array_range,
      [&isolated_runs](matiec::CompilationContext &pass_context) {
        ++isolated_runs;
        pass_context.diagnostics().note("array range pass ran");
        return SemanticPassResult::success(SemanticPassId::array_range);
      });
  assert(isolated_manager
             .run_pass(SemanticPassId::array_range, context,
                       {SemanticPassId::constant_propagation})
             .succeeded());
  assert(isolated_runs == 1);
  assert(context.diagnostics().diagnostics().size() == 1);
  assert(isolated_manager
             .run_pass(SemanticPassId::array_range, context)
             .skipped);
  assert(isolated_runs == 1);
  return 0;
}

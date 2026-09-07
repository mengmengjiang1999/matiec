#include "compiler/compilation_context.hh"
#include "stage3/constant_analysis_store.hh"
#include "stage3/constant_folding.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  integer_c *literal = context.ast_arena().make<integer_c>("42");
  constant_propagation_c propagation(literal, context.diagnostics());
  literal->accept(propagation);
  assert(propagation.get_error_count() == 0);
  assert(literal->const_value._int64.is_valid());
  assert(literal->const_value._int64.get() == 42);

  assert(publish_constant_analysis(literal, context.analysis()));
  assert(context.analysis().constant_size() == 1);
  const matiec::AnalysisEntry<matiec::ConstantAnalysisRecord> *entry =
      context.analysis().constant(literal);
  assert(entry != nullptr);
  assert(entry->status == matiec::AnalysisStatus::valid);
  const_value_c stored_value = entry->value.value;
  assert(stored_value._int64.is_valid());
  assert(stored_value._int64.get() == 42);

  literal->const_value._int64.set(7);
  {
    matiec::ActiveAnalysisStoreScope analysis_scope(context.analysis());
    assert(matiec::analysis_constant_value(literal)._int64.get() == 42);

    integer_c transient("9");
    transient.const_value._int64.set(9);
    assert(matiec::analysis_constant_value(&transient)._int64.get() == 9);
  }
  assert(matiec::active_analysis_store() == nullptr);
  assert(literal->const_value._int64.get() == 7);
  context.analysis().clear();
  assert(context.analysis().constant_size() == 0);
  return 0;
}

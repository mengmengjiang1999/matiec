#include "compiler/compilation_context.hh"
#include "stage3/constant_folding.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  integer_c *literal = context.ast_arena().make<integer_c>("42");
  constant_propagation_c propagation(literal, context.diagnostics(),
                                     context.analysis());
  literal->accept(propagation);
  assert(propagation.get_error_count() == 0);
  assert(literal->const_value(context.analysis())._int64.is_valid());
  assert(literal->const_value(context.analysis())._int64.get() == 42);

  assert(context.analysis().constant_size() == 1);
  const matiec::AnalysisEntry<matiec::ConstantAnalysisRecord> *entry =
      context.analysis().constant(literal);
  assert(entry != nullptr);
  assert(entry->status == matiec::AnalysisStatus::valid);
  const_value_c stored_value = entry->value.value;
  assert(stored_value._int64.is_valid());
  assert(stored_value._int64.get() == 42);

  literal->const_value(context.analysis())._int64.set(7);
  assert(matiec::analysis_constant_value(context.analysis(), literal)._int64.get() == 7);

  integer_c transient("9");
  transient.const_value(context.analysis())._int64.set(9);
  assert(matiec::analysis_constant_value(context.analysis(), &transient)._int64.get() == 9);
  assert(literal->const_value(context.analysis())._int64.get() == 7);
  context.analysis().clear();
  assert(context.analysis().constant_size() == 0);
  assert(matiec::analysis_constant_value(context.analysis(), literal)._int64.is_undefined());
  assert(matiec::analysis_constant_value(context.analysis(), &transient)._int64.is_undefined());
  return 0;
}

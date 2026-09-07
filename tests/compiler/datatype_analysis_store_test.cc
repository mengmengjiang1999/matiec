#include "compiler/compilation_context.hh"
#include "stage3/datatype_analysis_store.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  integer_c *literal = context.ast_arena().make<integer_c>("42");
  int_type_name_c *candidate = context.ast_arena().make<int_type_name_c>();
  literal->candidate_datatypes.push_back(candidate);

  assert(publish_datatype_candidates(literal, context.analysis()));
  assert(context.analysis().datatype_size() == 1);
  const matiec::AnalysisEntry<matiec::DatatypeAnalysisRecord> *entry =
      context.analysis().datatype(literal);
  assert(entry != nullptr);
  assert(entry->status == matiec::AnalysisStatus::valid);
  assert(entry->value.candidates.size() == 1);
  assert(entry->value.candidates[0] == candidate);

  literal->candidate_datatypes.clear();
  materialize_datatype_candidates(literal, context.analysis());
  assert(literal->candidate_datatypes.size() == 1);
  assert(literal->candidate_datatypes[0] == candidate);

  int_type_name_c shared_candidate;
  matiec::DatatypeAnalysisRecord shared;
  shared.candidates.push_back(&shared_candidate);
  assert(context.analysis().set_datatype(literal, shared));
  assert(context.analysis().datatype(literal)->value.candidates[0] ==
         &shared_candidate);

  matiec::CompilationContext other;
  matiec::ActiveAstArenaScope other_scope(other.ast_arena());
  int_type_name_c *foreign = other.ast_arena().make<int_type_name_c>();
  matiec::DatatypeAnalysisRecord invalid;
  invalid.candidates.push_back(foreign);
  assert(!context.analysis().set_datatype(literal, invalid));
  assert(context.analysis().datatype_size() == 1);

  literal->candidate_datatypes.clear();
  literal->datatype = &shared_candidate;
  literal->scope = literal;
  assert(publish_selected_datatypes(literal, context.analysis()));
  entry = context.analysis().datatype(literal);
  assert(entry->value.candidates.size() == 1);
  assert(entry->value.selected == &shared_candidate);
  assert(entry->value.scope == literal);

  literal->datatype = nullptr;
  literal->scope = nullptr;
  materialize_selected_datatypes(literal, context.analysis());
  assert(literal->datatype == &shared_candidate);
  assert(literal->scope == literal);

  invalid = entry->value;
  invalid.scope = foreign;
  assert(!context.analysis().set_datatype(literal, invalid));
  assert(context.analysis().datatype(literal)->value.scope == literal);

  context.analysis().clear();
  assert(context.analysis().datatype_size() == 0);
  return 0;
}

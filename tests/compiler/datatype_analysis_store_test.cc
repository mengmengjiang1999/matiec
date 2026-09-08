#include "compiler/compilation_context.hh"

#include <cassert>

int main() {
  matiec::CompilationContext context;
  matiec::ActiveAstArenaScope arena_scope(context.ast_arena());
  integer_c *literal = context.ast_arena().make<integer_c>("42");
  int_type_name_c *candidate = context.ast_arena().make<int_type_name_c>();
  literal->candidate_datatypes(context.analysis()).push_back(candidate);

  assert(context.analysis().datatype_size() == 1);
  const matiec::AnalysisEntry<matiec::DatatypeAnalysisRecord> *entry =
      context.analysis().datatype(literal);
  assert(entry != nullptr);
  assert(entry->status == matiec::AnalysisStatus::valid);
  assert(entry->value.candidates.size() == 1);
  assert(entry->value.candidates[0] == candidate);

  assert(matiec::analysis_datatype_candidates(context.analysis(), literal).size() == 1);
  assert(matiec::analysis_datatype_candidates(context.analysis(), literal)[0] == candidate);

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

  literal->candidate_datatypes(context.analysis()).clear();
  literal->datatype(context.analysis()) = &shared_candidate;
  literal->scope(context.analysis()) = literal;
  entry = context.analysis().datatype(literal);
  assert(entry->value.candidates.empty());
  assert(entry->value.selected == &shared_candidate);
  assert(entry->value.scope == literal);

  assert(matiec::analysis_selected_datatype(context.analysis(), literal) == &shared_candidate);
  assert(matiec::analysis_scope(context.analysis(), literal) == literal);

  integer_c transient("7");
  transient.candidate_datatypes(context.analysis()).push_back(&shared_candidate);
  transient.datatype(context.analysis()) = &shared_candidate;
  transient.scope(context.analysis()) = literal;
  assert(context.analysis().datatype(&transient) == nullptr);
  assert(matiec::analysis_datatype_candidates(context.analysis(), &transient).size() == 1);
  assert(matiec::analysis_selected_datatype(context.analysis(), &transient) == &shared_candidate);
  assert(matiec::analysis_scope(context.analysis(), &transient) == literal);
  assert(context.analysis().validate_datatypes());

  invalid = entry->value;
  invalid.scope = foreign;
  assert(!context.analysis().set_datatype(literal, invalid));
  assert(context.analysis().datatype(literal)->value.scope == literal);

  context.analysis().clear();
  assert(context.analysis().datatype_size() == 0);
  assert(matiec::analysis_datatype_candidates(context.analysis(), &transient).empty());
  assert(matiec::analysis_selected_datatype(context.analysis(), &transient) == nullptr);
  assert(matiec::analysis_scope(context.analysis(), &transient) == nullptr);
  return 0;
}

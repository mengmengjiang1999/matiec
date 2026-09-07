#include "compiler/analysis_store.hh"

#include <utility>

namespace matiec {

AnalysisStore::AnalysisStore(const AstArena &arena) : arena_(arena) {}

bool AnalysisStore::owns(const symbol_c *node) const {
  return node == nullptr || arena_.owns(node);
}

bool AnalysisStore::owns_all(const std::vector<symbol_c *> &nodes) const {
  for (symbol_c *node : nodes)
    if (!owns(node)) return false;
  return true;
}

bool AnalysisStore::valid(const FlowAnalysisRecord &record) const {
  return owns_all(record.predecessors) && owns_all(record.successors);
}

bool AnalysisStore::valid(const ConstantAnalysisRecord &) const { return true; }

bool AnalysisStore::valid(const DatatypeAnalysisRecord &record) const {
  return owns_all(record.candidates) && owns(record.selected) && owns(record.scope);
}

bool AnalysisStore::valid(const ResolutionAnalysisRecord &record) const {
  return owns_all(record.candidates) && owns(record.declaration);
}

bool AnalysisStore::valid(const EnumerationAnalysisRecord &record) const {
  for (const auto &value : record.values)
    if (!owns(value.second)) return false;
  return true;
}

bool AnalysisStore::valid(const GeneratorAnalysisRecord &record) const {
  for (const auto &value : record.symbols)
    if (!owns(value.second)) return false;
  return true;
}

bool AnalysisStore::set_flow(const symbol_c *key, FlowAnalysisRecord record,
                             AnalysisStatus status) {
  return set(flow_, key, std::move(record), status);
}

bool AnalysisStore::set_constant(const symbol_c *key,
                                 ConstantAnalysisRecord record,
                                 AnalysisStatus status) {
  return set(constants_, key, std::move(record), status);
}

bool AnalysisStore::set_datatype(const symbol_c *key,
                                 DatatypeAnalysisRecord record,
                                 AnalysisStatus status) {
  return set(datatypes_, key, std::move(record), status);
}

bool AnalysisStore::set_resolution(const symbol_c *key,
                                   ResolutionAnalysisRecord record,
                                   AnalysisStatus status) {
  return set(resolutions_, key, std::move(record), status);
}

bool AnalysisStore::set_enumeration(const symbol_c *key,
                                    EnumerationAnalysisRecord record,
                                    AnalysisStatus status) {
  return set(enumerations_, key, std::move(record), status);
}

bool AnalysisStore::set_generator(const symbol_c *key,
                                  GeneratorAnalysisRecord record,
                                  AnalysisStatus status) {
  return set(generators_, key, std::move(record), status);
}

const AnalysisEntry<FlowAnalysisRecord> *AnalysisStore::flow(
    const symbol_c *key) const { return find(flow_, key); }
const AnalysisEntry<ConstantAnalysisRecord> *AnalysisStore::constant(
    const symbol_c *key) const { return find(constants_, key); }
const AnalysisEntry<DatatypeAnalysisRecord> *AnalysisStore::datatype(
    const symbol_c *key) const { return find(datatypes_, key); }
const AnalysisEntry<ResolutionAnalysisRecord> *AnalysisStore::resolution(
    const symbol_c *key) const { return find(resolutions_, key); }
const AnalysisEntry<EnumerationAnalysisRecord> *AnalysisStore::enumeration(
    const symbol_c *key) const { return find(enumerations_, key); }
const AnalysisEntry<GeneratorAnalysisRecord> *AnalysisStore::generator(
    const symbol_c *key) const { return find(generators_, key); }

std::size_t AnalysisStore::size() const {
  return flow_.size() + constants_.size() + datatypes_.size() +
         resolutions_.size() + enumerations_.size() + generators_.size();
}

void AnalysisStore::clear() {
  flow_.clear();
  constants_.clear();
  datatypes_.clear();
  resolutions_.clear();
  enumerations_.clear();
  generators_.clear();
}

}  // namespace matiec

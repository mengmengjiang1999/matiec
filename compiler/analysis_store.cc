#include "compiler/analysis_store.hh"

#include <utility>

namespace matiec {

AnalysisStore::AnalysisStore(const AstArena &arena) : arena_(arena) {}

bool AnalysisStore::add_flow_edge(symbol_c *predecessor, symbol_c *successor,
                                  bool insert_front) {
  if (!owns(predecessor) || !owns(successor) || predecessor == nullptr ||
      successor == nullptr)
    return false;
  AnalysisEntry<FlowAnalysisRecord> &previous = flow_[predecessor];
  AnalysisEntry<FlowAnalysisRecord> &next = flow_[successor];
  if (insert_front) {
    previous.value.successors.insert(previous.value.successors.begin(), successor);
    next.value.predecessors.insert(next.value.predecessors.begin(), predecessor);
  } else {
    previous.value.successors.push_back(successor);
    next.value.predecessors.push_back(predecessor);
  }
  return true;
}

bool AnalysisStore::owns(const symbol_c *node) const {
  return node == nullptr || arena_.owns(node);
}

bool AnalysisStore::owns_or_is_shared(const symbol_c *node) const {
  return owns(node) || (node != nullptr && node->arena_owner_ == nullptr);
}

bool AnalysisStore::owns_all(const std::vector<symbol_c *> &nodes) const {
  for (symbol_c *node : nodes)
    if (!owns(node)) return false;
  return true;
}

bool AnalysisStore::owns_or_is_shared_all(
    const std::vector<symbol_c *> &nodes) const {
  for (symbol_c *node : nodes)
    if (!owns_or_is_shared(node)) return false;
  return true;
}

bool AnalysisStore::valid(const FlowAnalysisRecord &record) const {
  return owns_all(record.predecessors) && owns_all(record.successors);
}

bool AnalysisStore::valid(const ConstantAnalysisRecord &) const { return true; }

bool AnalysisStore::valid(const DatatypeAnalysisRecord &record) const {
  return owns_or_is_shared_all(record.candidates) &&
         owns_or_is_shared(record.selected) && owns(record.scope);
}

bool AnalysisStore::valid(const ResolutionAnalysisRecord &record) const {
  return owns_or_is_shared_all(record.candidates) &&
         owns_or_is_shared(record.declaration);
}

bool AnalysisStore::valid(const EnumerationAnalysisRecord &record) const {
  for (const auto &value : record.values)
    if (!owns_or_is_shared(value.second)) return false;
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

std::size_t AnalysisStore::flow_size() const { return flow_.size(); }

std::size_t AnalysisStore::constant_size() const { return constants_.size(); }

std::size_t AnalysisStore::datatype_size() const { return datatypes_.size(); }

std::size_t AnalysisStore::resolution_size() const { return resolutions_.size(); }

std::size_t AnalysisStore::enumeration_size() const {
  return enumerations_.size();
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

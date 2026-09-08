#include "compiler/analysis_store.hh"

#include <cassert>
#include <utility>

namespace matiec {

namespace {

thread_local AnalysisStore *current_analysis_store = nullptr;

}  // namespace

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
    if (!owns_or_is_shared(value.second)) return false;
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

bool AnalysisStore::set_generator_symbol(const symbol_c *key,
                                         const std::string &name,
                                         symbol_c *value) {
  if (key == nullptr || !owns(key) || !owns_or_is_shared(value)) return false;
  generators_[key].value.symbols[name] = value;
  return true;
}

FlowAnalysisRecord &AnalysisStore::flow_working(symbol_c *key) {
  if (key != nullptr && arena_.owns(key)) return flow_[key].value;
  return transient_flow_[key];
}

const FlowAnalysisRecord *AnalysisStore::flow_working(
    const symbol_c *key) const {
  const AnalysisEntry<FlowAnalysisRecord> *persistent = flow(key);
  if (persistent != nullptr) return &persistent->value;
  std::unordered_map<const symbol_c *, FlowAnalysisRecord>::const_iterator
      transient = transient_flow_.find(key);
  return transient == transient_flow_.end() ? nullptr : &transient->second;
}

ConstantAnalysisRecord &AnalysisStore::constant_working(symbol_c *key) {
  if (key != nullptr && arena_.owns(key)) return constants_[key].value;
  return transient_constants_[key];
}

const ConstantAnalysisRecord *AnalysisStore::constant_working(
    const symbol_c *key) const {
  const AnalysisEntry<ConstantAnalysisRecord> *persistent = constant(key);
  if (persistent != nullptr) return &persistent->value;
  std::unordered_map<const symbol_c *, ConstantAnalysisRecord>::const_iterator
      transient = transient_constants_.find(key);
  return transient == transient_constants_.end() ? nullptr : &transient->second;
}

DatatypeAnalysisRecord &AnalysisStore::datatype_working(symbol_c *key) {
  if (key != nullptr && arena_.owns(key)) return datatypes_[key].value;
  return transient_datatypes_[key];
}

const DatatypeAnalysisRecord *AnalysisStore::datatype_working(
    const symbol_c *key) const {
  const AnalysisEntry<DatatypeAnalysisRecord> *persistent = datatype(key);
  if (persistent != nullptr) return &persistent->value;
  std::unordered_map<const symbol_c *, DatatypeAnalysisRecord>::const_iterator
      transient = transient_datatypes_.find(key);
  return transient == transient_datatypes_.end() ? nullptr : &transient->second;
}

bool AnalysisStore::validate_datatypes() const {
  for (const auto &entry : datatypes_)
    if (!valid(entry.second.value)) return false;
  return true;
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

symbol_c *AnalysisStore::generator_symbol(const symbol_c *key,
                                          const std::string &name) const {
  const AnalysisEntry<GeneratorAnalysisRecord> *entry = generator(key);
  if (entry == nullptr) return nullptr;
  std::map<std::string, symbol_c *>::const_iterator value =
      entry->value.symbols.find(name);
  return value == entry->value.symbols.end() ? nullptr : value->second;
}

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

std::size_t AnalysisStore::generator_size() const { return generators_.size(); }

void AnalysisStore::clear() {
  flow_.clear();
  transient_flow_.clear();
  constants_.clear();
  transient_constants_.clear();
  datatypes_.clear();
  transient_datatypes_.clear();
  resolutions_.clear();
  enumerations_.clear();
  generators_.clear();
}

ActiveAnalysisStoreScope::ActiveAnalysisStoreScope(AnalysisStore &analysis)
    : previous_(current_analysis_store) {
  current_analysis_store = &analysis;
}

ActiveAnalysisStoreScope::~ActiveAnalysisStoreScope() {
  current_analysis_store = previous_;
}

AnalysisStore *active_analysis_store() { return current_analysis_store; }

const FlowAnalysisRecord *analysis_flow(const symbol_c *symbol) {
  if (current_analysis_store == nullptr) return nullptr;
  return current_analysis_store->flow_working(symbol);
}

const std::vector<symbol_c *> &analysis_flow_predecessors(
    const symbol_c *symbol) {
  static const std::vector<symbol_c *> empty;
  if (symbol == nullptr) return empty;
  const FlowAnalysisRecord *record = analysis_flow(symbol);
  return record == nullptr ? empty : record->predecessors;
}

const std::vector<symbol_c *> &analysis_flow_predecessors(
    const il_instruction_c *symbol) {
  return analysis_flow_predecessors(static_cast<const symbol_c *>(symbol));
}

const std::vector<symbol_c *> &analysis_flow_predecessors(
    const il_simple_instruction_c *symbol) {
  return analysis_flow_predecessors(static_cast<const symbol_c *>(symbol));
}

const std::vector<symbol_c *> &analysis_flow_successors(
    const symbol_c *symbol) {
  static const std::vector<symbol_c *> empty;
  if (symbol == nullptr) return empty;
  const FlowAnalysisRecord *record = analysis_flow(symbol);
  return record == nullptr ? empty : record->successors;
}

const std::vector<symbol_c *> &analysis_flow_successors(
    const il_instruction_c *symbol) {
  return analysis_flow_successors(static_cast<const symbol_c *>(symbol));
}

const std::vector<symbol_c *> &analysis_flow_successors(
    const il_simple_instruction_c *symbol) {
  return analysis_flow_successors(static_cast<const symbol_c *>(symbol));
}

std::vector<symbol_c *> &analysis_flow_predecessors_mut(symbol_c *symbol) {
  assert(current_analysis_store != nullptr && symbol != nullptr);
  return current_analysis_store->flow_working(symbol).predecessors;
}

std::vector<symbol_c *> &analysis_flow_successors_mut(symbol_c *symbol) {
  assert(current_analysis_store != nullptr && symbol != nullptr);
  return current_analysis_store->flow_working(symbol).successors;
}

const const_value_c &analysis_constant_value(const symbol_c *symbol) {
  static const const_value_c empty;
  if (symbol == nullptr) return empty;
  if (current_analysis_store == nullptr) return empty;
  const ConstantAnalysisRecord *record =
      current_analysis_store->constant_working(symbol);
  return record == nullptr ? empty : record->value;
}

const_value_c &analysis_constant_value_mut(symbol_c *symbol) {
  assert(current_analysis_store != nullptr && symbol != nullptr);
  return current_analysis_store->constant_working(symbol).value;
}

const DatatypeAnalysisRecord *analysis_datatype(const symbol_c *symbol) {
  if (current_analysis_store == nullptr) return nullptr;
  const AnalysisEntry<DatatypeAnalysisRecord> *entry =
      current_analysis_store->datatype(symbol);
  return entry == nullptr ? nullptr : &entry->value;
}

const std::vector<symbol_c *> &analysis_datatype_candidates(
    const symbol_c *symbol) {
  static const std::vector<symbol_c *> empty;
  if (symbol == nullptr) return empty;
  if (current_analysis_store == nullptr) return empty;
  const DatatypeAnalysisRecord *record =
      current_analysis_store->datatype_working(symbol);
  return record == nullptr ? empty : record->candidates;
}

std::vector<symbol_c *> &analysis_datatype_candidates_mut(symbol_c *symbol) {
  assert(current_analysis_store != nullptr && symbol != nullptr);
  return current_analysis_store->datatype_working(symbol).candidates;
}

symbol_c *analysis_selected_datatype(const symbol_c *symbol) {
  if (symbol == nullptr) return nullptr;
  if (current_analysis_store == nullptr) return nullptr;
  const DatatypeAnalysisRecord *record =
      current_analysis_store->datatype_working(symbol);
  return record == nullptr ? nullptr : record->selected;
}

symbol_c *&analysis_selected_datatype_ref(symbol_c *symbol) {
  assert(current_analysis_store != nullptr && symbol != nullptr);
  return current_analysis_store->datatype_working(symbol).selected;
}

symbol_c *analysis_scope(const symbol_c *symbol) {
  if (symbol == nullptr) return nullptr;
  if (current_analysis_store == nullptr) return nullptr;
  const DatatypeAnalysisRecord *record =
      current_analysis_store->datatype_working(symbol);
  return record == nullptr ? nullptr : record->scope;
}

symbol_c *&analysis_scope_ref(symbol_c *symbol) {
  assert(current_analysis_store != nullptr && symbol != nullptr);
  return current_analysis_store->datatype_working(symbol).scope;
}

}  // namespace matiec

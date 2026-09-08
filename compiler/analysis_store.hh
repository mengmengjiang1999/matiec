#ifndef MATIEC_COMPILER_ANALYSIS_STORE_HH
#define MATIEC_COMPILER_ANALYSIS_STORE_HH

#include "../absyntax/absyntax.hh"
#include "ast_arena.hh"

#include <cstddef>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace matiec {

enum class AnalysisStatus { valid, invalid };

template <typename Record>
struct AnalysisEntry {
  AnalysisStatus status = AnalysisStatus::valid;
  Record value;
};

struct FlowAnalysisRecord {
  std::vector<symbol_c *> predecessors;
  std::vector<symbol_c *> successors;
};

struct ConstantAnalysisRecord {
  const_value_c value;
};

struct DatatypeAnalysisRecord {
  std::vector<symbol_c *> candidates;
  symbol_c *selected = nullptr;
  symbol_c *scope = nullptr;
};

struct ResolutionAnalysisRecord {
  std::vector<symbol_c *> candidates;
  symbol_c *declaration = nullptr;
  int extensible_parameter_count = 0;
};

struct EnumerationAnalysisRecord {
  symbol_c::enumvalue_symtable_t values;
};

struct GeneratorAnalysisRecord {
  std::map<std::string, symbol_c *> symbols;
};

class AnalysisStore {
 public:
  explicit AnalysisStore(const AstArena &arena);

  bool set_flow(const symbol_c *key, FlowAnalysisRecord record,
                AnalysisStatus status = AnalysisStatus::valid);
  bool add_flow_edge(symbol_c *predecessor, symbol_c *successor,
                     bool insert_front);
  bool set_constant(const symbol_c *key, ConstantAnalysisRecord record,
                    AnalysisStatus status = AnalysisStatus::valid);
  bool set_datatype(const symbol_c *key, DatatypeAnalysisRecord record,
                    AnalysisStatus status = AnalysisStatus::valid);
  bool set_resolution(const symbol_c *key, ResolutionAnalysisRecord record,
                      AnalysisStatus status = AnalysisStatus::valid);
  bool set_enumeration(const symbol_c *key, EnumerationAnalysisRecord record,
                       AnalysisStatus status = AnalysisStatus::valid);
  bool set_generator(const symbol_c *key, GeneratorAnalysisRecord record,
                     AnalysisStatus status = AnalysisStatus::valid);
  bool set_generator_symbol(const symbol_c *key, const std::string &name,
                            symbol_c *value);
  DatatypeAnalysisRecord &datatype_working(symbol_c *key);
  const DatatypeAnalysisRecord *datatype_working(
      const symbol_c *key) const;
  bool validate_datatypes() const;

  const AnalysisEntry<FlowAnalysisRecord> *flow(const symbol_c *key) const;
  const AnalysisEntry<ConstantAnalysisRecord> *constant(const symbol_c *key) const;
  const AnalysisEntry<DatatypeAnalysisRecord> *datatype(const symbol_c *key) const;
  const AnalysisEntry<ResolutionAnalysisRecord> *resolution(const symbol_c *key) const;
  const AnalysisEntry<EnumerationAnalysisRecord> *enumeration(const symbol_c *key) const;
  const AnalysisEntry<GeneratorAnalysisRecord> *generator(const symbol_c *key) const;
  symbol_c *generator_symbol(const symbol_c *key,
                             const std::string &name) const;

  std::size_t size() const;
  std::size_t flow_size() const;
  std::size_t constant_size() const;
  std::size_t datatype_size() const;
  std::size_t resolution_size() const;
  std::size_t enumeration_size() const;
  std::size_t generator_size() const;
  void clear();

 private:
  template <typename Record>
  using Table = std::unordered_map<const symbol_c *, AnalysisEntry<Record>>;

  template <typename Record>
  const AnalysisEntry<Record> *find(const Table<Record> &table,
                                    const symbol_c *key) const {
    typename Table<Record>::const_iterator entry = table.find(key);
    return entry == table.end() ? nullptr : &entry->second;
  }

  bool owns(const symbol_c *node) const;
  bool owns_or_is_shared(const symbol_c *node) const;
  bool owns_all(const std::vector<symbol_c *> &nodes) const;
  bool owns_or_is_shared_all(const std::vector<symbol_c *> &nodes) const;
  bool valid(const FlowAnalysisRecord &record) const;
  bool valid(const ConstantAnalysisRecord &) const;
  bool valid(const DatatypeAnalysisRecord &record) const;
  bool valid(const ResolutionAnalysisRecord &record) const;
  bool valid(const EnumerationAnalysisRecord &record) const;
  bool valid(const GeneratorAnalysisRecord &record) const;

  template <typename Record>
  bool set(Table<Record> &table, const symbol_c *key, Record record,
           AnalysisStatus status) {
    if (!owns(key) || !valid(record)) return false;
    table[key] = AnalysisEntry<Record>{status, std::move(record)};
    return true;
  }

  const AstArena &arena_;
  Table<FlowAnalysisRecord> flow_;
  Table<ConstantAnalysisRecord> constants_;
  Table<DatatypeAnalysisRecord> datatypes_;
  std::unordered_map<const symbol_c *, DatatypeAnalysisRecord>
      transient_datatypes_;
  Table<ResolutionAnalysisRecord> resolutions_;
  Table<EnumerationAnalysisRecord> enumerations_;
  Table<GeneratorAnalysisRecord> generators_;
};

class ActiveAnalysisStoreScope {
 public:
  explicit ActiveAnalysisStoreScope(AnalysisStore &analysis);
  ~ActiveAnalysisStoreScope();

  ActiveAnalysisStoreScope(const ActiveAnalysisStoreScope &) = delete;
  ActiveAnalysisStoreScope &operator=(const ActiveAnalysisStoreScope &) = delete;

 private:
  AnalysisStore *previous_;
};

AnalysisStore *active_analysis_store();
const FlowAnalysisRecord *analysis_flow(const symbol_c *symbol);
const std::vector<symbol_c *> &analysis_flow_predecessors(
    const symbol_c *symbol);
const std::vector<symbol_c *> &analysis_flow_predecessors(
    const il_instruction_c *symbol);
const std::vector<symbol_c *> &analysis_flow_predecessors(
    const il_simple_instruction_c *symbol);
const std::vector<symbol_c *> &analysis_flow_successors(
    const symbol_c *symbol);
const std::vector<symbol_c *> &analysis_flow_successors(
    const il_instruction_c *symbol);
const std::vector<symbol_c *> &analysis_flow_successors(
    const il_simple_instruction_c *symbol);
const const_value_c &analysis_constant_value(const symbol_c *symbol);
const DatatypeAnalysisRecord *analysis_datatype(const symbol_c *symbol);
const std::vector<symbol_c *> &analysis_datatype_candidates(
    const symbol_c *symbol);
std::vector<symbol_c *> &analysis_datatype_candidates_mut(symbol_c *symbol);
symbol_c *analysis_selected_datatype(const symbol_c *symbol);
symbol_c *&analysis_selected_datatype_ref(symbol_c *symbol);
symbol_c *analysis_scope(const symbol_c *symbol);
symbol_c *&analysis_scope_ref(symbol_c *symbol);

}  // namespace matiec

#endif

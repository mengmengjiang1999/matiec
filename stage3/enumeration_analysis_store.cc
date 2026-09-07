#include "enumeration_analysis_store.hh"

#include "../absyntax/visitor.hh"
#include "../compiler/analysis_store.hh"

namespace {

class publish_enumerations_c : public iterator_visitor_c {
 public:
  explicit publish_enumerations_c(matiec::AnalysisStore &analysis)
      : analysis_(analysis) {}

  void publish(symbol_c *key, const symbol_c::enumvalue_symtable_t &values) {
    matiec::EnumerationAnalysisRecord record;
    record.values = values;
    if (!analysis_.set_enumeration(key, record)) succeeded_ = false;
  }

#define ENUMERATION_VISIT(class_name)                                         \
  void *visit(class_name *symbol) override {                                  \
    publish(symbol, symbol->enumvalue_symtable);                              \
    return iterator_visitor_c::visit(symbol);                                 \
  }
  ENUMERATION_VISIT(library_c)
  ENUMERATION_VISIT(function_declaration_c)
  ENUMERATION_VISIT(function_block_declaration_c)
  ENUMERATION_VISIT(program_declaration_c)
  ENUMERATION_VISIT(configuration_declaration_c)
  ENUMERATION_VISIT(resource_declaration_c)
#undef ENUMERATION_VISIT

  bool succeeded() const { return succeeded_; }

 private:
  matiec::AnalysisStore &analysis_;
  bool succeeded_ = true;
};

class materialize_enumerations_c : public iterator_visitor_c {
 public:
  explicit materialize_enumerations_c(const matiec::AnalysisStore &analysis)
      : analysis_(analysis) {}

  void materialize(symbol_c *key, symbol_c::enumvalue_symtable_t &values) {
    const matiec::AnalysisEntry<matiec::EnumerationAnalysisRecord> *entry =
        analysis_.enumeration(key);
    if (entry != nullptr) values = entry->value.values;
  }

#define ENUMERATION_VISIT(class_name)                                         \
  void *visit(class_name *symbol) override {                                  \
    materialize(symbol, symbol->enumvalue_symtable);                          \
    return iterator_visitor_c::visit(symbol);                                 \
  }
  ENUMERATION_VISIT(library_c)
  ENUMERATION_VISIT(function_declaration_c)
  ENUMERATION_VISIT(function_block_declaration_c)
  ENUMERATION_VISIT(program_declaration_c)
  ENUMERATION_VISIT(configuration_declaration_c)
  ENUMERATION_VISIT(resource_declaration_c)
#undef ENUMERATION_VISIT

 private:
  const matiec::AnalysisStore &analysis_;
};

}  // namespace

bool publish_enumeration_analysis(symbol_c *tree_root,
                                  matiec::AnalysisStore &analysis) {
  publish_enumerations_c publisher(analysis);
  tree_root->accept(publisher);
  return publisher.succeeded();
}

void materialize_enumeration_analysis(
    symbol_c *tree_root, const matiec::AnalysisStore &analysis) {
  materialize_enumerations_c materializer(analysis);
  tree_root->accept(materializer);
}

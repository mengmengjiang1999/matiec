#include "datatype_analysis_store.hh"

#include "../absyntax/visitor.hh"
#include "../compiler/analysis_store.hh"

namespace {

class publish_candidates_c : public iterator_visitor_c {
 public:
  explicit publish_candidates_c(matiec::AnalysisStore &analysis)
      : analysis_(analysis) {}

  void publish(symbol_c *symbol) {
    matiec::DatatypeAnalysisRecord record;
    const matiec::AnalysisEntry<matiec::DatatypeAnalysisRecord> *existing =
        analysis_.datatype(symbol);
    if (existing != nullptr) record = existing->value;
    record.candidates = symbol->candidate_datatypes;
    if (!analysis_.set_datatype(symbol, record)) succeeded_ = false;
  }

#define DATATYPE_VISIT(class_name)                                            \
  void *visit(class_name *symbol) override {                                  \
    publish(symbol);                                                          \
    return iterator_visitor_c::visit(symbol);                                 \
  }
#define SYM_LIST(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_TOKEN(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF0(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF1(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF2(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF3(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF4(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF5(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF6(class_name, ...) DATATYPE_VISIT(class_name)
#include "../absyntax/absyntax.def"
#undef SYM_LIST
#undef SYM_TOKEN
#undef SYM_REF0
#undef SYM_REF1
#undef SYM_REF2
#undef SYM_REF3
#undef SYM_REF4
#undef SYM_REF5
#undef SYM_REF6
#undef DATATYPE_VISIT

  bool succeeded() const { return succeeded_; }

 private:
  matiec::AnalysisStore &analysis_;
  bool succeeded_ = true;
};

class materialize_candidates_c : public iterator_visitor_c {
 public:
  explicit materialize_candidates_c(const matiec::AnalysisStore &analysis)
      : analysis_(analysis) {}

  void materialize(symbol_c *symbol) {
    const matiec::AnalysisEntry<matiec::DatatypeAnalysisRecord> *entry =
        analysis_.datatype(symbol);
    if (entry != nullptr) symbol->candidate_datatypes = entry->value.candidates;
  }

#define DATATYPE_VISIT(class_name)                                            \
  void *visit(class_name *symbol) override {                                  \
    materialize(symbol);                                                      \
    return iterator_visitor_c::visit(symbol);                                 \
  }
#define SYM_LIST(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_TOKEN(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF0(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF1(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF2(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF3(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF4(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF5(class_name, ...) DATATYPE_VISIT(class_name)
#define SYM_REF6(class_name, ...) DATATYPE_VISIT(class_name)
#include "../absyntax/absyntax.def"
#undef SYM_LIST
#undef SYM_TOKEN
#undef SYM_REF0
#undef SYM_REF1
#undef SYM_REF2
#undef SYM_REF3
#undef SYM_REF4
#undef SYM_REF5
#undef SYM_REF6
#undef DATATYPE_VISIT

 private:
  const matiec::AnalysisStore &analysis_;
};

}  // namespace

bool publish_datatype_candidates(symbol_c *tree_root,
                                 matiec::AnalysisStore &analysis) {
  publish_candidates_c publisher(analysis);
  tree_root->accept(publisher);
  return publisher.succeeded();
}

void materialize_datatype_candidates(
    symbol_c *tree_root, const matiec::AnalysisStore &analysis) {
  materialize_candidates_c materializer(analysis);
  tree_root->accept(materializer);
}

#include "constant_analysis_store.hh"

#include "../absyntax/visitor.hh"
#include "../compiler/analysis_store.hh"

namespace {

class publish_constants_c : public iterator_visitor_c {
 public:
  explicit publish_constants_c(matiec::AnalysisStore &analysis)
      : analysis_(analysis) {}

  void *publish(symbol_c *symbol) {
    matiec::ConstantAnalysisRecord record;
    record.value = symbol->const_value;
    if (!analysis_.set_constant(symbol, record)) succeeded_ = false;
    return nullptr;
  }

#define CONSTANT_VISIT(class_name)                                             \
  void *visit(class_name *symbol) override {                                  \
    publish(symbol);                                                          \
    return iterator_visitor_c::visit(symbol);                                 \
  }
#define SYM_LIST(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_TOKEN(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF0(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF1(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF2(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF3(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF4(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF5(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF6(class_name, ...) CONSTANT_VISIT(class_name)
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
#undef CONSTANT_VISIT

  bool succeeded() const { return succeeded_; }

 private:
  matiec::AnalysisStore &analysis_;
  bool succeeded_ = true;
};

class materialize_constants_c : public iterator_visitor_c {
 public:
  explicit materialize_constants_c(const matiec::AnalysisStore &analysis)
      : analysis_(analysis) {}

  void *materialize(symbol_c *symbol) {
    const matiec::AnalysisEntry<matiec::ConstantAnalysisRecord> *entry =
        analysis_.constant(symbol);
    if (entry != nullptr) symbol->const_value = entry->value.value;
    return nullptr;
  }

#define CONSTANT_VISIT(class_name)                                             \
  void *visit(class_name *symbol) override {                                  \
    materialize(symbol);                                                      \
    return iterator_visitor_c::visit(symbol);                                 \
  }
#define SYM_LIST(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_TOKEN(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF0(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF1(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF2(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF3(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF4(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF5(class_name, ...) CONSTANT_VISIT(class_name)
#define SYM_REF6(class_name, ...) CONSTANT_VISIT(class_name)
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
#undef CONSTANT_VISIT

 private:
  const matiec::AnalysisStore &analysis_;
};

}  // namespace

bool publish_constant_analysis(symbol_c *tree_root,
                               matiec::AnalysisStore &analysis) {
  publish_constants_c publisher(analysis);
  tree_root->accept(publisher);
  return publisher.succeeded();
}

void materialize_constant_compatibility(
    symbol_c *tree_root, const matiec::AnalysisStore &analysis) {
  materialize_constants_c materializer(analysis);
  tree_root->accept(materializer);
}

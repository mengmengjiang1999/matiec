#include "resolution_analysis_store.hh"

#include "../absyntax/visitor.hh"
#include "../compiler/analysis_store.hh"

namespace {

class publish_resolution_c : public iterator_visitor_c {
 public:
  explicit publish_resolution_c(matiec::AnalysisStore &analysis)
      : analysis_(analysis) {}

  void publish_function(symbol_c *key, const std::vector<symbol_c *> &candidates,
                        symbol_c *declaration, int extensible_parameter_count) {
    matiec::ResolutionAnalysisRecord record;
    record.candidates = candidates;
    record.declaration = declaration;
    record.extensible_parameter_count = extensible_parameter_count;
    if (!analysis_.set_resolution(key, record)) succeeded_ = false;
  }

  void publish_fb(symbol_c *key, symbol_c *declaration) {
    publish_function(key, std::vector<symbol_c *>(), declaration, 0);
  }

#define FUNCTION_RESOLUTION_VISIT(class_name)                                 \
  void *visit(class_name *symbol) override {                                  \
    publish_function(symbol, symbol->candidate_functions,                     \
                     symbol->called_function_declaration,                     \
                     symbol->extensible_param_count);                         \
    return iterator_visitor_c::visit(symbol);                                 \
  }
  FUNCTION_RESOLUTION_VISIT(il_function_call_c)
  FUNCTION_RESOLUTION_VISIT(il_formal_funct_call_c)
  FUNCTION_RESOLUTION_VISIT(function_invocation_c)
#undef FUNCTION_RESOLUTION_VISIT

#define FB_RESOLUTION_VISIT(class_name)                                       \
  void *visit(class_name *symbol) override {                                  \
    publish_fb(symbol, symbol->called_fb_declaration);                        \
    return iterator_visitor_c::visit(symbol);                                 \
  }
  FB_RESOLUTION_VISIT(il_fb_call_c)
  FB_RESOLUTION_VISIT(fb_invocation_c)
  FB_RESOLUTION_VISIT(S_operator_c)
  FB_RESOLUTION_VISIT(R_operator_c)
  FB_RESOLUTION_VISIT(S1_operator_c)
  FB_RESOLUTION_VISIT(R1_operator_c)
  FB_RESOLUTION_VISIT(CLK_operator_c)
  FB_RESOLUTION_VISIT(CU_operator_c)
  FB_RESOLUTION_VISIT(CD_operator_c)
  FB_RESOLUTION_VISIT(PV_operator_c)
  FB_RESOLUTION_VISIT(IN_operator_c)
  FB_RESOLUTION_VISIT(PT_operator_c)
#undef FB_RESOLUTION_VISIT

  void *visit(object_method_invocation_c *symbol) override {
    iterator_visitor_c::visit(symbol);
    if (symbol->compatibility_invocation != nullptr)
      symbol->compatibility_invocation->accept(*this);
    return nullptr;
  }

  bool succeeded() const { return succeeded_; }

 private:
  matiec::AnalysisStore &analysis_;
  bool succeeded_ = true;
};

}  // namespace

bool publish_declaration_resolution(symbol_c *tree_root,
                                    matiec::AnalysisStore &analysis) {
  publish_resolution_c publisher(analysis);
  tree_root->accept(publisher);
  return publisher.succeeded();
}

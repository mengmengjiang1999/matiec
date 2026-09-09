#ifndef MATIEC_COMPILER_OBJECT_METHOD_BINDING_HH
#define MATIEC_COMPILER_OBJECT_METHOD_BINDING_HH

class symbol_c;

namespace matiec {

class DiagnosticEngine;
struct ObjectMethodAnalysisResult;

bool bind_object_method_semantics(
    symbol_c *tree_root, const ObjectMethodAnalysisResult &model,
    DiagnosticEngine &diagnostics);

}  // namespace matiec

#endif

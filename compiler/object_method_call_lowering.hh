#ifndef MATIEC_COMPILER_OBJECT_METHOD_CALL_LOWERING_HH
#define MATIEC_COMPILER_OBJECT_METHOD_CALL_LOWERING_HH

class symbol_c;

namespace matiec {

class DiagnosticEngine;
struct ObjectMethodAnalysisResult;

bool lower_object_method_calls(
    symbol_c *tree_root, const ObjectMethodAnalysisResult &model,
    DiagnosticEngine &diagnostics);

}  // namespace matiec

#endif

#ifndef MATIEC_COMPILER_OBJECT_METHOD_COMPATIBILITY_AST_HH
#define MATIEC_COMPILER_OBJECT_METHOD_COMPATIBILITY_AST_HH

class symbol_c;

namespace matiec {

class DiagnosticEngine;
struct ObjectMethodNormalizeResult;

bool construct_object_method_compatibility_ast(
    symbol_c *tree_root, const ObjectMethodNormalizeResult &model,
    DiagnosticEngine &diagnostics);

}  // namespace matiec

#endif

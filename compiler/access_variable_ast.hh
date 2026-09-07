#ifndef MATIEC_COMPILER_ACCESS_VARIABLE_AST_HH
#define MATIEC_COMPILER_ACCESS_VARIABLE_AST_HH

class symbol_c;

namespace matiec {

class DiagnosticEngine;
struct AccessVariableNormalizeResult;

bool analyze_access_variables_from_ast(
    symbol_c *tree_root, DiagnosticEngine &diagnostics,
    AccessVariableNormalizeResult *result);

}  // namespace matiec

#endif

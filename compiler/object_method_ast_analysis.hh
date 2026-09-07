#ifndef MATIEC_COMPILER_OBJECT_METHOD_AST_ANALYSIS_HH
#define MATIEC_COMPILER_OBJECT_METHOD_AST_ANALYSIS_HH

#include "compiler/experimental_syntax_model.hh"

#include <map>
#include <string>
#include <vector>

class symbol_c;

namespace matiec {

class DiagnosticEngine;

struct ObjectMethodAnalysisResult {
  std::vector<ObjectMethodAst> methods;
  std::map<std::string, std::string> instance_types;
  bool used_methods = false;
};

bool analyze_object_methods_from_ast(
    symbol_c *tree_root, DiagnosticEngine &diagnostics,
    ObjectMethodAnalysisResult *result);

}  // namespace matiec

#endif

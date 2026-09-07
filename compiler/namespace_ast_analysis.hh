#ifndef MATIEC_COMPILER_NAMESPACE_AST_ANALYSIS_HH
#define MATIEC_COMPILER_NAMESPACE_AST_ANALYSIS_HH

#include "compiler/experimental_syntax_model.hh"

#include <vector>

class symbol_c;

namespace matiec {

class DiagnosticEngine;

struct NamespaceAnalysisResult {
  std::vector<NamespaceDeclarationAst> declarations;
  bool used_namespaces = false;
};

bool analyze_namespaces_from_ast(symbol_c *tree_root,
                                 DiagnosticEngine &diagnostics,
                                 NamespaceAnalysisResult *result);

}  // namespace matiec

#endif

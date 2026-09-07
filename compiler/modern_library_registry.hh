#ifndef MATIEC_COMPILER_MODERN_LIBRARY_REGISTRY_HH
#define MATIEC_COMPILER_MODERN_LIBRARY_REGISTRY_HH

#include "compiler/experimental_syntax_model.hh"

#include <vector>

class symbol_c;

namespace matiec {

class DiagnosticEngine;

struct ModernLibraryRegistrationResult {
  std::vector<ModernLibraryFunctionAst> functions;
  bool used_modern_library = false;
};

bool register_experimental_modern_library_from_ast(
    symbol_c *tree_root, bool disable_implicit_en_eno,
    DiagnosticEngine &diagnostics, ModernLibraryRegistrationResult *result);

}  // namespace matiec

#endif

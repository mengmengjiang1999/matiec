#ifndef MATIEC_COMPILER_MODERN_LIBRARY_NORMALIZER_HH
#define MATIEC_COMPILER_MODERN_LIBRARY_NORMALIZER_HH

#include "compiler/compiler_types.hh"

#include <string>
#include <string_view>
#include <vector>

namespace matiec {

class DiagnosticEngine;

struct ModernLibraryFunctionAst {
  std::string name;
  std::string source_signature;
  std::string lowered_name;
  SourceRange range;
};

struct ModernLibraryNormalizeResult {
  std::string source;
  std::vector<ModernLibraryFunctionAst> functions;
  bool used_modern_library = false;
};

bool normalize_experimental_modern_library(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, ModernLibraryNormalizeResult *result);

}  // namespace matiec

#endif

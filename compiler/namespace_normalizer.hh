#ifndef MATIEC_COMPILER_NAMESPACE_NORMALIZER_HH
#define MATIEC_COMPILER_NAMESPACE_NORMALIZER_HH

#include "compiler/experimental_syntax_model.hh"

#include <string>
#include <string_view>
#include <vector>

namespace matiec {

class DiagnosticEngine;

struct NamespaceNormalizeResult {
  std::string source;
  bool used_namespaces = false;
};

bool normalize_experimental_namespaces(std::string_view source,
                                       const std::string &source_path,
                                       DiagnosticEngine &diagnostics,
                                       NamespaceNormalizeResult *result);

bool normalize_experimental_namespace_file(
    const std::string &source_path, DiagnosticEngine &diagnostics,
    NamespaceNormalizeResult *result);

}  // namespace matiec

#endif

#ifndef MATIEC_COMPILER_NAMESPACE_NORMALIZER_HH
#define MATIEC_COMPILER_NAMESPACE_NORMALIZER_HH

#include "compiler/compiler_types.hh"

#include <string>
#include <string_view>
#include <vector>

namespace matiec {

class DiagnosticEngine;

enum class NamespaceVisibility { public_, internal };

struct NamespaceImportAst {
  std::string namespace_name;
  SourceRange range;
};

struct NamespaceDeclarationAst {
  std::string name;
  NamespaceVisibility visibility = NamespaceVisibility::public_;
  SourceRange range;
  std::vector<NamespaceImportAst> imports;
};

struct NamespaceNormalizeResult {
  std::string source;
  std::vector<NamespaceDeclarationAst> declarations;
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

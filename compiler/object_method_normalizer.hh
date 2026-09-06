#ifndef MATIEC_COMPILER_OBJECT_METHOD_NORMALIZER_HH
#define MATIEC_COMPILER_OBJECT_METHOD_NORMALIZER_HH

#include "compiler/compiler_types.hh"

#include <string>
#include <string_view>
#include <vector>

namespace matiec {

class DiagnosticEngine;

struct ObjectMethodAst {
  std::string owner;
  std::string name;
  std::string return_type;
  std::string lowered_name;
  SourceRange range;
};

struct ObjectMethodNormalizeResult {
  std::string source;
  std::vector<ObjectMethodAst> methods;
  bool used_methods = false;
};

bool normalize_experimental_object_methods(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, ObjectMethodNormalizeResult *result);

}  // namespace matiec

#endif

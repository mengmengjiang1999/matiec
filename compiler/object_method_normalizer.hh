#ifndef MATIEC_COMPILER_OBJECT_METHOD_NORMALIZER_HH
#define MATIEC_COMPILER_OBJECT_METHOD_NORMALIZER_HH

#include "compiler/experimental_syntax_model.hh"

#include <string>
#include <string_view>
#include <vector>

namespace matiec {

class DiagnosticEngine;

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

#ifndef MATIEC_COMPILER_ACCESS_VARIABLE_NORMALIZER_HH
#define MATIEC_COMPILER_ACCESS_VARIABLE_NORMALIZER_HH

#include "compiler/compiler_types.hh"

#include <string>
#include <string_view>
#include <vector>

namespace matiec {

class DiagnosticEngine;
class OutputManager;

struct AccessVariableAst {
  std::string configuration;
  std::string name;
  std::string path;
  std::string type;
  std::string direction;
  SourceRange range;
};

struct AccessVariableNormalizeResult {
  std::string source;
  std::vector<AccessVariableAst> declarations;
  bool used_access_variables = false;
};

bool normalize_experimental_access_variables(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, AccessVariableNormalizeResult *result);

bool write_access_variable_metadata(
    const AccessVariableNormalizeResult &result,
    const std::string &output_directory, OutputManager &outputs);

}  // namespace matiec

#endif

#ifndef MATIEC_COMPILER_ACCESS_VARIABLE_NORMALIZER_HH
#define MATIEC_COMPILER_ACCESS_VARIABLE_NORMALIZER_HH

#include "compiler/experimental_syntax_model.hh"

#include <string>
#include <string_view>
#include <vector>

namespace matiec {

class DiagnosticEngine;
class OutputManager;

struct AccessVariableNormalizeResult {
  std::string source;
  std::vector<AccessVariableAst> declarations;
  bool used_access_variables = false;
};

bool normalize_experimental_access_variables(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, AccessVariableNormalizeResult *result);

bool reject_legacy_access_variables_in_file(
    const std::string &source_path, DiagnosticEngine &diagnostics);
bool reject_legacy_access_variables(std::string_view source,
                                    const std::string &source_path,
                                    DiagnosticEngine &diagnostics);

bool write_access_variable_metadata(
    const AccessVariableNormalizeResult &result,
    const std::string &output_directory, OutputManager &outputs);

}  // namespace matiec

#endif

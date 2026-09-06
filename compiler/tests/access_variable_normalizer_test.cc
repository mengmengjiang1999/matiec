#include "compiler/access_variable_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <cassert>
#include <string>

namespace {

const char *configuration_with_access(const char *declaration,
                                      bool constant = false) {
  static std::string source;
  source =
      "CONFIGURATION Demo\nVAR_GLOBAL";
  if (constant) source += " CONSTANT";
  source +=
      "\nShared : INT;\nEND_VAR\n"
      "RESOURCE R ON PLC\nPROGRAM P : Main;\nEND_RESOURCE\n"
      "VAR_ACCESS\n";
  source += declaration;
  source += "\nEND_VAR\nEND_CONFIGURATION\n";
  return source.c_str();
}

}  // namespace

int main() {
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::AccessVariableNormalizeResult result;
    assert(matiec::normalize_experimental_access_variables(
        configuration_with_access("Remote : Shared : INT READ_WRITE;"),
        "access.st", diagnostics, &result));
    assert(result.used_access_variables);
    assert(result.declarations.size() == 1);
    assert(result.declarations[0].configuration == "Demo");
    assert(result.declarations[0].direction == "READ_WRITE");
    assert(result.source.find("VAR_ACCESS") == std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::AccessVariableNormalizeResult result;
    assert(matiec::normalize_experimental_access_variables(
        configuration_with_access("Remote : Shared : INT;"), "access.st",
        diagnostics, &result));
    assert(result.declarations[0].direction == "READ_ONLY");
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::AccessVariableNormalizeResult result;
    assert(!matiec::normalize_experimental_access_variables(
        configuration_with_access("Remote : Missing : INT;"), "access.st",
        diagnostics, &result));
    assert(diagnostics.diagnostics().front().message.find("Unresolved") !=
           std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::AccessVariableNormalizeResult result;
    assert(!matiec::normalize_experimental_access_variables(
        configuration_with_access("Remote : Shared : BOOL;"), "access.st",
        diagnostics, &result));
    assert(diagnostics.diagnostics().front().message.find("type does not match") !=
           std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::AccessVariableNormalizeResult result;
    assert(!matiec::normalize_experimental_access_variables(
        configuration_with_access("Remote : Shared : INT READ_WRITE;", true),
        "access.st", diagnostics, &result));
    assert(diagnostics.diagnostics().front().message.find("CONSTANT") !=
           std::string::npos);
  }
}

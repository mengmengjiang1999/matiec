#include "compiler/access_variable_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <cassert>
#include <string>

int main() {
  {
    matiec::DiagnosticEngine diagnostics;
    assert(!matiec::reject_legacy_access_variables(
        "CONFIGURATION Demo\nVAR_ACCESS\nEND_VAR\nEND_CONFIGURATION\n",
        "access.st", diagnostics));
    assert(diagnostics.diagnostics().size() == 1);
    assert(diagnostics.diagnostics().front().message.find(
               "2025-experimental") != std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    assert(!matiec::reject_legacy_access_variables(
        "\tvar_access (* mixed case *)\n", "mixed.st", diagnostics));
    assert(diagnostics.diagnostics().size() == 1);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    assert(matiec::reject_legacy_access_variables(
        "VAR_ACCESSIBLE : BOOL;\nXVAR_ACCESS : BOOL;\n", "near.st",
        diagnostics));
    assert(!diagnostics.has_errors());
  }
  {
    matiec::DiagnosticEngine diagnostics;
    assert(matiec::reject_legacy_access_variables(
        "PROGRAM Main\nVAR value : INT; END_VAR\nEND_PROGRAM\n",
        "main.st", diagnostics));
    assert(!diagnostics.has_errors());
  }
  return 0;
}

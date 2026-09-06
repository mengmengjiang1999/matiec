#include "compiler/namespace_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <cassert>
#include <string>

namespace {

bool normalize(const std::string &source, matiec::DiagnosticEngine *diagnostics,
               matiec::NamespaceNormalizeResult *result) {
  return matiec::normalize_experimental_namespaces(source, "namespace.st",
                                                    *diagnostics, result);
}

}  // namespace

int main() {
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::NamespaceNormalizeResult result;
    const std::string source =
        "NAMESPACE Factory.Motion\n"
        "TYPE Speed : INT; END_TYPE\n"
        "FUNCTION Read : Speed\nVAR_INPUT X : Speed; END_VAR\nRead := X;\nEND_FUNCTION\n"
        "END_NAMESPACE\n"
        "PROGRAM Main\nVAR Value : Factory.Motion.Speed; END_VAR\n"
        "Value := Factory.Motion.Read(Value);\nEND_PROGRAM\n";
    assert(normalize(source, &diagnostics, &result));
    assert(result.used_namespaces);
    assert(result.declarations.size() == 1);
    assert(result.source.find("NAMESPACE") == std::string::npos);
    assert(result.source.find("MATIECNS7FACTORY6MOTION5SPEED") !=
           std::string::npos);
    assert(result.source.find("MATIECNS7FACTORY6MOTION4READ") !=
           std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::NamespaceNormalizeResult result;
    const std::string source =
        "NAMESPACE A\nTYPE Value : INT; END_TYPE\nEND_NAMESPACE\n"
        "USING A;\nPROGRAM Main\nVAR X : Value; END_VAR\nEND_PROGRAM\n";
    assert(normalize(source, &diagnostics, &result));
    assert(result.source.find("MATIECNS1A5VALUE") != std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::NamespaceNormalizeResult result;
    const std::string source =
        "NAMESPACE A\nTYPE Value : INT; END_TYPE\nEND_NAMESPACE\n"
        "NAMESPACE B\nTYPE Value : INT; END_TYPE\nEND_NAMESPACE\n"
        "USING A;\nUSING B;\nPROGRAM Main\nVAR X : Value; END_VAR\nEND_PROGRAM\n";
    assert(!normalize(source, &diagnostics, &result));
    assert(diagnostics.diagnostics().front().message.find("Ambiguous") !=
           std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::NamespaceNormalizeResult result;
    const std::string source =
        "NAMESPACE INTERNAL Secret\nTYPE Value : INT; END_TYPE\nEND_NAMESPACE\n"
        "PROGRAM Main\nVAR X : Secret.Value; END_VAR\nEND_PROGRAM\n";
    assert(!normalize(source, &diagnostics, &result));
    assert(diagnostics.diagnostics().front().message.find("inaccessible") !=
           std::string::npos);
  }
  {
    matiec::DiagnosticEngine diagnostics;
    matiec::NamespaceNormalizeResult result;
    const std::string source =
        "PROGRAM Main\nVAR X : INT; END_VAR\nEND_PROGRAM\n";
    assert(normalize(source, &diagnostics, &result));
    assert(!result.used_namespaces);
    assert(result.source == source);
  }
}

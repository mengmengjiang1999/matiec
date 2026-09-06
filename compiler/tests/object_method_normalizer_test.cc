#include "compiler/object_method_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <cassert>
#include <string>

int main() {
  {
    const std::string source =
        "FUNCTION_BLOCK Counter\n"
        "VAR\nCount : INT;\nEND_VAR\n"
        "Count := Count;\n"
        "METHOD PUBLIC Increment : INT\n"
        "VAR_INPUT\nDelta : INT;\nEND_VAR\n"
        "Count := Count + Delta;\nIncrement := Count;\n"
        "END_METHOD\nEND_FUNCTION_BLOCK\n"
        "PROGRAM Main\nVAR\nC : Counter;\nValue : INT;\nEND_VAR\n"
        "Value := C.Increment(2);\nEND_PROGRAM\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ObjectMethodNormalizeResult result;
    assert(matiec::normalize_experimental_object_methods(
        source, "method.st", diagnostics, &result));
    assert(result.used_methods);
    assert(result.methods.size() == 1);
    assert(result.source.find("METHOD PUBLIC") == std::string::npos);
    assert(result.source.find("FUNCTION MATIECMETHOD7COUNTER9INCREMENT : INT") !=
           std::string::npos);
    assert(result.source.find("MATIECSELF.COUNT := MATIECSELF.COUNT + Delta") !=
           std::string::npos);
    assert(result.source.find("MATIECMETHOD7COUNTER9INCREMENT(C, 2)") !=
           std::string::npos);
  }
  {
    const std::string source =
        "FUNCTION_BLOCK Counter\nVAR\nCount : INT;\nEND_VAR\nCount := Count;\n"
        "METHOD PRIVATE Hidden : INT\nHidden := Count;\nEND_METHOD\n"
        "END_FUNCTION_BLOCK\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ObjectMethodNormalizeResult result;
    assert(!matiec::normalize_experimental_object_methods(
        source, "private.st", diagnostics, &result));
    assert(diagnostics.diagnostics().front().message.find("Only PUBLIC") !=
           std::string::npos);
  }
}

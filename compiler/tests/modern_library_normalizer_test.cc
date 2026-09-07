#include "compiler/modern_library_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <cassert>
#include <string>

int main() {
  {
    const std::string source =
        "PROGRAM Demo\nVAR Value : INT; END_VAR\n"
        "ASSERT(Value = 0);\nEND_PROGRAM\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryNormalizeResult result;
    assert(matiec::normalize_experimental_modern_library(
        source, "assert.st", diagnostics, &result));
    assert(result.used_modern_library);
    assert(result.functions.size() == 1);
    assert(result.functions[0].source_signature ==
           "ASSERT(IN : BOOL) : VOID");
    assert(result.source.find("FUNCTION ASSERT : VOID") !=
           std::string::npos);
    assert(result.source.find("ASSERT(Value = 0);") !=
           std::string::npos);
  }
  {
    const std::string source =
        "PROGRAM Demo\n(*\nASSERT(FALSE);\n*)\nEND_PROGRAM\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryNormalizeResult result;
    assert(matiec::normalize_experimental_modern_library(
        source, "comment.st", diagnostics, &result));
    assert(!result.used_modern_library);
    assert(result.source == source);
  }
  {
    const std::string source =
        "PROGRAM Demo\nVAR Value : BOOL; END_VAR\n"
        "ASSERT(Value) AND Value;\nEND_PROGRAM\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryNormalizeResult result;
    assert(matiec::normalize_experimental_modern_library(
        source, "invalid.st", diagnostics, &result));
    assert(result.used_modern_library);
    assert(!diagnostics.has_errors());
  }
  {
    const std::string source =
        "PROGRAM Demo\nVAR Value : BOOL; END_VAR\n"
        "ASSERT (* reason *) (\nValue\n);\nEND_PROGRAM\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryNormalizeResult result;
    assert(matiec::normalize_experimental_modern_library(
        source, "multiline.st", diagnostics, &result));
    assert(result.used_modern_library);
    assert(result.source.find("ASSERT (* reason *) (\nValue\n);") !=
           std::string::npos);
  }
  {
    const std::string source =
        "PROGRAM Demo\nVAR Text : STRING; END_VAR\n"
        "Text := 'ASSERT(FALSE)';\nEND_PROGRAM\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryNormalizeResult result;
    assert(matiec::normalize_experimental_modern_library(
        source, "string.st", diagnostics, &result));
    assert(!result.used_modern_library);
    assert(result.source == source);
  }
  {
    const std::string source =
        "FUNCTION ASSERT : BOOL\nVAR_INPUT IN : BOOL; END_VAR\n"
        "ASSERT := IN;\nEND_FUNCTION\n"
        "PROGRAM Demo\nVAR Value : BOOL; END_VAR\n"
        "Value := ASSERT(TRUE);\nEND_PROGRAM\n";
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryNormalizeResult result;
    assert(matiec::normalize_experimental_modern_library(
        source, "custom.st", diagnostics, &result));
    assert(!result.used_modern_library);
    assert(result.source == source);
  }
}

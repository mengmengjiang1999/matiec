#include "compiler/compiler_types.hh"

#include <cassert>
#include <string>

int main() {
  matiec::CompilerOptions options;
  assert(options.language_profile == matiec::LanguageProfile::legacy);
  assert(!options.pre_parsing);
  assert(!options.syntax_only);
  assert(options.output_language == matiec::OutputLanguage::c);

  matiec::LanguageProfile profile = matiec::LanguageProfile::legacy;
  assert(matiec::parse_language_profile("legacy", &profile));
  assert(profile == matiec::LanguageProfile::legacy);
  assert(!matiec::language_profile_is_experimental(profile));
  assert(matiec::parse_language_profile(
      "iec61131-3:2025-experimental", &profile));
  assert(profile == matiec::LanguageProfile::iec61131_3_2025_experimental);
  assert(matiec::language_profile_is_experimental(profile));
  assert(std::string(matiec::language_profile_name(profile)) ==
         "iec61131-3:2025-experimental");
  assert(!matiec::parse_language_profile("iec61131-3:2025", &profile));
  assert(!matiec::parse_language_profile("legacy", nullptr));

  const matiec::SourceLocation begin{"program.st", 4, 2, 18};
  const matiec::SourceLocation end{"program.st", 4, 7, 23};
  const matiec::SourceRange range{begin, end};
  assert(begin.valid());
  assert(range.valid());

  const matiec::Diagnostic diagnostic{
      matiec::DiagnosticSeverity::warning, "example", range};
  assert(std::string(matiec::diagnostic_severity_name(diagnostic.severity)) == "warning");

  const matiec::CompilationResult success = matiec::CompilationResult::success(2);
  assert(success.succeeded());
  assert(success.warning_count == 2);

  const matiec::CompilationResult failure = matiec::CompilationResult::failure(0);
  assert(!failure.succeeded());
  assert(failure.error_count == 1);
  return 0;
}

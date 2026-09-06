#include "compiler/compiler_types.hh"

namespace matiec {

const char *language_profile_name(LanguageProfile profile) {
  switch (profile) {
    case LanguageProfile::legacy:
      return "legacy";
    case LanguageProfile::iec61131_3_2025_experimental:
      return "iec61131-3:2025-experimental";
  }
  return "unknown";
}

bool parse_language_profile(const std::string &name, LanguageProfile *profile) {
  if (profile == nullptr) return false;
  if (name == "legacy") {
    *profile = LanguageProfile::legacy;
    return true;
  }
  if (name == "iec61131-3:2025-experimental") {
    *profile = LanguageProfile::iec61131_3_2025_experimental;
    return true;
  }
  return false;
}

bool language_profile_is_experimental(LanguageProfile profile) {
  return profile == LanguageProfile::iec61131_3_2025_experimental;
}

bool SourceLocation::valid() const {
  return !file.empty() && line > 0 && column > 0;
}

bool SourceRange::valid() const {
  return begin.valid() && end.valid() && begin.file == end.file;
}

bool CompilationResult::succeeded() const {
  return error_count == 0;
}

CompilationResult CompilationResult::success(std::size_t warnings) {
  return {0, warnings};
}

CompilationResult CompilationResult::failure(std::size_t errors, std::size_t warnings) {
  return {errors == 0 ? 1 : errors, warnings};
}

const char *diagnostic_severity_name(DiagnosticSeverity severity) {
  switch (severity) {
    case DiagnosticSeverity::note: return "note";
    case DiagnosticSeverity::warning: return "warning";
    case DiagnosticSeverity::error: return "error";
    case DiagnosticSeverity::fatal: return "fatal";
  }
  return "unknown";
}

}  // namespace matiec

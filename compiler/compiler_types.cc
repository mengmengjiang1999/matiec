#include "compiler/compiler_types.hh"

namespace matiec {

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

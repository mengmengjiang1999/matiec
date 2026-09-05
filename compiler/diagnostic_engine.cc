#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <ostream>
#include <utility>

namespace matiec {

void DiagnosticEngine::report(DiagnosticSeverity severity, std::string message,
                              SourceRange range) {
  diagnostics_.push_back({severity, std::move(message), std::move(range)});
}

void DiagnosticEngine::note(std::string message, SourceRange range) {
  report(DiagnosticSeverity::note, std::move(message), std::move(range));
}

void DiagnosticEngine::warning(std::string message, SourceRange range) {
  report(DiagnosticSeverity::warning, std::move(message), std::move(range));
}

void DiagnosticEngine::error(std::string message, SourceRange range) {
  report(DiagnosticSeverity::error, std::move(message), std::move(range));
}

void DiagnosticEngine::fatal(std::string message, SourceRange range) {
  report(DiagnosticSeverity::fatal, std::move(message), std::move(range));
}

const std::vector<Diagnostic> &DiagnosticEngine::diagnostics() const {
  return diagnostics_;
}

std::size_t DiagnosticEngine::error_count() const {
  return std::count_if(diagnostics_.begin(), diagnostics_.end(),
                       [](const Diagnostic &diagnostic) {
                         return diagnostic.severity == DiagnosticSeverity::error ||
                                diagnostic.severity == DiagnosticSeverity::fatal;
                       });
}

std::size_t DiagnosticEngine::warning_count() const {
  return std::count_if(diagnostics_.begin(), diagnostics_.end(),
                       [](const Diagnostic &diagnostic) {
                         return diagnostic.severity == DiagnosticSeverity::warning;
                       });
}

bool DiagnosticEngine::has_errors() const {
  return error_count() != 0;
}

CompilationResult DiagnosticEngine::result() const {
  return {error_count(), warning_count()};
}

void DiagnosticEngine::render(std::ostream &output) const {
  for (const Diagnostic &diagnostic : diagnostics_) {
    if (diagnostic.range.valid()) {
      const SourceLocation &begin = diagnostic.range.begin;
      const SourceLocation &end = diagnostic.range.end;
      output << begin.file << ':' << begin.line << '-' << begin.column
             << ".." << end.line << '-' << end.column << ": ";
    }
    output << diagnostic_severity_name(diagnostic.severity) << ": "
           << diagnostic.message << '\n';
  }
}

void DiagnosticEngine::clear() {
  diagnostics_.clear();
}

}  // namespace matiec

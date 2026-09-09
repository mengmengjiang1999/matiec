#include "diagnostic_engine.hh"

#include <algorithm>
#include <ostream>
#include <utility>

namespace matiec {
namespace {
std::string generic_code(DiagnosticSeverity severity, DiagnosticPhase phase) {
  const char severity_letter[] = {'N', 'W', 'E', 'F'};
  const unsigned phase_number = static_cast<unsigned>(phase);
  return std::string("MATIEC-") + severity_letter[static_cast<unsigned>(severity)] +
         std::to_string(phase_number > 4 ? 0 : phase_number) + "000";
}
}  // namespace

void DiagnosticEngine::report(DiagnosticSeverity severity, std::string message,
                              SourceRange range, std::string code) {
  if (limit_ != 0 && diagnostics_.size() >= limit_) {
    limit_exceeded_ = true;
    diagnostics_.back() = {DiagnosticSeverity::fatal,
                           "Diagnostic limit exceeded", {},
                           "MATIEC-F0003", phase_};
    return;
  }
  if (code.empty()) code = generic_code(severity, phase_);
  diagnostics_.push_back(
      {severity, std::move(message), std::move(range), std::move(code), phase_});
}

void DiagnosticEngine::note(std::string message, SourceRange range,
                            std::string code) {
  report(DiagnosticSeverity::note, std::move(message), std::move(range),
         std::move(code));
}

void DiagnosticEngine::warning(std::string message, SourceRange range,
                               std::string code) {
  report(DiagnosticSeverity::warning, std::move(message), std::move(range),
         std::move(code));
}

void DiagnosticEngine::error(std::string message, SourceRange range,
                             std::string code) {
  report(DiagnosticSeverity::error, std::move(message), std::move(range),
         std::move(code));
}

void DiagnosticEngine::fatal(std::string message, SourceRange range,
                             std::string code) {
  report(DiagnosticSeverity::fatal, std::move(message), std::move(range),
         std::move(code));
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
  limit_exceeded_ = false;
}

void DiagnosticEngine::set_limit(std::size_t maximum) { limit_ = maximum; }

bool DiagnosticEngine::limit_exceeded() const { return limit_exceeded_; }

void DiagnosticEngine::set_phase(DiagnosticPhase phase) { phase_ = phase; }

DiagnosticPhase DiagnosticEngine::phase() const { return phase_; }

}  // namespace matiec

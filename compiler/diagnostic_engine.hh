#ifndef MATIEC_COMPILER_DIAGNOSTIC_ENGINE_HH
#define MATIEC_COMPILER_DIAGNOSTIC_ENGINE_HH

#include "compiler_types.hh"

#include <iosfwd>
#include <string>
#include <vector>

namespace matiec {

class DiagnosticEngine {
 public:
  void report(DiagnosticSeverity severity, std::string message,
              SourceRange range = {}, std::string code = {});
  void note(std::string message, SourceRange range = {}, std::string code = {});
  void warning(std::string message, SourceRange range = {},
               std::string code = {});
  void error(std::string message, SourceRange range = {}, std::string code = {});
  void fatal(std::string message, SourceRange range = {}, std::string code = {});

  const std::vector<Diagnostic> &diagnostics() const;
  std::size_t error_count() const;
  std::size_t warning_count() const;
  bool has_errors() const;
  CompilationResult result() const;

  void render(std::ostream &output) const;
  void clear();
  void set_limit(std::size_t maximum);
  bool limit_exceeded() const;
  void set_phase(DiagnosticPhase phase);
  DiagnosticPhase phase() const;

 private:
  std::vector<Diagnostic> diagnostics_;
  std::size_t limit_ = 0;
  bool limit_exceeded_ = false;
  DiagnosticPhase phase_ = DiagnosticPhase::unknown;
};

}  // namespace matiec

#endif

#ifndef MATIEC_COMPILER_DIAGNOSTIC_ENGINE_HH
#define MATIEC_COMPILER_DIAGNOSTIC_ENGINE_HH

#include "compiler/compiler_types.hh"

#include <iosfwd>
#include <string>
#include <vector>

namespace matiec {

class DiagnosticEngine {
 public:
  void report(DiagnosticSeverity severity, std::string message,
              SourceRange range = {});
  void note(std::string message, SourceRange range = {});
  void warning(std::string message, SourceRange range = {});
  void error(std::string message, SourceRange range = {});
  void fatal(std::string message, SourceRange range = {});

  const std::vector<Diagnostic> &diagnostics() const;
  std::size_t error_count() const;
  std::size_t warning_count() const;
  bool has_errors() const;
  CompilationResult result() const;

  void render(std::ostream &output) const;
  void clear();

 private:
  std::vector<Diagnostic> diagnostics_;
};

}  // namespace matiec

#endif

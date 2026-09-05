#ifndef MATIEC_STAGE3_SEMANTIC_DIAGNOSTICS_HH
#define MATIEC_STAGE3_SEMANTIC_DIAGNOSTICS_HH

#include "../compiler/diagnostic_engine.hh"

#include <cstdarg>

class symbol_c;

namespace matiec {

class SemanticDiagnostics {
 public:
  explicit SemanticDiagnostics(DiagnosticEngine &diagnostics);

  void error(symbol_c *first, symbol_c *last, const char *format, ...);
  void warning(symbol_c *first, symbol_c *last, const char *format, ...);

 private:
  void report(DiagnosticSeverity severity, symbol_c *first, symbol_c *last,
              const char *format, va_list arguments);

  DiagnosticEngine &diagnostics_;
};

}  // namespace matiec

#endif

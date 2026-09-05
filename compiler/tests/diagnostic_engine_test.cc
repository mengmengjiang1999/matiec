#include "compiler/diagnostic_engine.hh"

#include <cassert>
#include <sstream>
#include <string>

int main() {
  matiec::DiagnosticEngine diagnostics;
  const matiec::SourceRange range{
      {"program.st", 8, 3, 40}, {"program.st", 8, 9, 46}};

  diagnostics.note("checking declaration");
  diagnostics.warning("deprecated form", range);
  diagnostics.error("invalid declaration", range);
  diagnostics.fatal("generation stopped");

  assert(diagnostics.diagnostics().size() == 4);
  assert(diagnostics.warning_count() == 1);
  assert(diagnostics.error_count() == 2);
  assert(diagnostics.has_errors());
  assert(!diagnostics.result().succeeded());

  std::ostringstream rendered;
  diagnostics.render(rendered);
  assert(rendered.str() ==
         "note: checking declaration\n"
         "program.st:8-3..8-9: warning: deprecated form\n"
         "program.st:8-3..8-9: error: invalid declaration\n"
         "fatal: generation stopped\n");

  diagnostics.clear();
  assert(diagnostics.diagnostics().empty());
  assert(diagnostics.result().succeeded());
  return 0;
}

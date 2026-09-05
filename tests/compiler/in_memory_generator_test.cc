#include "compiler/ast_arena.hh"
#include "compiler/diagnostic_engine.hh"
#include "compiler/output_manager.hh"
#include "compiler/output_sink.hh"
#include "stage4/generate_c/generate_c_internal.hh"
#include "stage4/stage4.hh"

#include <cassert>

runtime_options_t runtime_options = {};

int main() {
  matiec::DiagnosticEngine diagnostics;
  matiec::MemoryOutputSink memory;
  matiec::OutputManager outputs(diagnostics, memory);
  matiec::AstArena arena;
  matiec::ActiveAstArenaScope arena_scope(arena);

  integer_c literal("42");
  stage4out_c output(outputs);
  visitor_c *generator =
      new_generate_c_st_generator(&output, NULL, &literal, NULL);

  literal.accept(*generator);
  delete generator;
  output.flush();

  assert(!diagnostics.has_errors());
  assert(memory.contents() == "42");
  return 0;
}

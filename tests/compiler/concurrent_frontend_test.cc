#include "compiler/compilation_context.hh"
#include "compiler/compiler.hh"
#include "compiler/parser_symbol_tables.hh"

#include <cassert>
#include <cstdlib>
#include <future>
#include <string>
#include <thread>

namespace {

void configure(matiec::CompilationContext &context, const char *library,
               const std::string &name) {
  context.options().include_directory = library;
  context.options().syntax_only = true;
  context.set_source(
      "memory://" + name + ".st",
      "PROGRAM " + name + "\nVAR value : INT; END_VAR\n"
      "value := 1;\nEND_PROGRAM\n");
}

bool contains(matiec::CompilationContext &context, const char *name) {
  symtable_c<int> &symbols =
      context.parser_state().symbols().library_elements;
  return symbols.find(name) != symbols.end();
}

}  // namespace

int main() {
  const char *library = std::getenv("MATIEC_LIBDIR");
  assert(library != nullptr);

  for (int iteration = 0; iteration < 4; ++iteration) {
    matiec::CompilationContext alpha;
    matiec::CompilationContext beta;
    configure(alpha, library, "ParallelAlpha");
    configure(beta, library, "ParallelBeta");

    std::promise<void> release;
    std::shared_future<void> start(release.get_future());
    matiec::CompilationResult alpha_result;
    matiec::CompilationResult beta_result;

    std::thread alpha_thread([&] {
      start.wait();
      alpha_result = matiec::Compiler().compile(alpha);
    });
    std::thread beta_thread([&] {
      start.wait();
      beta_result = matiec::Compiler().compile(beta);
    });
    release.set_value();
    alpha_thread.join();
    beta_thread.join();

    assert(alpha_result.succeeded());
    assert(beta_result.succeeded());
    assert(alpha.ast_arena().allocation_count() > 0);
    assert(beta.ast_arena().allocation_count() > 0);
    assert(contains(alpha, "ParallelAlpha"));
    assert(!contains(alpha, "ParallelBeta"));
    assert(contains(beta, "ParallelBeta"));
    assert(!contains(beta, "ParallelAlpha"));
  }

  return 0;
}

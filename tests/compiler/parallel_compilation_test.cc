#include "compiler/compilation_context.hh"
#include "compiler/compiler.hh"
#include "compiler/parser_symbol_tables.hh"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

namespace {

class TemporaryDirectory {
 public:
  explicit TemporaryDirectory(const char *label) {
    std::string pattern = std::string("/tmp/matiec-") + label + "-XXXXXX";
    std::vector<char> writable(pattern.begin(), pattern.end());
    writable.push_back('\0');
    char *created = mkdtemp(writable.data());
    assert(created != nullptr);
    path_ = created;
  }

  ~TemporaryDirectory() { std::filesystem::remove_all(path_); }

  const std::string &path() const { return path_; }

 private:
  std::string path_;
};

void configure(matiec::CompilationContext &context, const char *library,
               const std::string &name, const std::string &source,
               const TemporaryDirectory &output) {
  context.options().include_directory = library;
  context.options().output_directory = output.path();
  context.set_source("memory://" + name + ".st", source);
}

std::string read_file(const std::string &path) {
  std::ifstream input(path);
  assert(input.good());
  std::ostringstream contents;
  contents << input.rdbuf();
  return contents.str();
}

bool contains(matiec::CompilationContext &context, const char *name) {
  symtable_c<int> &symbols = context.parser_state().symbols().library_elements;
  return symbols.find(name) != symbols.end();
}

}  // namespace

int main() {
  const char *library = std::getenv("MATIEC_LIBDIR");
  assert(library != nullptr);

  TemporaryDirectory alpha_output("parallel-alpha");
  TemporaryDirectory broken_output("parallel-broken");
  TemporaryDirectory gamma_output("parallel-gamma");
  matiec::CompilationContext alpha;
  matiec::CompilationContext broken;
  matiec::CompilationContext gamma;

  configure(alpha, library, "ParallelAlpha",
            "PROGRAM ParallelAlpha\nVAR Value : INT; END_VAR\n"
            "Value := 1;\nEND_PROGRAM\n",
            alpha_output);
  configure(broken, library, "ParallelBroken",
            "PROGRAM ParallelBroken\nVAR Value : MissingType; END_VAR\n"
            "END_PROGRAM\n",
            broken_output);
  configure(gamma, library, "ParallelGamma",
            "PROGRAM ParallelGamma\nVAR Flag : BOOL; END_VAR\n"
            "Flag := TRUE;\nEND_PROGRAM\n",
            gamma_output);

  matiec::Compiler compiler;
  const std::vector<std::reference_wrapper<matiec::CompilationContext>> jobs = {
      std::ref(alpha), std::ref(broken), std::ref(gamma)};
  const std::vector<matiec::CompilationResult> results =
      compiler.compile_parallel(jobs, 2);

  assert(results.size() == 3);
  assert(results[0].succeeded());
  assert(!results[1].succeeded());
  assert(results[2].succeeded());
  assert(!alpha.diagnostics().has_errors());
  assert(!gamma.diagnostics().has_errors());
  assert(alpha.ast_arena().allocation_count() > 0);
  assert(broken.ast_arena().allocation_count() > 0);
  assert(gamma.ast_arena().allocation_count() > 0);
  assert(contains(alpha, "ParallelAlpha"));
  assert(!contains(alpha, "ParallelGamma"));
  assert(contains(gamma, "ParallelGamma"));
  assert(!contains(gamma, "ParallelAlpha"));

  const std::string alpha_c = read_file(alpha_output.path() + "/POUS.c");
  const std::string gamma_c = read_file(gamma_output.path() + "/POUS.c");
  assert(alpha_c.find("PARALLELALPHA") != std::string::npos);
  assert(alpha_c.find("PARALLELGAMMA") == std::string::npos);
  assert(gamma_c.find("PARALLELGAMMA") != std::string::npos);
  assert(gamma_c.find("PARALLELALPHA") == std::string::npos);
  assert(!std::filesystem::exists(broken_output.path() + "/POUS.c"));

  matiec::CompilationContext duplicate;
  duplicate.options().include_directory = library;
  duplicate.options().syntax_only = true;
  duplicate.set_source("memory://duplicate.st",
                       "PROGRAM Duplicate\nEND_PROGRAM\n");
  const std::vector<std::reference_wrapper<matiec::CompilationContext>>
      duplicate_jobs = {std::ref(duplicate), std::ref(duplicate)};
  const std::vector<matiec::CompilationResult> duplicate_results =
      compiler.compile_parallel(duplicate_jobs);
  assert(duplicate_results.size() == 2);
  assert(!duplicate_results[0].succeeded());
  assert(!duplicate_results[1].succeeded());
  assert(duplicate_results[0].error_count == 1);
  assert(duplicate_results[1].error_count == 1);
  assert(duplicate.diagnostics().has_errors());
  assert(duplicate.ast_arena().allocation_count() == 0);

  matiec::CompilationContext automatic;
  automatic.options().include_directory = library;
  automatic.options().syntax_only = true;
  automatic.set_source("memory://automatic.st",
                       "PROGRAM Automatic\nVAR Value : INT; END_VAR\n"
                       "Value := 1;\nEND_PROGRAM\n");
  const std::vector<std::reference_wrapper<matiec::CompilationContext>>
      automatic_jobs = {std::ref(automatic)};
  const std::vector<matiec::CompilationResult> automatic_results =
      compiler.compile_parallel(automatic_jobs);
  assert(automatic_results.size() == 1);
  assert(automatic_results[0].succeeded());

  assert(compiler.compile_parallel({}).empty());
  return 0;
}

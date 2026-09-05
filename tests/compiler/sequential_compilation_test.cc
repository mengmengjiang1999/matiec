#include "compiler/compilation_context.hh"
#include "compiler/compiler.hh"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <unistd.h>

namespace {

std::string write_invalid_source() {
  char path[] = "/tmp/matiec-invalid-XXXXXX";
  const int descriptor = mkstemp(path);
  assert(descriptor >= 0);

  FILE *file = fdopen(descriptor, "w");
  assert(file != nullptr);
  const char source[] = "PROGRAM Broken\nVAR\nvalue : INT;\n";
  assert(std::fwrite(source, 1, sizeof(source) - 1, file) == sizeof(source) - 1);
  assert(std::fclose(file) == 0);
  return path;
}

matiec::CompilationResult compile_syntax(const std::string &source,
                                         const std::string &library,
                                         bool pre_parsing,
                                         bool full_token_location) {
  matiec::CompilationContext context;
  context.set_source_path(source);
  context.options().include_directory = library;
  context.options().syntax_only = true;
  context.options().pre_parsing = pre_parsing;
  context.options().full_token_location = full_token_location;

  const matiec::CompilationResult result = matiec::Compiler().compile(context);
  assert(context.options().pre_parsing == pre_parsing);
  assert(context.options().full_token_location == full_token_location);
  assert(context.ast_arena().allocation_count() > 0);
  return result;
}

}  // namespace

int main() {
  const char *valid_source = std::getenv("SEQUENTIAL_VALID_INPUT");
  const char *library = std::getenv("MATIEC_LIBDIR");
  assert(valid_source != nullptr);
  assert(library != nullptr);

  const std::string invalid_source = write_invalid_source();
  const matiec::CompilationResult invalid =
      compile_syntax(invalid_source, library, false, false);
  assert(!invalid.succeeded());
  assert(std::remove(invalid_source.c_str()) == 0);

  const matiec::CompilationResult valid =
      compile_syntax(valid_source, library, false, false);
  assert(valid.succeeded());

  const matiec::CompilationResult valid_with_different_options =
      compile_syntax(valid_source, library, true, true);
  assert(valid_with_different_options.succeeded());
  return 0;
}

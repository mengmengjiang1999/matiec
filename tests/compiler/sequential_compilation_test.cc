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

matiec::CompilationResult compile_memory(const std::string &source,
                                         const std::string &library,
                                         bool pre_parsing,
                                         matiec::LanguageProfile profile) {
  matiec::CompilationContext context;
  context.set_source("memory://main.st", source);
  context.options().include_directory = library;
  context.options().syntax_only = true;
  context.options().pre_parsing = pre_parsing;
  context.options().language_profile = profile;
  const matiec::CompilationResult result = matiec::Compiler().compile(context);
  assert(context.source_path() == "memory://main.st");
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

  const std::string memory_source =
      "PROGRAM MemoryMain\nVAR value : INT; END_VAR\nvalue := 42;\n"
      "END_PROGRAM\n";
  assert(compile_memory(memory_source, library, false,
                        matiec::LanguageProfile::legacy).succeeded());
  assert(compile_memory(memory_source, library, true,
                        matiec::LanguageProfile::legacy).succeeded());

  const std::string experimental_source =
      "PROGRAM MemoryExperimental\nVAR value : BOOL; END_VAR\n"
      "ASSERT(TRUE);\nEND_PROGRAM\n";
  assert(compile_memory(
             experimental_source, library, false,
             matiec::LanguageProfile::iec61131_3_2025_experimental)
             .succeeded());
  assert(!compile_memory("PROGRAM Broken\n", library, false,
                         matiec::LanguageProfile::legacy).succeeded());
  const std::string legacy_access =
      "CONFIGURATION BadAccess\nVAR_ACCESS\n"
      "Alias : Target : INT;\nEND_VAR\nEND_CONFIGURATION\n";
  assert(!compile_memory(legacy_access, library, false,
                         matiec::LanguageProfile::legacy).succeeded());

  matiec::CompilationContext reusable;
  reusable.options().include_directory = library;
  reusable.options().syntax_only = true;
  reusable.options().language_profile =
      matiec::LanguageProfile::iec61131_3_2025_experimental;
  reusable.set_source("memory://experimental.st", experimental_source);
  assert(matiec::Compiler().compile(reusable).succeeded());
  assert(reusable.experimental_syntax().library_functions.size() == 1);
  assert(!reusable.options().allow_void_datatype);
  const std::string namespace_source =
      "NAMESPACE Factory.Motion\n"
      "TYPE Speed : INT; END_TYPE\nEND_NAMESPACE\n"
      "USING Factory.Motion;\n"
      "PROGRAM NamespaceMain\nVAR Value : Factory.Motion.Speed; END_VAR\n"
      "Value := Value;\nEND_PROGRAM\n";
  reusable.set_source("memory://namespace.st", namespace_source);
  assert(matiec::Compiler().compile(reusable).succeeded());
  assert(reusable.experimental_syntax().namespaces.size() == 1);
  assert(reusable.experimental_syntax().namespaces[0].name == "Factory.Motion");
  assert(reusable.experimental_syntax().namespaces[0].visibility ==
         matiec::NamespaceVisibility::public_);
  const std::string method_source =
      "FUNCTION_BLOCK Counter\nVAR Count : INT; END_VAR\n"
      "Count := Count;\nMETHOD PUBLIC Read : INT\n"
      "Read := Count;\nEND_METHOD\nEND_FUNCTION_BLOCK\n"
      "PROGRAM MethodMain\nVAR C : Counter; Value : INT; END_VAR\n"
      "Value := C.Read();\nEND_PROGRAM\n";
  reusable.set_source("memory://method.st", method_source);
  assert(matiec::Compiler().compile(reusable).succeeded());
  assert(reusable.experimental_syntax().methods.size() == 1);
  assert(reusable.experimental_syntax().methods[0].owner == "Counter");
  assert(reusable.experimental_syntax().methods[0].owner_fields.size() == 1);
  assert(reusable.experimental_syntax().methods[0].owner_fields[0].first ==
         "COUNT");
  const std::string access_source =
      "TYPE UserCount : INT; END_TYPE\n"
      "PROGRAM AccessMain\nVAR value : INT; END_VAR\n"
      "value := value;\nEND_PROGRAM\n"
      "CONFIGURATION AccessConfig\n"
      "VAR_GLOBAL Counter : UserCount; END_VAR\n"
      "RESOURCE R ON PLC PROGRAM P : AccessMain; END_RESOURCE\n"
      "VAR_ACCESS Monitor : Counter : UserCount; END_VAR\n"
      "END_CONFIGURATION\n";
  reusable.set_source("memory://access.st", access_source);
  assert(matiec::Compiler().compile(reusable).succeeded());
  assert(reusable.experimental_syntax().access_variables.size() == 1);
  assert(reusable.experimental_syntax().access_variables[0].type == "UserCount");
  reusable.options().language_profile = matiec::LanguageProfile::legacy;
  reusable.set_source("memory://legacy.st", memory_source);
  assert(matiec::Compiler().compile(reusable).succeeded());
  assert(reusable.experimental_syntax().library_functions.empty());
  assert(!reusable.options().allow_void_datatype);
  reusable.set_source("memory://legacy-void.st",
                      "FUNCTION LegacyVoid : VOID\nEND_FUNCTION\n");
  assert(!matiec::Compiler().compile(reusable).succeeded());
  assert(!reusable.options().allow_void_datatype);
  return 0;
}

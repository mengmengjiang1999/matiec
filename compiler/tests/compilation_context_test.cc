#include "compiler/compilation_context.hh"

#include <cassert>

int main() {
  matiec::CompilationContext first;
  matiec::CompilationContext second;

  first.options().pre_parsing = true;
  first.options().include_directory = "first-lib";
  first.set_source_path("first.st");
  first.diagnostics().error("first failure");

  second.options().relaxed_datatype_model = true;
  second.options().include_directory = "second-lib";
  second.set_source_path("second.st");

  assert(first.options().pre_parsing);
  assert(!second.options().pre_parsing);
  assert(!first.options().relaxed_datatype_model);
  assert(second.options().relaxed_datatype_model);
  assert(first.options().include_directory == "first-lib");
  assert(second.options().include_directory == "second-lib");
  assert(first.source_path() == "first.st");
  assert(second.source_path() == "second.st");
  assert(first.diagnostics().has_errors());
  assert(!second.diagnostics().has_errors());
  return 0;
}

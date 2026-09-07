#include "compiler/modern_library_registry.hh"

#include "absyntax/absyntax.hh"
#include "compiler/ast_arena.hh"
#include "compiler/diagnostic_engine.hh"

#include <cassert>

namespace {

function_invocation_c *assert_call() {
  return new function_invocation_c(new poutype_identifier_c("ASSERT"), nullptr,
                                   nullptr);
}

function_declaration_c *user_assert() {
  return new function_declaration_c(
      new identifier_c("ASSERT"), new bool_type_name_c,
      new var_declarations_list_c, new statement_list_c);
}

}  // namespace

int main() {
  matiec::AstArena arena;
  matiec::ActiveAstArenaScope scope(arena);

  {
    library_c *library = new library_c;
    library->add_element(assert_call());
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryRegistrationResult result;
    assert(matiec::register_experimental_modern_library_from_ast(
        library, false, diagnostics, &result));
    assert(!diagnostics.has_errors());
    assert(result.used_modern_library);
    assert(result.functions.size() == 1);
    assert(result.functions[0].source_signature ==
           "ASSERT(IN : BOOL) : VOID");
    assert(library->n == 2);
    function_declaration_c *declaration =
        dynamic_cast<function_declaration_c *>(library->get_element(1));
    assert(declaration != nullptr);
    assert(declaration->profile_library_declaration);
    assert(dynamic_cast<void_type_name_c *>(declaration->type_name) != nullptr);
  }

  {
    library_c *library = new library_c;
    library->add_element(user_assert());
    library->add_element(assert_call());
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryRegistrationResult result;
    assert(matiec::register_experimental_modern_library_from_ast(
        library, false, diagnostics, &result));
    assert(!result.used_modern_library);
    assert(result.functions.empty());
    assert(library->n == 2);
  }

  {
    library_c *library = new library_c;
    matiec::DiagnosticEngine diagnostics;
    matiec::ModernLibraryRegistrationResult result;
    assert(matiec::register_experimental_modern_library_from_ast(
        library, true, diagnostics, &result));
    assert(!result.used_modern_library);
    assert(library->n == 0);
  }
}

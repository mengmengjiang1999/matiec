#include "compiler/modern_library_registry.hh"

#include "absyntax/absyntax.hh"
#include "absyntax/visitor.hh"
#include "absyntax_utils/add_en_eno_param_decl.hh"
#include "compiler/ast_arena.hh"
#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <string>

namespace matiec {
namespace {

std::string uppercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::toupper(ch));
                 });
  return value;
}

bool named_assert(symbol_c *symbol) {
  token_c *token = dynamic_cast<token_c *>(symbol);
  return token != nullptr && uppercase(token->value) == "ASSERT";
}

SourceRange source_range(symbol_c *symbol) {
  if (symbol == nullptr) return {};
  return {{symbol->first_file == nullptr ? "" : symbol->first_file,
           static_cast<std::size_t>(symbol->first_line),
           static_cast<std::size_t>(symbol->first_column), 0},
          {symbol->last_file == nullptr ? "" : symbol->last_file,
           static_cast<std::size_t>(symbol->last_line),
           static_cast<std::size_t>(symbol->last_column), 0}};
}

template <typename Symbol>
Symbol *located_token(const char *value, symbol_c *location) {
  return new Symbol(retain_ast_string(value), location->first_line,
                    location->first_column, location->first_file,
                    location->first_order, location->last_line,
                    location->last_column, location->last_file,
                    location->last_order);
}

class modern_library_usage_c : public iterator_visitor_c {
 public:
  void *visit(function_declaration_c *symbol) override {
    if (named_assert(symbol->derived_function_name)) user_declares_assert = true;
    return iterator_visitor_c::visit(symbol);
  }

  void *visit(function_invocation_c *symbol) override {
    if (first_assert_call == nullptr && named_assert(symbol->function_name))
      first_assert_call = symbol;
    return iterator_visitor_c::visit(symbol);
  }

  bool user_declares_assert = false;
  function_invocation_c *first_assert_call = nullptr;
};

function_declaration_c *make_assert_declaration(symbol_c *location,
                                                bool disable_implicit_en_eno) {
  var1_list_c *names = new var1_list_c;
  names->add_element(located_token<identifier_c>("IN", location));
  simple_spec_init_c *bool_spec = new simple_spec_init_c(
      new bool_type_name_c(location->first_line, location->first_column,
                           location->first_file, location->first_order,
                           location->last_line, location->last_column,
                           location->last_file, location->last_order),
      nullptr);
  input_declaration_list_c *inputs = new input_declaration_list_c;
  inputs->add_element(new var1_init_decl_c(names, bool_spec));
  var_declarations_list_c *declarations = new var_declarations_list_c;
  declarations->add_element(new input_declarations_c(nullptr, inputs, nullptr));

  statement_list_c *body = new statement_list_c;
  body->add_element(new return_statement_c(
      location->first_line, location->first_column, location->first_file,
      location->first_order, location->last_line, location->last_column,
      location->last_file, location->last_order));
  function_declaration_c *function = new function_declaration_c(
      located_token<identifier_c>("ASSERT", location),
      new void_type_name_c(location->first_line, location->first_column,
                           location->first_file, location->first_order,
                           location->last_line, location->last_column,
                           location->last_file, location->last_order),
      declarations, body, location->first_line, location->first_column,
      location->first_file, location->first_order, location->last_line,
      location->last_column, location->last_file, location->last_order);
  function->profile_library_declaration = true;
  if (!disable_implicit_en_eno) add_en_eno_param_decl_c::add_to(function);
  return function;
}

}  // namespace

bool register_experimental_modern_library_from_ast(
    symbol_c *tree_root, bool disable_implicit_en_eno,
    DiagnosticEngine &diagnostics, ModernLibraryRegistrationResult *result) {
  if (result == nullptr) return false;
  result->functions.clear();
  result->used_modern_library = false;

  library_c *library = dynamic_cast<library_c *>(tree_root);
  if (library == nullptr) {
    diagnostics.error("Modern library registration requires a library AST", {});
    return false;
  }

  modern_library_usage_c usage;
  library->accept(usage);
  if (usage.user_declares_assert || usage.first_assert_call == nullptr)
    return true;

  result->used_modern_library = true;
  result->functions.push_back(
      {"ASSERT", "ASSERT(IN : BOOL) : VOID", "ASSERT",
       source_range(usage.first_assert_call)});
  library->add_element(make_assert_declaration(
      usage.first_assert_call, disable_implicit_en_eno));
  return true;
}

}  // namespace matiec

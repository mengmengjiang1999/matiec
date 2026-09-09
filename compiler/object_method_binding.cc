#include "compiler/object_method_binding.hh"

#include "absyntax/absyntax.hh"
#include "absyntax/visitor.hh"
#include "absyntax_utils/add_en_eno_param_decl.hh"
#include "compiler/ast_arena.hh"
#include "compiler/diagnostic_engine.hh"
#include "compiler/object_method_ast_analysis.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace matiec {
namespace {

std::string uppercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
  return value;
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
Symbol *located_token(const std::string &value, symbol_c *location) {
  return new Symbol(retain_ast_string(value.c_str()), location->first_line,
                    location->first_column, location->first_file,
                    location->first_order, location->last_line,
                    location->last_column, location->last_file,
                    location->last_order);
}

#define BUILTIN_TYPE_CASE(spelling, class_name) \
  if (key == spelling) return new class_name( \
      location->first_line, location->first_column, location->first_file, \
      location->first_order, location->last_line, location->last_column, \
      location->last_file, location->last_order)

symbol_c *type_name(const std::string &name, symbol_c *location) {
  const std::string key = uppercase(name);
  BUILTIN_TYPE_CASE("TIME", time_type_name_c);
  BUILTIN_TYPE_CASE("BOOL", bool_type_name_c);
  BUILTIN_TYPE_CASE("SINT", sint_type_name_c);
  BUILTIN_TYPE_CASE("INT", int_type_name_c);
  BUILTIN_TYPE_CASE("DINT", dint_type_name_c);
  BUILTIN_TYPE_CASE("LINT", lint_type_name_c);
  BUILTIN_TYPE_CASE("USINT", usint_type_name_c);
  BUILTIN_TYPE_CASE("UINT", uint_type_name_c);
  BUILTIN_TYPE_CASE("UDINT", udint_type_name_c);
  BUILTIN_TYPE_CASE("ULINT", ulint_type_name_c);
  BUILTIN_TYPE_CASE("REAL", real_type_name_c);
  BUILTIN_TYPE_CASE("LREAL", lreal_type_name_c);
  BUILTIN_TYPE_CASE("DATE", date_type_name_c);
  BUILTIN_TYPE_CASE("TOD", tod_type_name_c);
  BUILTIN_TYPE_CASE("DT", dt_type_name_c);
  BUILTIN_TYPE_CASE("BYTE", byte_type_name_c);
  BUILTIN_TYPE_CASE("WORD", word_type_name_c);
  BUILTIN_TYPE_CASE("DWORD", dword_type_name_c);
  BUILTIN_TYPE_CASE("LWORD", lword_type_name_c);
  BUILTIN_TYPE_CASE("STRING", string_type_name_c);
  BUILTIN_TYPE_CASE("WSTRING", wstring_type_name_c);
  return located_token<derived_datatype_identifier_c>(name, location);
}

symbol_c *type_specification(const std::string &name, symbol_c *location) {
  return new simple_spec_init_c(type_name(name, location), nullptr,
      location->first_line, location->first_column, location->first_file,
      location->first_order, location->last_line, location->last_column,
      location->last_file, location->last_order);
}

#undef BUILTIN_TYPE_CASE

class identifier_alias_c : public iterator_visitor_c {
 public:
  explicit identifier_alias_c(
      const std::map<std::string, std::string> &aliases)
      : aliases_(aliases) {}

  void *visit(identifier_c *symbol) override {
    const auto found = aliases_.find(uppercase(symbol->value));
    if (found != aliases_.end()) {
      if (symbol->source_value == nullptr) symbol->source_value = symbol->value;
      symbol->value = retain_ast_string(found->second.c_str());
    }
    return nullptr;
  }

 private:
  const std::map<std::string, std::string> &aliases_;
};

struct NativeMethod {
  function_block_declaration_c *owner = nullptr;
  object_method_declaration_c *method = nullptr;
};

class native_method_collector_c : public iterator_visitor_c {
 public:
  void *visit(function_block_declaration_c *symbol) override {
    object_method_declaration_list_c *methods =
        dynamic_cast<object_method_declaration_list_c *>(symbol->methods);
    if (methods != nullptr) {
      for (int index = 0; index < methods->n; ++index) {
        object_method_declaration_c *method =
            dynamic_cast<object_method_declaration_c *>(methods->get_element(index));
        if (method != nullptr) methods_.push_back({symbol, method});
      }
    }
    return iterator_visitor_c::visit(symbol);
  }

  std::vector<NativeMethod> methods_;
};

const ObjectMethodAst *find_model_method(
    const ObjectMethodAnalysisResult &model,
    function_block_declaration_c *owner,
    object_method_declaration_c *method) {
  token_c *owner_name = dynamic_cast<token_c *>(owner->fblock_name);
  object_method_header_c *header =
      dynamic_cast<object_method_header_c *>(method->header);
  token_c *method_name = header == nullptr
      ? nullptr : dynamic_cast<token_c *>(header->method_name);
  if (owner_name == nullptr || method_name == nullptr) return nullptr;
  for (const ObjectMethodAst &candidate : model.methods) {
    if (uppercase(candidate.owner) == uppercase(owner_name->value) &&
        uppercase(candidate.name) == uppercase(method_name->value))
      return &candidate;
  }
  return nullptr;
}

function_declaration_c *make_semantic_declaration(
    const NativeMethod &native, const ObjectMethodAst &model) {
  var_declarations_list_c *native_declarations =
      dynamic_cast<var_declarations_list_c *>(native.method->var_declarations);
  if (native_declarations == nullptr) return nullptr;

  var_declarations_list_c *declarations = new var_declarations_list_c(
      native.method->first_line, native.method->first_column,
      native.method->first_file, native.method->first_order,
      native.method->last_line, native.method->last_column,
      native.method->last_file, native.method->last_order);
  for (int index = 0; index < native_declarations->n; ++index)
    declarations->add_element(native_declarations->get_element(index));

  if (!model.owner_fields.empty()) {
    var_declaration_list_c *fields = new var_declaration_list_c;
    for (const auto &field : model.owner_fields) {
      var1_list_c *names = new var1_list_c;
      names->add_element(located_token<identifier_c>(
          "MATIECSELF" + field.first, native.method));
      fields->add_element(new var1_init_decl_c(
          names, type_specification(field.second, native.method)));
    }
    declarations->add_element(new input_output_declarations_c(fields));
  }

  std::map<std::string, std::string> aliases;
  aliases[uppercase(model.name)] = model.lowered_name;
  for (const auto &field : model.owner_fields)
    aliases[uppercase(field.first)] = "MATIECSELF" + field.first;
  identifier_alias_c aliaser(aliases);
  native.method->method_body->accept(aliaser);

  symbol_c *type_parent = native.method->type_name->parent;
  symbol_c *body_parent = native.method->method_body->parent;
  function_declaration_c *function = new function_declaration_c(
      located_token<identifier_c>(model.lowered_name, native.method),
      native.method->type_name, declarations, native.method->method_body,
      native.method->first_line, native.method->first_column,
      native.method->first_file, native.method->first_order,
      native.method->last_line, native.method->last_column,
      native.method->last_file, native.method->last_order);
  native.method->type_name->parent = type_parent;
  native.method->method_body->parent = body_parent;
  add_en_eno_param_decl_c::add_to(function);
  return function;
}

}  // namespace

bool bind_object_method_semantics(
    symbol_c *tree_root, const ObjectMethodAnalysisResult &model,
    DiagnosticEngine &diagnostics) {
  library_c *library = dynamic_cast<library_c *>(tree_root);
  if (library == nullptr) return false;

  native_method_collector_c collector;
  library->accept(collector);
  for (const NativeMethod &native : collector.methods_) {
    const ObjectMethodAst *method = find_model_method(
        model, native.owner, native.method);
    if (method == nullptr) {
      diagnostics.error("Missing semantic metadata for parsed method",
                        source_range(native.method));
      continue;
    }
    function_declaration_c *function =
        make_semantic_declaration(native, *method);
    if (function == nullptr) {
      diagnostics.error("Unsupported native method declaration",
                        source_range(native.method));
      continue;
    }
    native.method->semantic_declaration = function;
  }
  return !diagnostics.has_errors();
}

}  // namespace matiec

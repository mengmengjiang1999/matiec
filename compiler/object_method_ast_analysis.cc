#include "compiler/object_method_ast_analysis.hh"

#include "absyntax/absyntax.hh"
#include "absyntax/visitor.hh"
#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>

namespace matiec {
namespace {

std::string uppercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
  return value;
}

std::string token_value(symbol_c *symbol) {
  token_c *token = dynamic_cast<token_c *>(symbol);
  return token == nullptr || token->value == nullptr ? "" : token->value;
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

#define TYPE_SPELLING(class_name, spelling) \
  if (dynamic_cast<class_name *>(symbol) != nullptr) return spelling

std::string type_spelling(symbol_c *symbol) {
  if (symbol == nullptr) return "";
  if (simple_spec_init_c *node = dynamic_cast<simple_spec_init_c *>(symbol))
    return type_spelling(node->simple_specification);
  if (subrange_spec_init_c *node = dynamic_cast<subrange_spec_init_c *>(symbol))
    return type_spelling(node->subrange_specification);
  if (subrange_specification_c *node =
          dynamic_cast<subrange_specification_c *>(symbol))
    return type_spelling(node->integer_type_name);
  if (initialized_structure_c *node =
          dynamic_cast<initialized_structure_c *>(symbol))
    return type_spelling(node->structure_type_name);
  if (fb_spec_init_c *node = dynamic_cast<fb_spec_init_c *>(symbol))
    return type_spelling(node->function_block_type_name);
  const std::string value = token_value(symbol);
  if (!value.empty()) return value;
  TYPE_SPELLING(time_type_name_c, "TIME");
  TYPE_SPELLING(bool_type_name_c, "BOOL");
  TYPE_SPELLING(sint_type_name_c, "SINT");
  TYPE_SPELLING(int_type_name_c, "INT");
  TYPE_SPELLING(dint_type_name_c, "DINT");
  TYPE_SPELLING(lint_type_name_c, "LINT");
  TYPE_SPELLING(usint_type_name_c, "USINT");
  TYPE_SPELLING(uint_type_name_c, "UINT");
  TYPE_SPELLING(udint_type_name_c, "UDINT");
  TYPE_SPELLING(ulint_type_name_c, "ULINT");
  TYPE_SPELLING(real_type_name_c, "REAL");
  TYPE_SPELLING(lreal_type_name_c, "LREAL");
  TYPE_SPELLING(date_type_name_c, "DATE");
  TYPE_SPELLING(tod_type_name_c, "TOD");
  TYPE_SPELLING(dt_type_name_c, "DT");
  TYPE_SPELLING(byte_type_name_c, "BYTE");
  TYPE_SPELLING(word_type_name_c, "WORD");
  TYPE_SPELLING(dword_type_name_c, "DWORD");
  TYPE_SPELLING(lword_type_name_c, "LWORD");
  TYPE_SPELLING(string_type_name_c, "STRING");
  TYPE_SPELLING(wstring_type_name_c, "WSTRING");
  return "";
}

#undef TYPE_SPELLING

std::string lowered_method_name(const std::string &owner,
                                const std::string &method) {
  const std::string owner_key = uppercase(owner);
  const std::string method_key = uppercase(method);
  return "MATIECMETHOD" + std::to_string(owner_key.size()) + owner_key +
         std::to_string(method_key.size()) + method_key;
}

void add_names(symbol_c *names, const std::string &type,
               std::map<std::string, std::string> *declarations) {
  list_c *list = dynamic_cast<list_c *>(names);
  if (list == nullptr) return;
  for (int index = 0; index < list->n; ++index) {
    const std::string name = token_value(list->get_element(index));
    if (!name.empty()) (*declarations)[uppercase(name)] = type;
  }
}

class declaration_collector_c : public iterator_visitor_c {
 public:
  void *visit(var1_init_decl_c *symbol) override {
    add_names(symbol->var1_list, type_spelling(symbol->spec_init), &variables);
    return nullptr;
  }

  void *visit(fb_name_decl_c *symbol) override {
    fb_spec_init_c *spec = dynamic_cast<fb_spec_init_c *>(symbol->fb_spec_init);
    const std::string type = spec == nullptr
        ? "" : type_spelling(spec->function_block_type_name);
    add_names(symbol->fb_name_list, type, &variables);
    return nullptr;
  }

  std::map<std::string, std::string> variables;
};

class receiver_collector_c : public iterator_visitor_c {
 public:
  void *visit(fb_name_decl_c *symbol) override {
    fb_spec_init_c *spec = dynamic_cast<fb_spec_init_c *>(symbol->fb_spec_init);
    const std::string type = spec == nullptr
        ? "" : type_spelling(spec->function_block_type_name);
    add_names(symbol->fb_name_list, uppercase(type), &instance_types);
    return nullptr;
  }

  std::map<std::string, std::string> instance_types;
};

void analyze_owner(function_block_declaration_c *owner,
                   DiagnosticEngine &diagnostics,
                   ObjectMethodAnalysisResult *result,
                   std::set<std::string> *method_keys) {
  object_method_declaration_list_c *methods =
      dynamic_cast<object_method_declaration_list_c *>(owner->methods);
  if (methods == nullptr || methods->n == 0) return;
  result->used_methods = true;
  const std::string owner_name = token_value(owner->fblock_name);

  declaration_collector_c owner_declarations;
  if (owner->var_declarations != nullptr)
    owner->var_declarations->accept(owner_declarations);

  for (int index = 0; index < methods->n; ++index) {
    object_method_declaration_c *method =
        dynamic_cast<object_method_declaration_c *>(methods->get_element(index));
    object_method_header_c *header = method == nullptr
        ? nullptr : dynamic_cast<object_method_header_c *>(method->header);
    if (header == nullptr) continue;
    const std::string method_name = token_value(header->method_name);
    const SourceRange range = source_range(method);
    if (dynamic_cast<object_method_public_c *>(header->visibility) == nullptr) {
      diagnostics.error(
          "Only PUBLIC methods are supported by the MATIEC experimental method subset",
          range);
      continue;
    }
    const std::string key = uppercase(owner_name) + "." + uppercase(method_name);
    if (!method_keys->insert(key).second) {
      diagnostics.error("Duplicate method declaration: " + key, range);
      continue;
    }

    declaration_collector_c local_declarations;
    if (method->var_declarations != nullptr)
      method->var_declarations->accept(local_declarations);

    ObjectMethodAst ast;
    ast.owner = owner_name;
    ast.name = method_name;
    ast.return_type = type_spelling(method->type_name);
    ast.lowered_name = lowered_method_name(owner_name, method_name);
    ast.range = range;
    for (const auto &field : owner_declarations.variables) {
      if (local_declarations.variables.count(field.first) == 0) {
        ast.owner_fields.push_back(field);
        const auto receiver = result->instance_types.find(field.first);
        if (receiver != result->instance_types.end())
          result->instance_types["MATIECSELF" + field.first] = receiver->second;
      }
    }
    result->methods.push_back(ast);
  }
}

}  // namespace

bool analyze_object_methods_from_ast(
    symbol_c *tree_root, DiagnosticEngine &diagnostics,
    ObjectMethodAnalysisResult *result) {
  if (tree_root == nullptr || result == nullptr) return false;
  result->methods.clear();
  result->instance_types.clear();
  result->used_methods = false;

  receiver_collector_c receivers;
  tree_root->accept(receivers);
  result->instance_types = receivers.instance_types;

  library_c *library = dynamic_cast<library_c *>(tree_root);
  if (library == nullptr) return false;
  std::set<std::string> method_keys;
  for (int index = 0; index < library->n; ++index) {
    function_block_declaration_c *owner =
        dynamic_cast<function_block_declaration_c *>(library->get_element(index));
    if (owner != nullptr)
      analyze_owner(owner, diagnostics, result, &method_keys);
  }
  return !diagnostics.has_errors();
}

}  // namespace matiec

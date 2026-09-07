#include "compiler/access_variable_ast.hh"

#include "absyntax/absyntax.hh"
#include "compiler/access_variable_normalizer.hh"
#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>

namespace matiec {
namespace {

struct GlobalVariable {
  std::string type;
  bool constant = false;
};

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

std::string token_value(symbol_c *symbol) {
  token_c *token = dynamic_cast<token_c *>(symbol);
  return token == nullptr || token->value == nullptr ? "" : token->value;
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

void add_global_names(symbol_c *specification, const GlobalVariable &variable,
                      std::map<std::string, GlobalVariable> *globals) {
  if (global_var_list_c *list = dynamic_cast<global_var_list_c *>(specification)) {
    for (int index = 0; index < list->n; ++index) {
      const std::string name = token_value(list->get_element(index));
      if (!name.empty()) (*globals)[uppercase(name)] = variable;
    }
    return;
  }
  if (global_var_spec_c *single = dynamic_cast<global_var_spec_c *>(specification)) {
    const std::string name = token_value(single->global_var_name);
    if (!name.empty()) (*globals)[uppercase(name)] = variable;
  }
}

void collect_globals(configuration_declaration_c *configuration,
                     std::map<std::string, GlobalVariable> *globals) {
  global_var_declarations_list_c *blocks =
      dynamic_cast<global_var_declarations_list_c *>(
          configuration->global_var_declarations);
  if (blocks == nullptr) return;
  for (int block_index = 0; block_index < blocks->n; ++block_index) {
    global_var_declarations_c *block =
        dynamic_cast<global_var_declarations_c *>(blocks->get_element(block_index));
    if (block == nullptr) continue;
    global_var_decl_list_c *declarations =
        dynamic_cast<global_var_decl_list_c *>(block->global_var_decl_list);
    if (declarations == nullptr) continue;
    const bool constant = dynamic_cast<constant_option_c *>(block->option) != nullptr;
    for (int index = 0; index < declarations->n; ++index) {
      global_var_decl_c *declaration =
          dynamic_cast<global_var_decl_c *>(declarations->get_element(index));
      if (declaration == nullptr) continue;
      add_global_names(declaration->global_var_spec,
                       {type_spelling(declaration->type_specification), constant},
                       globals);
    }
  }
}

void analyze_configuration(configuration_declaration_c *configuration,
                           DiagnosticEngine &diagnostics,
                           AccessVariableNormalizeResult *result) {
  access_declarations_c *access =
      dynamic_cast<access_declarations_c *>(configuration->access_declarations);
  if (access == nullptr) return;
  result->used_access_variables = true;

  std::map<std::string, GlobalVariable> globals;
  collect_globals(configuration, &globals);
  std::set<std::string> access_names;
  access_declaration_list_c *declarations =
      dynamic_cast<access_declaration_list_c *>(access->access_declaration_list);
  if (declarations == nullptr) return;
  const std::string configuration_name = token_value(configuration->configuration_name);
  for (int index = 0; index < declarations->n; ++index) {
    access_declaration_c *node =
        dynamic_cast<access_declaration_c *>(declarations->get_element(index));
    if (node == nullptr) continue;
    AccessVariableAst declaration;
    declaration.configuration = configuration_name;
    declaration.name = token_value(node->access_name);
    access_path_c *path = dynamic_cast<access_path_c *>(node->access_path);
    declaration.path = path == nullptr ? "" : token_value(path->global_var_name);
    declaration.type = type_spelling(node->type_name);
    declaration.direction =
        dynamic_cast<read_write_c *>(node->direction) == nullptr
            ? "READ_ONLY" : "READ_WRITE";
    declaration.range = source_range(node);

    const auto target = globals.find(uppercase(declaration.path));
    if (!access_names.insert(uppercase(declaration.name)).second) {
      diagnostics.error("Duplicate VAR_ACCESS name: " + declaration.name,
                        declaration.range);
    } else if (target == globals.end()) {
      diagnostics.error("Unresolved VAR_ACCESS target: " + declaration.path,
                        declaration.range);
    } else if (uppercase(declaration.type) != uppercase(target->second.type)) {
      diagnostics.error("VAR_ACCESS type does not match target '" +
                            declaration.path + "'", declaration.range);
    } else if (declaration.direction == "READ_WRITE" && target->second.constant) {
      diagnostics.error(
          "READ_WRITE VAR_ACCESS cannot target CONSTANT variable '" +
              declaration.path + "'", declaration.range);
    } else {
      result->declarations.push_back(declaration);
    }
  }
}

}  // namespace

bool analyze_access_variables_from_ast(
    symbol_c *tree_root, DiagnosticEngine &diagnostics,
    AccessVariableNormalizeResult *result) {
  if (result == nullptr) return false;
  result->declarations.clear();
  result->used_access_variables = false;
  library_c *library = dynamic_cast<library_c *>(tree_root);
  if (library == nullptr) return false;
  for (int index = 0; index < library->n; ++index) {
    configuration_declaration_c *configuration =
        dynamic_cast<configuration_declaration_c *>(library->get_element(index));
    if (configuration != nullptr)
      analyze_configuration(configuration, diagnostics, result);
  }
  return !diagnostics.has_errors();
}

}  // namespace matiec

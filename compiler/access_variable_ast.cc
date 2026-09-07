#include "compiler/access_variable_ast.hh"

#include "absyntax/absyntax.hh"
#include "compiler/access_variable_normalizer.hh"
#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace matiec {
namespace {

struct Variable { symbol_c *specification = nullptr; std::string type; bool constant = false; };
struct Resource { std::map<std::string, Variable> globals; std::map<std::string, std::string> programs; };
struct Program { std::map<std::string, Variable> outputs; };
struct Selector {
  enum class Kind { field, subscript };
  Kind kind = Kind::field;
  std::string spelling;
  symbol_c *node = nullptr;
};
struct TypeCursor { symbol_c *specification = nullptr; std::string spelling; std::size_t array_dimension = 0; };
using TypeTable = std::map<std::string, symbol_c *>;

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

std::string integer_spelling(symbol_c *symbol) {
  if (neg_integer_c *negative = dynamic_cast<neg_integer_c *>(symbol))
    return "-" + integer_spelling(negative->exp);
  return token_value(symbol);
}

bool parse_integer(symbol_c *symbol, long long *value) {
  std::string text = integer_spelling(symbol);
  text.erase(std::remove(text.begin(), text.end(), '_'), text.end());
  if (text.empty()) return false;
  char *end = nullptr;
  const long long parsed = std::strtoll(text.c_str(), &end, 10);
  if (end == nullptr || *end != '\0') return false;
  *value = parsed;
  return true;
}

#define TYPE_SPELLING(class_name, spelling) \
  if (dynamic_cast<class_name *>(symbol) != nullptr) return spelling

std::string type_spelling(symbol_c *symbol) {
  if (symbol == nullptr) return "";
  if (simple_spec_init_c *node = dynamic_cast<simple_spec_init_c *>(symbol))
    return type_spelling(node->simple_specification);
  if (subrange_spec_init_c *node = dynamic_cast<subrange_spec_init_c *>(symbol))
    return type_spelling(node->subrange_specification);
  if (subrange_specification_c *node = dynamic_cast<subrange_specification_c *>(symbol))
    return type_spelling(node->integer_type_name);
  if (initialized_structure_c *node = dynamic_cast<initialized_structure_c *>(symbol))
    return type_spelling(node->structure_type_name);
  if (fb_spec_init_c *node = dynamic_cast<fb_spec_init_c *>(symbol))
    return type_spelling(node->function_block_type_name);
  if (array_spec_init_c *node = dynamic_cast<array_spec_init_c *>(symbol))
    return type_spelling(node->array_specification);
  if (array_specification_c *node = dynamic_cast<array_specification_c *>(symbol))
    return type_spelling(node->non_generic_type_name);
  const std::string value = token_value(symbol);
  if (!value.empty()) return value;
  TYPE_SPELLING(time_type_name_c, "TIME"); TYPE_SPELLING(bool_type_name_c, "BOOL");
  TYPE_SPELLING(sint_type_name_c, "SINT"); TYPE_SPELLING(int_type_name_c, "INT");
  TYPE_SPELLING(dint_type_name_c, "DINT"); TYPE_SPELLING(lint_type_name_c, "LINT");
  TYPE_SPELLING(usint_type_name_c, "USINT"); TYPE_SPELLING(uint_type_name_c, "UINT");
  TYPE_SPELLING(udint_type_name_c, "UDINT"); TYPE_SPELLING(ulint_type_name_c, "ULINT");
  TYPE_SPELLING(real_type_name_c, "REAL"); TYPE_SPELLING(lreal_type_name_c, "LREAL");
  TYPE_SPELLING(date_type_name_c, "DATE"); TYPE_SPELLING(tod_type_name_c, "TOD");
  TYPE_SPELLING(dt_type_name_c, "DT"); TYPE_SPELLING(byte_type_name_c, "BYTE");
  TYPE_SPELLING(word_type_name_c, "WORD"); TYPE_SPELLING(dword_type_name_c, "DWORD");
  TYPE_SPELLING(lword_type_name_c, "LWORD"); TYPE_SPELLING(string_type_name_c, "STRING");
  TYPE_SPELLING(wstring_type_name_c, "WSTRING");
  return "";
}
#undef TYPE_SPELLING

void add_names(symbol_c *names, symbol_c *specification, bool constant,
               std::map<std::string, Variable> *variables) {
  list_c *list = dynamic_cast<list_c *>(names);
  if (list != nullptr) {
    for (int index = 0; index < list->n; ++index) {
      const std::string name = token_value(list->get_element(index));
      if (!name.empty()) (*variables)[uppercase(name)] = {specification, type_spelling(specification), constant};
    }
    return;
  }
  const std::string name = token_value(names);
  if (!name.empty()) (*variables)[uppercase(name)] = {specification, type_spelling(specification), constant};
}

void collect_global_blocks(symbol_c *blocks_symbol, std::map<std::string, Variable> *globals) {
  global_var_declarations_list_c *blocks = dynamic_cast<global_var_declarations_list_c *>(blocks_symbol);
  if (blocks == nullptr) return;
  for (int block_index = 0; block_index < blocks->n; ++block_index) {
    global_var_declarations_c *block = dynamic_cast<global_var_declarations_c *>(blocks->get_element(block_index));
    if (block == nullptr) continue;
    global_var_decl_list_c *declarations = dynamic_cast<global_var_decl_list_c *>(block->global_var_decl_list);
    if (declarations == nullptr) continue;
    const bool constant = dynamic_cast<constant_option_c *>(block->option) != nullptr;
    for (int index = 0; index < declarations->n; ++index) {
      global_var_decl_c *declaration = dynamic_cast<global_var_decl_c *>(declarations->get_element(index));
      if (declaration == nullptr) continue;
      if (global_var_list_c *names = dynamic_cast<global_var_list_c *>(declaration->global_var_spec))
        add_names(names, declaration->type_specification, constant, globals);
      else if (global_var_spec_c *single = dynamic_cast<global_var_spec_c *>(declaration->global_var_spec))
        add_names(single->global_var_name, declaration->type_specification, constant, globals);
    }
  }
}

void collect_output_declaration(symbol_c *declaration, std::map<std::string, Variable> *outputs) {
  if (var1_init_decl_c *node = dynamic_cast<var1_init_decl_c *>(declaration))
    add_names(node->var1_list, node->spec_init, false, outputs);
  else if (array_var_init_decl_c *node = dynamic_cast<array_var_init_decl_c *>(declaration))
    add_names(node->var1_list, node->array_spec_init, false, outputs);
  else if (structured_var_init_decl_c *node = dynamic_cast<structured_var_init_decl_c *>(declaration))
    add_names(node->var1_list, node->initialized_structure, false, outputs);
}

void collect_program(program_declaration_c *node, std::map<std::string, Program> *programs) {
  Program program;
  var_declarations_list_c *blocks = dynamic_cast<var_declarations_list_c *>(node->var_declarations);
  if (blocks != nullptr) {
    for (int block_index = 0; block_index < blocks->n; ++block_index) {
      output_declarations_c *block = dynamic_cast<output_declarations_c *>(blocks->get_element(block_index));
      if (block == nullptr) continue;
      var_init_decl_list_c *declarations = dynamic_cast<var_init_decl_list_c *>(block->var_init_decl_list);
      if (declarations == nullptr) continue;
      for (int index = 0; index < declarations->n; ++index)
        collect_output_declaration(declarations->get_element(index), &program.outputs);
    }
  }
  (*programs)[uppercase(token_value(node->program_type_name))] = program;
}

void collect_types(data_type_declaration_c *node, TypeTable *types) {
  type_declaration_list_c *declarations = dynamic_cast<type_declaration_list_c *>(node->type_declaration_list);
  if (declarations == nullptr) return;
  for (int index = 0; index < declarations->n; ++index) {
    symbol_c *declaration = declarations->get_element(index);
    if (structure_type_declaration_c *item = dynamic_cast<structure_type_declaration_c *>(declaration))
      (*types)[uppercase(token_value(item->structure_type_name))] = item->structure_specification;
    else if (array_type_declaration_c *item = dynamic_cast<array_type_declaration_c *>(declaration))
      (*types)[uppercase(token_value(item->identifier))] = item->array_spec_init;
    else if (simple_type_declaration_c *item = dynamic_cast<simple_type_declaration_c *>(declaration))
      (*types)[uppercase(token_value(item->simple_type_name))] = item->simple_spec_init;
    else if (subrange_type_declaration_c *item = dynamic_cast<subrange_type_declaration_c *>(declaration))
      (*types)[uppercase(token_value(item->subrange_type_name))] = item->subrange_spec_init;
  }
}

void collect_resources(configuration_declaration_c *configuration,
                       std::map<std::string, Resource> *resources) {
  resource_declaration_list_c *list = dynamic_cast<resource_declaration_list_c *>(configuration->resource_declarations);
  if (list == nullptr) return;
  for (int index = 0; index < list->n; ++index) {
    resource_declaration_c *node = dynamic_cast<resource_declaration_c *>(list->get_element(index));
    if (node == nullptr) continue;
    Resource resource;
    collect_global_blocks(node->global_var_declarations, &resource.globals);
    single_resource_declaration_c *body = dynamic_cast<single_resource_declaration_c *>(node->resource_declaration);
    program_configuration_list_c *program_list = body == nullptr ? nullptr :
        dynamic_cast<program_configuration_list_c *>(body->program_configuration_list);
    if (program_list != nullptr) {
      for (int program_index = 0; program_index < program_list->n; ++program_index) {
        program_configuration_c *program = dynamic_cast<program_configuration_c *>(program_list->get_element(program_index));
        if (program != nullptr)
          resource.programs[uppercase(token_value(program->program_name))] = token_value(program->program_type_name);
      }
    }
    (*resources)[uppercase(token_value(node->resource_name))] = resource;
  }
}

symbol_c *unwrap_type(symbol_c *specification, const TypeTable &types) {
  symbol_c *current = specification;
  std::set<std::string> visited;
  while (current != nullptr) {
    if (simple_spec_init_c *node = dynamic_cast<simple_spec_init_c *>(current)) current = node->simple_specification;
    else if (array_spec_init_c *node = dynamic_cast<array_spec_init_c *>(current)) current = node->array_specification;
    else if (initialized_structure_c *node = dynamic_cast<initialized_structure_c *>(current)) current = node->structure_type_name;
    else {
      const std::string name = uppercase(token_value(current));
      const auto found = types.find(name);
      if (name.empty() || found == types.end() || !visited.insert(name).second) return current;
      current = found->second;
    }
  }
  return current;
}

std::vector<Selector> path_selectors(access_path_c *path) {
  std::vector<Selector> selectors;
  access_path_selector_list_c *list = path == nullptr ? nullptr : dynamic_cast<access_path_selector_list_c *>(path->selectors);
  if (list == nullptr) return selectors;
  for (int index = 0; index < list->n; ++index) {
    symbol_c *item = list->get_element(index);
    if (access_field_selector_c *field = dynamic_cast<access_field_selector_c *>(item))
      selectors.push_back({Selector::Kind::field, token_value(field->field_name), item});
    else if (access_subscript_selector_c *subscript = dynamic_cast<access_subscript_selector_c *>(item))
      selectors.push_back({Selector::Kind::subscript, integer_spelling(subscript->subscript), item});
  }
  return selectors;
}

std::string path_spelling(access_path_c *path, const std::vector<Selector> &selectors) {
  std::string result = path == nullptr ? "" : token_value(path->root_name);
  for (const Selector &selector : selectors)
    result += selector.kind == Selector::Kind::field ? "." + selector.spelling : "[" + selector.spelling + "]";
  return result;
}

bool select_field(const Selector &selector, const TypeTable &types, TypeCursor *cursor,
                  DiagnosticEngine &diagnostics, const std::string &path) {
  structure_element_declaration_list_c *fields = dynamic_cast<structure_element_declaration_list_c *>(unwrap_type(cursor->specification, types));
  if (fields == nullptr) {
    diagnostics.error("VAR_ACCESS field selector is not valid at '" + path + "'", source_range(selector.node));
    return false;
  }
  for (int index = 0; index < fields->n; ++index) {
    structure_element_declaration_c *field = dynamic_cast<structure_element_declaration_c *>(fields->get_element(index));
    if (field != nullptr && uppercase(token_value(field->structure_element_name)) == uppercase(selector.spelling)) {
      cursor->specification = field->spec_init;
      cursor->spelling = type_spelling(field->spec_init);
      cursor->array_dimension = 0;
      return true;
    }
  }
  diagnostics.error("Unresolved VAR_ACCESS field '" + selector.spelling + "' in target '" + path + "'", source_range(selector.node));
  return false;
}

bool select_subscript(const Selector &selector, const TypeTable &types, TypeCursor *cursor,
                      DiagnosticEngine &diagnostics, const std::string &path) {
  array_specification_c *array = dynamic_cast<array_specification_c *>(unwrap_type(cursor->specification, types));
  array_subrange_list_c *dimensions = array == nullptr ? nullptr : dynamic_cast<array_subrange_list_c *>(array->array_subrange_list);
  if (dimensions == nullptr || cursor->array_dimension >= static_cast<std::size_t>(dimensions->n)) {
    diagnostics.error("VAR_ACCESS subscript is not valid at '" + path + "'", source_range(selector.node));
    return false;
  }
  long long index = 0, lower = 0, upper = 0;
  subrange_c *range = dynamic_cast<subrange_c *>(dimensions->get_element(static_cast<int>(cursor->array_dimension)));
  access_subscript_selector_c *subscript = dynamic_cast<access_subscript_selector_c *>(selector.node);
  if (subscript == nullptr || !parse_integer(subscript->subscript, &index) || range == nullptr ||
      !parse_integer(range->lower_limit, &lower) || !parse_integer(range->upper_limit, &upper)) {
    diagnostics.error("VAR_ACCESS requires an integer constant subscript", source_range(selector.node));
    return false;
  }
  if (index < lower || index > upper) {
    diagnostics.error("VAR_ACCESS subscript " + selector.spelling + " is outside declared bounds", source_range(selector.node));
    return false;
  }
  ++cursor->array_dimension;
  if (cursor->array_dimension == static_cast<std::size_t>(dimensions->n)) {
    cursor->specification = array->non_generic_type_name;
    cursor->spelling = type_spelling(array->non_generic_type_name);
    cursor->array_dimension = 0;
  }
  return true;
}

bool resolve_root(const std::string &root, const std::vector<Selector> &selectors,
                  const std::map<std::string, Variable> &globals,
                  const std::map<std::string, Resource> &resources,
                  const std::map<std::string, Program> &programs, Variable *variable,
                  std::size_t *consumed, DiagnosticEngine &diagnostics, const SourceRange &range) {
  const auto global = globals.find(uppercase(root));
  if (global != globals.end()) { *variable = global->second; return true; }
  const auto resource = resources.find(uppercase(root));
  if (resource == resources.end()) {
    diagnostics.error("Unresolved VAR_ACCESS target: " + root, range); return false;
  }
  if (selectors.empty() || selectors[0].kind != Selector::Kind::field) {
    diagnostics.error("VAR_ACCESS resource target requires a member: " + root, range); return false;
  }
  const auto resource_global = resource->second.globals.find(uppercase(selectors[0].spelling));
  if (resource_global != resource->second.globals.end()) {
    *variable = resource_global->second; *consumed = 1; return true;
  }
  const auto instance = resource->second.programs.find(uppercase(selectors[0].spelling));
  if (instance == resource->second.programs.end() || selectors.size() < 2 || selectors[1].kind != Selector::Kind::field) {
    diagnostics.error("Unresolved VAR_ACCESS resource member: " + selectors[0].spelling, source_range(selectors[0].node));
    return false;
  }
  const auto program = programs.find(uppercase(instance->second));
  if (program == programs.end()) {
    diagnostics.error("Unresolved VAR_ACCESS program type: " + instance->second, source_range(selectors[0].node)); return false;
  }
  const auto output = program->second.outputs.find(uppercase(selectors[1].spelling));
  if (output == program->second.outputs.end()) {
    diagnostics.error("Unresolved VAR_ACCESS program output: " + selectors[1].spelling, source_range(selectors[1].node)); return false;
  }
  *variable = output->second; *consumed = 2; return true;
}

void analyze_configuration(configuration_declaration_c *configuration, const TypeTable &types,
                           const std::map<std::string, Program> &programs,
                           DiagnosticEngine &diagnostics, AccessVariableNormalizeResult *result) {
  access_declarations_c *access = dynamic_cast<access_declarations_c *>(configuration->access_declarations);
  if (access == nullptr) return;
  result->used_access_variables = true;
  std::map<std::string, Variable> globals;
  std::map<std::string, Resource> resources;
  collect_global_blocks(configuration->global_var_declarations, &globals);
  collect_resources(configuration, &resources);
  std::set<std::string> access_names;
  access_declaration_list_c *declarations = dynamic_cast<access_declaration_list_c *>(access->access_declaration_list);
  if (declarations == nullptr) return;
  const std::string configuration_name = token_value(configuration->configuration_name);
  for (int index = 0; index < declarations->n; ++index) {
    access_declaration_c *node = dynamic_cast<access_declaration_c *>(declarations->get_element(index));
    if (node == nullptr) continue;
    AccessVariableAst declaration;
    declaration.configuration = configuration_name;
    declaration.name = token_value(node->access_name);
    access_path_c *path = dynamic_cast<access_path_c *>(node->access_path);
    const std::vector<Selector> selectors = path_selectors(path);
    declaration.path = path_spelling(path, selectors);
    for (const Selector &selector : selectors)
      declaration.selectors.push_back({selector.kind == Selector::Kind::field ? AccessVariableAst::Selector::Kind::field : AccessVariableAst::Selector::Kind::subscript, selector.spelling});
    declaration.type = type_spelling(node->type_name);
    declaration.direction = dynamic_cast<read_write_c *>(node->direction) == nullptr ? "READ_ONLY" : "READ_WRITE";
    declaration.range = source_range(node);
    if (!access_names.insert(uppercase(declaration.name)).second) {
      diagnostics.error("Duplicate VAR_ACCESS name: " + declaration.name, declaration.range); continue;
    }
    Variable target;
    std::size_t consumed = 0;
    const std::string root = path == nullptr ? "" : token_value(path->root_name);
    if (!resolve_root(root, selectors, globals, resources, programs, &target, &consumed, diagnostics, declaration.range)) continue;
    TypeCursor cursor{target.specification, target.type, 0};
    bool valid = true;
    for (std::size_t selector_index = consumed; selector_index < selectors.size() && valid; ++selector_index) {
      const Selector &selector = selectors[selector_index];
      valid = selector.kind == Selector::Kind::field
                  ? select_field(selector, types, &cursor, diagnostics, declaration.path)
                  : select_subscript(selector, types, &cursor, diagnostics, declaration.path);
    }
    if (valid && cursor.array_dimension != 0) {
      diagnostics.error("VAR_ACCESS requires a subscript for every array dimension",
                        declaration.range);
      valid = false;
    }
    if (!valid) continue;
    if (uppercase(declaration.type) != uppercase(cursor.spelling))
      diagnostics.error("VAR_ACCESS type does not match target '" + declaration.path + "'", declaration.range);
    else if (declaration.direction == "READ_WRITE" && target.constant)
      diagnostics.error("READ_WRITE VAR_ACCESS cannot target CONSTANT variable '" + declaration.path + "'", declaration.range);
    else
      result->declarations.push_back(declaration);
  }
}

}  // namespace

bool analyze_access_variables_from_ast(symbol_c *tree_root, DiagnosticEngine &diagnostics,
                                       AccessVariableNormalizeResult *result) {
  if (result == nullptr) return false;
  result->declarations.clear(); result->used_access_variables = false;
  library_c *library = dynamic_cast<library_c *>(tree_root);
  if (library == nullptr) return false;
  TypeTable types;
  std::map<std::string, Program> programs;
  for (int index = 0; index < library->n; ++index) {
    symbol_c *item = library->get_element(index);
    if (data_type_declaration_c *type = dynamic_cast<data_type_declaration_c *>(item)) collect_types(type, &types);
    else if (program_declaration_c *program = dynamic_cast<program_declaration_c *>(item)) collect_program(program, &programs);
  }
  for (int index = 0; index < library->n; ++index) {
    configuration_declaration_c *configuration = dynamic_cast<configuration_declaration_c *>(library->get_element(index));
    if (configuration != nullptr) analyze_configuration(configuration, types, programs, diagnostics, result);
  }
  return !diagnostics.has_errors();
}

}  // namespace matiec

#include "compiler/namespace_ast_analysis.hh"

#include "absyntax/absyntax.hh"
#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>
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

std::string namespace_name(symbol_c *symbol) {
  namespace_name_c *parts = dynamic_cast<namespace_name_c *>(symbol);
  if (parts == nullptr) return "";
  std::string name;
  for (int index = 0; index < parts->n; ++index) {
    token_c *part = dynamic_cast<token_c *>(parts->get_element(index));
    if (part == nullptr || part->value == nullptr) continue;
    if (!name.empty()) name += ".";
    name += part->value;
  }
  return name;
}

void collect_imports(namespace_element_list_c *elements,
                     NamespaceDeclarationAst *declaration,
                     std::vector<std::pair<std::string, SourceRange> > *all_imports) {
  if (elements == nullptr) return;
  for (int index = 0; index < elements->n; ++index) {
    namespace_using_declaration_c *using_node =
        dynamic_cast<namespace_using_declaration_c *>(elements->get_element(index));
    if (using_node == nullptr) continue;
    NamespaceImportAst import;
    import.namespace_name = namespace_name(using_node->namespace_name);
    import.range = source_range(using_node);
    declaration->imports.push_back(import);
    all_imports->push_back({uppercase(import.namespace_name), import.range});
  }
}

}  // namespace

bool analyze_namespaces_from_ast(symbol_c *tree_root,
                                 DiagnosticEngine &diagnostics,
                                 NamespaceAnalysisResult *result) {
  if (tree_root == nullptr || result == nullptr) return false;
  result->declarations.clear();
  result->used_namespaces = false;
  library_c *library = dynamic_cast<library_c *>(tree_root);
  if (library == nullptr) return false;

  std::set<std::string> namespace_names;
  std::vector<std::pair<std::string, SourceRange> > imports;
  for (int index = 0; index < library->n; ++index) {
    symbol_c *element = library->get_element(index);
    if (namespace_using_declaration_c *using_node =
            dynamic_cast<namespace_using_declaration_c *>(element)) {
      result->used_namespaces = true;
      imports.push_back({uppercase(namespace_name(using_node->namespace_name)),
                         source_range(using_node)});
      continue;
    }
    namespace_declaration_c *node =
        dynamic_cast<namespace_declaration_c *>(element);
    if (node == nullptr) continue;
    result->used_namespaces = true;
    NamespaceDeclarationAst declaration;
    declaration.name = namespace_name(node->namespace_name);
    declaration.visibility =
        dynamic_cast<namespace_internal_c *>(node->visibility) == nullptr
            ? NamespaceVisibility::public_ : NamespaceVisibility::internal;
    declaration.range = source_range(node->namespace_name);
    const std::string key = uppercase(declaration.name);
    if (!namespace_names.insert(key).second) {
      diagnostics.error("Namespace reopening is not supported: " +
                            declaration.name, declaration.range);
      continue;
    }
    collect_imports(dynamic_cast<namespace_element_list_c *>(node->elements),
                    &declaration, &imports);
    result->declarations.push_back(declaration);
  }

  for (const auto &import : imports) {
    if (namespace_names.count(import.first) == 0)
      diagnostics.error("USING refers to an unknown namespace: " + import.first,
                        import.second);
  }
  return !diagnostics.has_errors();
}

}  // namespace matiec

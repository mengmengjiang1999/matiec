#ifndef MATIEC_COMPILER_EXPERIMENTAL_SYNTAX_MODEL_HH
#define MATIEC_COMPILER_EXPERIMENTAL_SYNTAX_MODEL_HH

#include "compiler_types.hh"

#include <string>
#include <utility>
#include <vector>

namespace matiec {

enum class NamespaceVisibility { public_, internal };

struct NamespaceImportAst {
  std::string namespace_name;
  SourceRange range;
};

struct NamespaceDeclarationAst {
  std::string name;
  NamespaceVisibility visibility = NamespaceVisibility::public_;
  SourceRange range;
  std::vector<NamespaceImportAst> imports;
};

struct ObjectMethodAst {
  std::string owner;
  std::string name;
  std::string return_type;
  std::string lowered_name;
  std::vector<std::pair<std::string, std::string> > owner_fields;
  SourceRange range;
};

struct AccessVariableAst {
  struct Selector {
    enum class Kind { field, subscript };
    Kind kind = Kind::field;
    std::string spelling;
  };

  std::string configuration;
  std::string name;
  std::string path;
  std::vector<Selector> selectors;
  std::string type;
  std::string direction;
  SourceRange range;
};

struct ModernLibraryFunctionAst {
  std::string name;
  std::string source_signature;
  std::string lowered_name;
  SourceRange range;
};

struct ExperimentalSyntaxModel {
  std::vector<NamespaceDeclarationAst> namespaces;
  std::vector<ObjectMethodAst> methods;
  std::vector<AccessVariableAst> access_variables;
  std::vector<ModernLibraryFunctionAst> library_functions;

  void clear() {
    namespaces.clear();
    methods.clear();
    access_variables.clear();
    library_functions.clear();
  }
};

}  // namespace matiec

#endif

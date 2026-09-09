#ifndef MATIEC_COMPILER_PARSER_STATE_HH
#define MATIEC_COMPILER_PARSER_STATE_HH

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct runtime_options_t {
  bool allow_void_datatype = false;
  bool allow_missing_var_in = false;
  bool disable_implicit_en_eno = false;
  bool pre_parsing = false;
  bool safe_extensions = false;
  bool full_token_loc = false;
  bool conversion_functions = false;
  bool nested_comments = false;
  bool ref_standard_extensions = false;
  bool ref_nonstand_extensions = false;
  bool nonliteral_in_array_size = false;
  bool utf8_source_and_strings = false;
  bool iec2025_experimental = false;
  bool register_experimental_assert = false;
  const char *includedir = nullptr;
  bool relaxed_datatype_model = false;
};

class symbol_c;

namespace matiec {

enum class IncludeResolveStatus { resolved, use_filesystem, not_found, error };
using IncludeResolver = std::function<IncludeResolveStatus(
    std::string_view, std::string *, std::string *, std::string *)>;
using CancellationChecker = std::function<bool()>;

enum class NamespaceLookupStatus {
  unchanged,
  resolved,
  unknown_qualified,
  ambiguous,
  inaccessible
};

struct NamespaceLookup {
  NamespaceLookupStatus status = NamespaceLookupStatus::unchanged;
  std::string spelling;
  int token = 0;
};

class AstArena;
class ParserSymbolTables;

struct ParserState {
  ParserState();
  ~ParserState();

  ParserState(const ParserState &) = delete;
  ParserState &operator=(const ParserState &) = delete;

  runtime_options_t options;
  bool preparse = false;
  bool goto_body = false;
  bool goto_sfc_qualifier = false;
  bool goto_sfc_priority = false;
  bool goto_task_init = false;
  bool pop_state = false;
  int syntax_errors = 0;
  bool allow_function_overloading = false;
  bool allow_extensible_function_parameters = false;
  bool allow_ref_dereferencing = false;
  bool allow_ref_to_any = false;
  bool allow_ref_to_in_derived_datatypes = false;
  ::symbol_c *tree_root = nullptr;
  const char *current_error_msg = nullptr;

  void reset_for_parse();
  void bind_ast_arena(AstArena &arena);
  AstArena *ast_arena() const;
  ParserSymbolTables &symbols();
  const ParserSymbolTables &symbols() const;
  void set_include_resolver(IncludeResolver resolver);
  bool has_include_resolver() const;
  IncludeResolveStatus resolve_include(std::string_view requested,
                                       std::string *display_name,
                                       std::string *contents,
                                       std::string *error) const;
  void set_cancellation_checker(CancellationChecker checker);
  bool cancellation_requested() const;
  void bind_lexer_scanner(void *scanner);
  void *lexer_scanner() const;
  void begin_namespace_name();
  void enter_namespace(std::string_view name, bool internal_visibility);
  void leave_namespace();
  void add_namespace_import(std::string_view name);
  void register_namespace_symbol(std::string_view name, int token);
  bool parsing_namespace_name() const;
  NamespaceLookup resolve_namespace_name(std::string_view name) const;

 private:
  AstArena *ast_arena_ = nullptr;
  std::unique_ptr<ParserSymbolTables> symbols_;
  IncludeResolver include_resolver_;
  CancellationChecker cancellation_checker_;
  void *lexer_scanner_ = nullptr;
  struct NamespaceSymbol {
    std::string owner;
    std::string lowered;
    int token = 0;
    bool internal_visibility = false;
  };
  std::map<std::string, bool> namespaces_;
  std::map<std::string, std::vector<std::string> > namespace_imports_;
  std::map<std::string, NamespaceSymbol> namespace_symbols_;
  std::string namespace_scope_;
  bool parsing_namespace_name_ = false;
};

class ActiveRuntimeOptionsScope {
 public:
  explicit ActiveRuntimeOptionsScope(runtime_options_t &options);
  ~ActiveRuntimeOptionsScope();

  ActiveRuntimeOptionsScope(const ActiveRuntimeOptionsScope &) = delete;
  ActiveRuntimeOptionsScope &operator=(const ActiveRuntimeOptionsScope &) = delete;

 private:
  runtime_options_t *previous_;
};

runtime_options_t &active_runtime_options();

}  // namespace matiec

#endif

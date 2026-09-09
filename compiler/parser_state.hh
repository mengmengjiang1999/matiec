#ifndef MATIEC_COMPILER_PARSER_STATE_HH
#define MATIEC_COMPILER_PARSER_STATE_HH

#include <functional>
#include <memory>
#include <string>
#include <string_view>

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

namespace matiec {

enum class IncludeResolveStatus { resolved, use_filesystem, not_found, error };
using IncludeResolver = std::function<IncludeResolveStatus(
    std::string_view, std::string *, std::string *, std::string *)>;

class AstArena;
struct DeclarationSymbolTables;
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

  void reset_for_parse();
  void bind_ast_arena(AstArena &arena);
  AstArena *ast_arena() const;
  void bind_declaration_symbols(DeclarationSymbolTables &tables);
  DeclarationSymbolTables &declaration_symbols() const;
  ParserSymbolTables &symbols();
  const ParserSymbolTables &symbols() const;
  void set_include_resolver(IncludeResolver resolver);
  bool has_include_resolver() const;
  IncludeResolveStatus resolve_include(std::string_view requested,
                                       std::string *display_name,
                                       std::string *contents,
                                       std::string *error) const;

 private:
  AstArena *ast_arena_ = nullptr;
  DeclarationSymbolTables *declaration_symbols_ = nullptr;
  std::unique_ptr<ParserSymbolTables> symbols_;
  IncludeResolver include_resolver_;
};

class ActiveParserStateScope {
 public:
  explicit ActiveParserStateScope(ParserState &state);
  ~ActiveParserStateScope();

  ActiveParserStateScope(const ActiveParserStateScope &) = delete;
  ActiveParserStateScope &operator=(const ActiveParserStateScope &) = delete;

 private:
  ParserState *previous_;
};

ParserState &active_parser_state();
ParserState *active_parser_state_or_null();
runtime_options_t &active_runtime_options();

}  // namespace matiec

#endif

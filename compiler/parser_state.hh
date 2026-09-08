#ifndef MATIEC_COMPILER_PARSER_STATE_HH
#define MATIEC_COMPILER_PARSER_STATE_HH

#include <memory>

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

  void reset_for_parse();
  void bind_ast_arena(AstArena &arena);
  AstArena *ast_arena() const;
  ParserSymbolTables &symbols();
  const ParserSymbolTables &symbols() const;

 private:
  AstArena *ast_arena_ = nullptr;
  std::unique_ptr<ParserSymbolTables> symbols_;
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
runtime_options_t &active_runtime_options();

}  // namespace matiec

#endif

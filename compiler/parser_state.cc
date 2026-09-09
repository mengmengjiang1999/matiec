#include "compiler/parser_state.hh"
#include "compiler/declaration_symbol_tables.hh"
#include "compiler/parser_symbol_tables.hh"

#include <stdexcept>

namespace matiec {
namespace {
thread_local ParserState *current_parser_state = nullptr;
thread_local ParserState fallback_parser_state;
}  // namespace

ParserState::ParserState() : symbols_(new ParserSymbolTables) {}

ParserState::~ParserState() = default;

void ParserState::reset_for_parse() {
  symbols_->clear();
  preparse = false;
  goto_body = false;
  goto_sfc_qualifier = false;
  goto_sfc_priority = false;
  goto_task_init = false;
  pop_state = false;
  syntax_errors = 0;
}

void ParserState::bind_ast_arena(AstArena &arena) { ast_arena_ = &arena; }

AstArena *ParserState::ast_arena() const { return ast_arena_; }

void ParserState::bind_declaration_symbols(DeclarationSymbolTables &tables) {
  declaration_symbols_ = &tables;
}

DeclarationSymbolTables &ParserState::declaration_symbols() const {
  if (declaration_symbols_ == nullptr)
    throw std::logic_error("ParserState has no declaration symbol tables");
  return *declaration_symbols_;
}

ParserSymbolTables &ParserState::symbols() { return *symbols_; }

const ParserSymbolTables &ParserState::symbols() const { return *symbols_; }

ActiveParserStateScope::ActiveParserStateScope(ParserState &state)
    : previous_(current_parser_state) {
  current_parser_state = &state;
}

ActiveParserStateScope::~ActiveParserStateScope() {
  current_parser_state = previous_;
}

ParserState &active_parser_state() {
  return current_parser_state == nullptr ? fallback_parser_state
                                         : *current_parser_state;
}

runtime_options_t &active_runtime_options() {
  return active_parser_state().options;
}

}  // namespace matiec

#include "compiler/parser_state.hh"
#include "compiler/parser_symbol_tables.hh"

#include <stdexcept>
#include <utility>

namespace matiec {
namespace {
thread_local runtime_options_t *current_runtime_options = nullptr;
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

ParserSymbolTables &ParserState::symbols() { return *symbols_; }

const ParserSymbolTables &ParserState::symbols() const { return *symbols_; }

void ParserState::set_include_resolver(IncludeResolver resolver) {
  include_resolver_ = std::move(resolver);
}

bool ParserState::has_include_resolver() const {
  return static_cast<bool>(include_resolver_);
}

IncludeResolveStatus ParserState::resolve_include(
    std::string_view requested, std::string *display_name,
    std::string *contents, std::string *error) const {
  if (!include_resolver_) return IncludeResolveStatus::not_found;
  return include_resolver_(requested, display_name, contents, error);
}

void ParserState::set_cancellation_checker(CancellationChecker checker) {
  cancellation_checker_ = std::move(checker);
}

bool ParserState::cancellation_requested() const {
  return cancellation_checker_ && cancellation_checker_();
}

ActiveRuntimeOptionsScope::ActiveRuntimeOptionsScope(runtime_options_t &options)
    : previous_(current_runtime_options) {
  current_runtime_options = &options;
}

ActiveRuntimeOptionsScope::~ActiveRuntimeOptionsScope() {
  current_runtime_options = previous_;
}

runtime_options_t &active_runtime_options() {
  if (current_runtime_options == nullptr)
    throw std::logic_error("No active runtime options");
  return *current_runtime_options;
}

}  // namespace matiec

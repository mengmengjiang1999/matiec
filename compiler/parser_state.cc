#include "compiler/parser_state.hh"
#include "compiler/parser_symbol_tables.hh"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace matiec {
namespace {
thread_local runtime_options_t *current_runtime_options = nullptr;

std::string canonical_namespace_name(std::string_view value) {
  std::string result;
  result.reserve(value.size());
  for (unsigned char ch : value) {
    if (ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v') continue;
    result += ch >= 'a' && ch <= 'z' ? static_cast<char>(ch - 'a' + 'A')
                                     : static_cast<char>(ch);
  }
  return result;
}

std::string lowered_namespace_name(const std::string &qualified) {
  std::string result = "MATIECNS";
  std::size_t begin = 0;
  while (begin < qualified.size()) {
    const std::size_t end = qualified.find('.', begin);
    const std::size_t size =
        (end == std::string::npos ? qualified.size() : end) - begin;
    result += std::to_string(size);
    result.append(qualified, begin, size);
    if (end == std::string::npos) break;
    begin = end + 1;
  }
  return result;
}

bool same_namespace_tree(const std::string &requester,
                         const std::string &owner) {
  return requester == owner ||
         (requester.size() > owner.size() &&
          requester.compare(0, owner.size(), owner) == 0 &&
          requester[owner.size()] == '.');
}
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
  allow_function_overloading = false;
  allow_extensible_function_parameters = false;
  allow_ref_dereferencing = false;
  allow_ref_to_any = false;
  allow_ref_to_in_derived_datatypes = false;
  tree_root = nullptr;
  current_error_msg = nullptr;
  namespaces_.clear();
  namespace_imports_.clear();
  namespace_symbols_.clear();
  namespace_scope_.clear();
  parsing_namespace_name_ = false;
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

void ParserState::bind_lexer_scanner(void *scanner) {
  lexer_scanner_ = scanner;
}

void *ParserState::lexer_scanner() const { return lexer_scanner_; }

void ParserState::begin_namespace_name() { parsing_namespace_name_ = true; }

void ParserState::enter_namespace(std::string_view name,
                                  bool internal_visibility) {
  namespace_scope_ = canonical_namespace_name(name);
  namespaces_[namespace_scope_] = internal_visibility;
  parsing_namespace_name_ = false;
}

void ParserState::leave_namespace() {
  namespace_scope_.clear();
  parsing_namespace_name_ = false;
}

void ParserState::add_namespace_import(std::string_view name) {
  const std::string imported = canonical_namespace_name(name);
  std::vector<std::string> &imports = namespace_imports_[namespace_scope_];
  if (std::find(imports.begin(), imports.end(), imported) == imports.end())
    imports.push_back(imported);
  parsing_namespace_name_ = false;
}

void ParserState::register_namespace_symbol(std::string_view name, int token) {
  if (namespace_scope_.empty()) return;
  const std::string member = canonical_namespace_name(name);
  const std::string qualified = namespace_scope_ + "." + member;
  NamespaceSymbol symbol;
  symbol.owner = namespace_scope_;
  symbol.lowered = lowered_namespace_name(qualified);
  symbol.token = token;
  const auto visibility = namespaces_.find(namespace_scope_);
  symbol.internal_visibility =
      visibility != namespaces_.end() && visibility->second;
  namespace_symbols_[qualified] = std::move(symbol);
}

bool ParserState::parsing_namespace_name() const {
  return parsing_namespace_name_;
}

NamespaceLookup ParserState::resolve_namespace_name(std::string_view name) const {
  NamespaceLookup result;
  if (parsing_namespace_name_) return result;
  const std::string canonical = canonical_namespace_name(name);
  const bool qualified = canonical.find('.') != std::string::npos;
  if (qualified) {
    const auto found = namespace_symbols_.find(canonical);
    if (found != namespace_symbols_.end()) {
      if (found->second.internal_visibility &&
          !same_namespace_tree(namespace_scope_, found->second.owner)) {
        result.status = NamespaceLookupStatus::inaccessible;
        result.spelling = canonical;
        return result;
      }
      result.status = NamespaceLookupStatus::resolved;
      result.spelling = found->second.lowered;
      result.token = found->second.token;
      return result;
    }
    for (const auto &entry : namespaces_) {
      if (canonical.size() > entry.first.size() &&
          canonical.compare(0, entry.first.size(), entry.first) == 0 &&
          canonical[entry.first.size()] == '.') {
        result.status = NamespaceLookupStatus::unknown_qualified;
        result.spelling = canonical;
        return result;
      }
    }
    return result;
  }

  std::vector<const NamespaceSymbol *> candidates;
  if (!namespace_scope_.empty()) {
    const auto current =
        namespace_symbols_.find(namespace_scope_ + "." + canonical);
    if (current != namespace_symbols_.end()) candidates.push_back(&current->second);
  }
  if (candidates.empty()) {
    std::vector<std::string> imports;
    const auto root = namespace_imports_.find("");
    if (root != namespace_imports_.end()) imports = root->second;
    const auto local = namespace_imports_.find(namespace_scope_);
    if (local != namespace_imports_.end())
      imports.insert(imports.end(), local->second.begin(), local->second.end());
    for (const std::string &imported : imports) {
      const auto found = namespace_symbols_.find(imported + "." + canonical);
      if (found != namespace_symbols_.end() &&
          std::find(candidates.begin(), candidates.end(), &found->second) ==
              candidates.end())
        candidates.push_back(&found->second);
    }
  }
  if (candidates.size() > 1) {
    result.status = NamespaceLookupStatus::ambiguous;
    result.spelling = canonical;
  } else if (candidates.size() == 1) {
    const NamespaceSymbol &found = *candidates.front();
    if (found.internal_visibility &&
        !same_namespace_tree(namespace_scope_, found.owner)) {
      result.status = NamespaceLookupStatus::inaccessible;
      result.spelling = canonical;
    } else {
      result.status = NamespaceLookupStatus::resolved;
      result.spelling = found.lowered;
      result.token = found.token;
    }
  }
  return result;
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

#include "compiler/declaration_symbol_tables.hh"

namespace matiec {
namespace {

thread_local DeclarationSymbolTables *current_tables = nullptr;
thread_local DeclarationSymbolTables fallback_tables;

}  // namespace

void DeclarationSymbolTables::clear() {
  functions.reset();
  function_blocks.clear();
  programs.clear();
  types.clear();
}

DeclarationSymbolTables &active_declaration_symbol_tables() {
  return current_tables == nullptr ? fallback_tables : *current_tables;
}

ActiveDeclarationSymbolTablesScope::ActiveDeclarationSymbolTablesScope(
    DeclarationSymbolTables &tables)
    : previous_(current_tables) {
  current_tables = &tables;
}

ActiveDeclarationSymbolTablesScope::~ActiveDeclarationSymbolTablesScope() {
  current_tables = previous_;
}

}  // namespace matiec

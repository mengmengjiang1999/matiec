#include "compiler/declaration_symbol_tables.hh"
#include <stdexcept>

namespace matiec {
namespace {
thread_local DeclarationSymbolTables *current_declaration_symbols = nullptr;
}

void DeclarationSymbolTables::clear() {
  functions.reset();
  function_blocks.clear();
  programs.clear();
  types.clear();
}

DeclarationSymbolTables &active_declaration_symbol_tables() {
  if (current_declaration_symbols == nullptr)
    throw std::logic_error("No active declaration symbol tables");
  return *current_declaration_symbols;
}

ActiveDeclarationSymbolTablesScope::ActiveDeclarationSymbolTablesScope(
    DeclarationSymbolTables &tables)
    : previous_(current_declaration_symbols) {
  current_declaration_symbols = &tables;
}

ActiveDeclarationSymbolTablesScope::~ActiveDeclarationSymbolTablesScope() {
  current_declaration_symbols = previous_;
}

}  // namespace matiec

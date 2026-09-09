#include "compiler/declaration_symbol_tables.hh"
#include "compiler/parser_state.hh"

namespace matiec {

void DeclarationSymbolTables::clear() {
  functions.reset();
  function_blocks.clear();
  programs.clear();
  types.clear();
}

DeclarationSymbolTables &active_declaration_symbol_tables() {
  return active_parser_state().declaration_symbols();
}

}  // namespace matiec

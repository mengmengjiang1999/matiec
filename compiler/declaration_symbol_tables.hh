#ifndef MATIEC_COMPILER_DECLARATION_SYMBOL_TABLES_HH
#define MATIEC_COMPILER_DECLARATION_SYMBOL_TABLES_HH

#include "../util/dsymtable.hh"
#include "../util/symtable.hh"

namespace matiec {

struct DeclarationSymbolTables {
  dsymtable_c<function_declaration_c *> functions;
  symtable_c<function_block_declaration_c *> function_blocks;
  symtable_c<program_declaration_c *> programs;
  symtable_c<symbol_c *> types;

  void clear();
};

DeclarationSymbolTables &active_declaration_symbol_tables();

}  // namespace matiec

#endif

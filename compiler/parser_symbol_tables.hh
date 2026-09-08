#ifndef MATIEC_COMPILER_PARSER_SYMBOL_TABLES_HH
#define MATIEC_COMPILER_PARSER_SYMBOL_TABLES_HH

#include "../util/symtable.hh"

namespace matiec {

class ParserSymbolTables {
 public:
  symtable_c<int> library_elements;
  symtable_c<int> variable_names;
  symtable_c<int> direct_variables;

  void clear() {
    library_elements.clear();
    variable_names.clear();
    direct_variables.clear();
  }
};

}  // namespace matiec

#endif

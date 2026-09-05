#ifndef MATIEC_COMPILER_LEGACY_GLOBAL_STATE_ADAPTER_HH
#define MATIEC_COMPILER_LEGACY_GLOBAL_STATE_ADAPTER_HH

#include "compiler/compilation_context.hh"

class symbol_c;

namespace matiec {

class LegacyGlobalStateAdapter {
 public:
  explicit LegacyGlobalStateAdapter(CompilationContext &context);

  int parse(symbol_c **tree_root) const;
  void initialize_symbol_tables(symbol_c *tree_root) const;

 private:
  CompilationContext &context_;
};

}  // namespace matiec

#endif

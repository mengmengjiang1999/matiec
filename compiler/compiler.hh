#ifndef MATIEC_COMPILER_COMPILER_HH
#define MATIEC_COMPILER_COMPILER_HH

#include "compiler/compilation_context.hh"

namespace matiec {

class Compiler {
 public:
  CompilationResult compile(CompilationContext &context) const;
};

}  // namespace matiec

#endif

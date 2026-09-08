#ifndef MATIEC_COMPILER_COMPILER_HH
#define MATIEC_COMPILER_COMPILER_HH

#include "compiler/compilation_context.hh"

#include <cstddef>
#include <functional>
#include <vector>

namespace matiec {

class Compiler {
 public:
  CompilationResult compile(CompilationContext &context) const;
  std::vector<CompilationResult> compile_parallel(
      const std::vector<std::reference_wrapper<CompilationContext>> &contexts,
      std::size_t max_concurrency = 0) const;
};

}  // namespace matiec

#endif

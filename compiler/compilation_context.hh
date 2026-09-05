#ifndef MATIEC_COMPILER_COMPILATION_CONTEXT_HH
#define MATIEC_COMPILER_COMPILATION_CONTEXT_HH

#include "ast_arena.hh"
#include "compiler_types.hh"
#include "diagnostic_engine.hh"

#include <string>

namespace matiec {

class CompilationContext {
 public:
  CompilationContext() = default;
  explicit CompilationContext(CompilerOptions options);

  CompilerOptions &options();
  const CompilerOptions &options() const;

  DiagnosticEngine &diagnostics();
  const DiagnosticEngine &diagnostics() const;

  AstArena &ast_arena();
  const AstArena &ast_arena() const;

  void set_source_path(std::string source_path);
  const std::string &source_path() const;

 private:
  CompilerOptions options_;
  DiagnosticEngine diagnostics_;
  AstArena ast_arena_;
  std::string source_path_;
};

}  // namespace matiec

#endif

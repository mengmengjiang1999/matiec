#ifndef MATIEC_COMPILER_COMPILATION_CONTEXT_HH
#define MATIEC_COMPILER_COMPILATION_CONTEXT_HH

#include "ast_arena.hh"
#include "compiler_types.hh"
#include "diagnostic_engine.hh"
#include "output_manager.hh"
#include "source_manager.hh"

#include <string>

namespace matiec {

class CompilationContext {
 public:
  CompilationContext();
  explicit CompilationContext(CompilerOptions options);

  CompilerOptions &options();
  const CompilerOptions &options() const;

  DiagnosticEngine &diagnostics();
  const DiagnosticEngine &diagnostics() const;

  AstArena &ast_arena();
  const AstArena &ast_arena() const;

  OutputManager &outputs();
  const OutputManager &outputs() const;

  void set_source_path(std::string source_path);
  void set_source(std::string display_name, std::string source);
  const std::string &source_path() const;
  SourceManager &sources();
  const SourceManager &sources() const;

 private:
  CompilerOptions options_;
  DiagnosticEngine diagnostics_;
  OutputManager outputs_;
  AstArena ast_arena_;
  SourceManager sources_;
};

}  // namespace matiec

#endif

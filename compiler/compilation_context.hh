#ifndef MATIEC_COMPILER_COMPILATION_CONTEXT_HH
#define MATIEC_COMPILER_COMPILATION_CONTEXT_HH

#include "ast_arena.hh"
#include "analysis_store.hh"
#include "compiler_types.hh"
#include "declaration_symbol_tables.hh"
#include "diagnostic_engine.hh"
#include "experimental_syntax_model.hh"
#include "output_manager.hh"
#include "parser_state.hh"
#include "source_manager.hh"

#include <atomic>
#include <cstddef>
#include <string>

namespace matiec {

struct CompilationLimits {
  std::size_t max_source_bytes = 0;
  std::size_t max_diagnostics = 0;
  std::size_t max_output_bytes = 0;
};

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

  AnalysisStore &analysis();
  const AnalysisStore &analysis() const;

  DeclarationSymbolTables &declaration_symbols();
  const DeclarationSymbolTables &declaration_symbols() const;

  ParserState &parser_state();
  const ParserState &parser_state() const;

  OutputManager &outputs();
  const OutputManager &outputs() const;

  void set_source_path(std::string source_path);
  void set_source(std::string display_name, std::string source);
  const std::string &source_path() const;
  SourceManager &sources();
  const SourceManager &sources() const;

  ExperimentalSyntaxModel &experimental_syntax();
  const ExperimentalSyntaxModel &experimental_syntax() const;

  void set_limits(CompilationLimits limits);
  const CompilationLimits &limits() const;
  void request_cancel();
  void reset_cancel();
  bool cancel_requested() const;

 private:
  CompilerOptions options_;
  DiagnosticEngine diagnostics_;
  OutputManager outputs_;
  AstArena ast_arena_;
  AnalysisStore analysis_;
  DeclarationSymbolTables declaration_symbols_;
  ParserState parser_state_;
  SourceManager sources_;
  ExperimentalSyntaxModel experimental_syntax_;
  CompilationLimits limits_;
  std::atomic<bool> cancel_requested_{false};
};

}  // namespace matiec

#endif

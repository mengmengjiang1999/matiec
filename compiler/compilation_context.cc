#include "compilation_context.hh"

#include <utility>

namespace matiec {

CompilationContext::CompilationContext()
    : outputs_(diagnostics_), analysis_(ast_arena_) {}

CompilationContext::CompilationContext(CompilerOptions options)
    : options_(std::move(options)), outputs_(diagnostics_), analysis_(ast_arena_) {}

CompilerOptions &CompilationContext::options() {
  return options_;
}

const CompilerOptions &CompilationContext::options() const {
  return options_;
}

DiagnosticEngine &CompilationContext::diagnostics() {
  return diagnostics_;
}

const DiagnosticEngine &CompilationContext::diagnostics() const {
  return diagnostics_;
}

AstArena &CompilationContext::ast_arena() {
  return ast_arena_;
}

const AstArena &CompilationContext::ast_arena() const {
  return ast_arena_;
}

AnalysisStore &CompilationContext::analysis() { return analysis_; }

const AnalysisStore &CompilationContext::analysis() const { return analysis_; }

OutputManager &CompilationContext::outputs() {
  return outputs_;
}

const OutputManager &CompilationContext::outputs() const {
  return outputs_;
}

void CompilationContext::set_source_path(std::string source_path) {
  sources_.set_file(std::move(source_path));
}

const std::string &CompilationContext::source_path() const {
  return sources_.display_name();
}

void CompilationContext::set_source(std::string display_name,
                                    std::string source) {
  sources_.set_memory(std::move(display_name), std::move(source));
}

SourceManager &CompilationContext::sources() { return sources_; }

const SourceManager &CompilationContext::sources() const { return sources_; }

ExperimentalSyntaxModel &CompilationContext::experimental_syntax() {
  return experimental_syntax_;
}

const ExperimentalSyntaxModel &CompilationContext::experimental_syntax() const {
  return experimental_syntax_;
}

}  // namespace matiec

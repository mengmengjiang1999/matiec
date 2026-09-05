#include "compilation_context.hh"

#include <utility>

namespace matiec {

CompilationContext::CompilationContext() : outputs_(diagnostics_) {}

CompilationContext::CompilationContext(CompilerOptions options)
    : options_(std::move(options)), outputs_(diagnostics_) {}

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

OutputManager &CompilationContext::outputs() {
  return outputs_;
}

const OutputManager &CompilationContext::outputs() const {
  return outputs_;
}

void CompilationContext::set_source_path(std::string source_path) {
  source_path_ = std::move(source_path);
}

const std::string &CompilationContext::source_path() const {
  return source_path_;
}

}  // namespace matiec

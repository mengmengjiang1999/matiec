#ifndef MATIEC_COMPILER_COMPILER_TYPES_HH
#define MATIEC_COMPILER_COMPILER_TYPES_HH

#include <cstddef>
#include <string>

namespace matiec {

enum class OutputLanguage {
  c,
  iec
};

struct CompilerOptions {
  bool allow_void_datatype = false;
  bool allow_missing_var_in = false;
  bool disable_implicit_en_eno = false;
  bool pre_parsing = false;
  bool safe_extensions = false;
  bool full_token_location = false;
  bool conversion_functions = false;
  bool nested_comments = false;
  bool reference_extensions = false;
  bool nonstandard_reference_extensions = false;
  bool nonliteral_array_size = false;
  bool relaxed_datatype_model = false;
  bool syntax_only = false;
  std::string include_directory;
  std::string output_directory;
  std::string generator_options;
  OutputLanguage output_language = OutputLanguage::c;
};

struct SourceLocation {
  std::string file;
  std::size_t line = 0;
  std::size_t column = 0;
  std::size_t offset = 0;

  bool valid() const;
};

struct SourceRange {
  SourceLocation begin;
  SourceLocation end;

  bool valid() const;
};

enum class DiagnosticSeverity {
  note,
  warning,
  error,
  fatal
};

struct Diagnostic {
  DiagnosticSeverity severity = DiagnosticSeverity::error;
  std::string message;
  SourceRange range;
};

struct CompilationResult {
  std::size_t error_count = 0;
  std::size_t warning_count = 0;

  bool succeeded() const;
  static CompilationResult success(std::size_t warnings = 0);
  static CompilationResult failure(std::size_t errors = 1, std::size_t warnings = 0);
};

const char *diagnostic_severity_name(DiagnosticSeverity severity);

}  // namespace matiec

#endif

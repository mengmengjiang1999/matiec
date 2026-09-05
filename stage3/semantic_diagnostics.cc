#include "semantic_diagnostics.hh"

#include "../absyntax/absyntax.hh"

#include <cstdarg>
#include <cstdio>
#include <vector>

namespace matiec {
namespace {

symbol_c *first_symbol(symbol_c *left, symbol_c *right) {
  return left->first_order < right->first_order ? left : right;
}

symbol_c *last_symbol(symbol_c *left, symbol_c *right) {
  return left->last_order > right->last_order ? left : right;
}

SourceRange source_range(symbol_c *first, symbol_c *last) {
  symbol_c *begin = first_symbol(first, last);
  symbol_c *end = last_symbol(first, last);
  return {{begin->first_file == nullptr ? "" : begin->first_file,
           static_cast<std::size_t>(begin->first_line),
           static_cast<std::size_t>(begin->first_column), 0},
          {end->last_file == nullptr ? "" : end->last_file,
           static_cast<std::size_t>(end->last_line),
           static_cast<std::size_t>(end->last_column), 0}};
}

std::string format_message(const char *format, va_list arguments) {
  va_list size_arguments;
  va_copy(size_arguments, arguments);
  const int size = std::vsnprintf(nullptr, 0, format, size_arguments);
  va_end(size_arguments);
  if (size < 0) return "Unable to format semantic diagnostic";

  std::vector<char> buffer(static_cast<std::size_t>(size) + 1);
  std::vsnprintf(buffer.data(), buffer.size(), format, arguments);
  return std::string(buffer.data(), static_cast<std::size_t>(size));
}

}  // namespace

SemanticDiagnostics::SemanticDiagnostics(DiagnosticEngine &diagnostics)
    : diagnostics_(diagnostics) {}

void SemanticDiagnostics::error(symbol_c *first, symbol_c *last,
                                const char *format, ...) {
  va_list arguments;
  va_start(arguments, format);
  report(DiagnosticSeverity::error, first, last, format, arguments);
  va_end(arguments);
}

void SemanticDiagnostics::warning(symbol_c *first, symbol_c *last,
                                  const char *format, ...) {
  va_list arguments;
  va_start(arguments, format);
  report(DiagnosticSeverity::warning, first, last, format, arguments);
  va_end(arguments);
}

void SemanticDiagnostics::report(DiagnosticSeverity severity, symbol_c *first,
                                 symbol_c *last, const char *format,
                                 va_list arguments) {
  diagnostics_.report(severity, format_message(format, arguments),
                      source_range(first, last));
}

}  // namespace matiec

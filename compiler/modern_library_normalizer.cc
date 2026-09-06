#include "compiler/modern_library_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace matiec {
namespace {

struct Line {
  std::string text;
  std::size_t begin = 0;
  std::size_t end = 0;
  std::size_t number = 1;
  bool has_newline = false;
};

std::vector<Line> split_lines(std::string_view source) {
  std::vector<Line> lines;
  std::size_t begin = 0;
  std::size_t number = 1;
  while (begin < source.size()) {
    const std::size_t newline = source.find('\n', begin);
    const bool has_newline = newline != std::string_view::npos;
    const std::size_t content_end = has_newline ? newline : source.size();
    const std::size_t end = has_newline ? newline + 1 : source.size();
    lines.push_back({std::string(source.substr(begin, content_end - begin)),
                     begin, end, number++, has_newline});
    begin = end;
  }
  if (source.empty()) lines.push_back({"", 0, 0, 1, false});
  return lines;
}

SourceRange line_range(const Line &line, const std::string &path) {
  const SourceLocation begin{path, line.number, 1, line.begin};
  const SourceLocation end{path, line.number, line.text.size() + 1, line.end};
  return {begin, end};
}

std::string visible_code(const std::string &line, bool *in_comment) {
  std::string output(line.size(), ' ');
  char quote = 0;
  for (std::size_t index = 0; index < line.size();) {
    if (*in_comment) {
      if (index + 1 < line.size() && line[index] == '*' &&
          line[index + 1] == ')') {
        *in_comment = false;
        index += 2;
      } else {
        ++index;
      }
      continue;
    }
    if (quote != 0) {
      if (line[index] == '$' && index + 1 < line.size()) {
        index += 2;
      } else if (line[index++] == quote) {
        quote = 0;
      }
      continue;
    }
    if (index + 1 < line.size() && line[index] == '(' &&
        line[index + 1] == '*') {
      *in_comment = true;
      index += 2;
      continue;
    }
    if (line[index] == '\'' || line[index] == '"') {
      quote = line[index++];
      continue;
    }
    output[index] = line[index];
    ++index;
  }
  return output;
}

}  // namespace

bool normalize_experimental_modern_library(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, ModernLibraryNormalizeResult *result) {
  if (result == nullptr) return false;
  result->source = std::string(source);
  result->functions.clear();
  result->used_modern_library = false;

  const std::regex assert_call(
      "^([ \\t]*)ASSERT[ \\t]*\\((.*)\\)[ \\t]*;[ \\t]*$",
      std::regex::icase);
  const std::regex assert_prefix("^[ \\t]*ASSERT\\b.*$", std::regex::icase);
  const std::regex assert_declaration(
      "^[ \\t]*FUNCTION[ \\t]+ASSERT\\b.*$", std::regex::icase);

  const std::vector<Line> lines = split_lines(source);
  std::ostringstream normalized;
  bool in_comment = false;
  bool declared_by_user = false;
  for (const Line &line : lines) {
    const std::string visible = visible_code(line.text, &in_comment);
    std::smatch match;
    if (std::regex_match(visible, match, assert_declaration)) {
      declared_by_user = true;
      break;
    }
  }
  if (declared_by_user) return true;

  in_comment = false;
  for (const Line &line : lines) {
    const std::string visible = visible_code(line.text, &in_comment);
    std::smatch match;
    if (std::regex_match(visible, match, assert_call)) {
      std::smatch source_match;
      if (!std::regex_match(line.text, source_match, assert_call)) {
        diagnostics.error(
            "Experimental ASSERT does not support an inline block comment",
            line_range(line, source_path));
        if (line.has_newline) normalized << '\n';
        continue;
      }
      if (source_match[2].str().find_first_not_of(" \t") ==
          std::string::npos) {
        diagnostics.error("ASSERT requires a BOOL input expression",
                          line_range(line, source_path));
      } else {
        if (!result->used_modern_library)
          result->functions.push_back(
              {"ASSERT", "ASSERT(IN : BOOL) : VOID", "ASSERT",
               line_range(line, source_path)});
        result->used_modern_library = true;
        normalized << line.text;
      }
    } else if (std::regex_match(visible, match, assert_prefix)) {
      diagnostics.error(
          "Experimental ASSERT must be a standalone call on one source line",
          line_range(line, source_path));
      normalized << line.text;
    } else {
      normalized << line.text;
    }
    if (line.has_newline) normalized << '\n';
  }
  if (diagnostics.has_errors()) return false;
  if (!result->used_modern_library) return true;

  const std::string declaration =
      "FUNCTION ASSERT : VOID\n"
      "  VAR_INPUT\n"
      "    IN : BOOL;\n"
      "  END_VAR\n"
      "  RETURN;\n"
      "END_FUNCTION\n\n";
  result->source = declaration + normalized.str();
  return true;
}

}  // namespace matiec

#include "compiler/modern_library_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <cctype>
#include <string>
#include <utility>
#include <vector>

namespace matiec {
namespace {

struct Token {
  std::string text;
  std::size_t begin = 0;
  std::size_t end = 0;
  std::size_t line = 1;
  std::size_t column = 1;
};

bool identifier_start(unsigned char value) {
  return std::isalpha(value) || value == '_';
}

bool identifier_continue(unsigned char value) {
  return std::isalnum(value) || value == '_';
}

void advance(char value, std::size_t *line, std::size_t *column) {
  if (value == '\n') {
    ++*line;
    *column = 1;
  } else {
    ++*column;
  }
}

std::string uppercase(std::string value) {
  for (char &character : value)
    character = static_cast<char>(
        std::toupper(static_cast<unsigned char>(character)));
  return value;
}

std::vector<Token> scan_tokens(std::string_view source) {
  std::vector<Token> tokens;
  std::size_t offset = 0;
  std::size_t line = 1;
  std::size_t column = 1;
  while (offset < source.size()) {
    const unsigned char value = static_cast<unsigned char>(source[offset]);
    if (std::isspace(value)) {
      advance(source[offset++], &line, &column);
      continue;
    }
    if (offset + 1 < source.size() && source[offset] == '(' &&
        source[offset + 1] == '*') {
      std::size_t depth = 1;
      advance(source[offset++], &line, &column);
      advance(source[offset++], &line, &column);
      while (offset < source.size() && depth > 0) {
        if (offset + 1 < source.size() && source[offset] == '(' &&
            source[offset + 1] == '*') {
          ++depth;
          advance(source[offset++], &line, &column);
          advance(source[offset++], &line, &column);
        } else if (offset + 1 < source.size() && source[offset] == '*' &&
                   source[offset + 1] == ')') {
          --depth;
          advance(source[offset++], &line, &column);
          advance(source[offset++], &line, &column);
        } else {
          advance(source[offset++], &line, &column);
        }
      }
      continue;
    }
    if (source[offset] == '\'' || source[offset] == '"') {
      const char quote = source[offset];
      advance(source[offset++], &line, &column);
      while (offset < source.size()) {
        if (source[offset] == '$' && offset + 1 < source.size()) {
          advance(source[offset++], &line, &column);
          advance(source[offset++], &line, &column);
        } else {
          const char character = source[offset++];
          advance(character, &line, &column);
          if (character == quote) break;
        }
      }
      continue;
    }

    Token token;
    token.begin = offset;
    token.line = line;
    token.column = column;
    if (identifier_start(value)) {
      while (offset < source.size() &&
             identifier_continue(static_cast<unsigned char>(source[offset]))) {
        token.text.push_back(source[offset]);
        advance(source[offset++], &line, &column);
      }
      token.text = uppercase(token.text);
    } else {
      token.text.assign(1, source[offset]);
      advance(source[offset++], &line, &column);
    }
    token.end = offset;
    tokens.push_back(std::move(token));
  }
  return tokens;
}

SourceRange token_range(const Token &token, const std::string &path) {
  return {{path, token.line, token.column, token.begin},
          {path, token.line, token.column + token.end - token.begin, token.end}};
}

}  // namespace

bool normalize_experimental_modern_library(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, ModernLibraryNormalizeResult *result) {
  if (result == nullptr) return false;
  (void)diagnostics;
  result->source = std::string(source);
  result->functions.clear();
  result->used_modern_library = false;

  const std::vector<Token> tokens = scan_tokens(source);
  for (std::size_t index = 0; index + 1 < tokens.size(); ++index) {
    if (tokens[index].text == "FUNCTION" &&
        tokens[index + 1].text == "ASSERT")
      return true;
  }

  const Token *assertion = nullptr;
  for (std::size_t index = 0; index + 1 < tokens.size(); ++index) {
    if (tokens[index].text != "ASSERT" || tokens[index + 1].text != "(")
      continue;
    if (index > 0 && tokens[index - 1].text == ".") continue;
    assertion = &tokens[index];
    break;
  }
  if (assertion == nullptr) return true;

  result->used_modern_library = true;
  result->functions.push_back(
      {"ASSERT", "ASSERT(IN : BOOL) : VOID", "ASSERT",
       token_range(*assertion, source_path)});
  const std::string declaration =
      "FUNCTION ASSERT : VOID\n"
      "  VAR_INPUT\n"
      "    IN : BOOL;\n"
      "  END_VAR\n"
      "  RETURN;\n"
      "END_FUNCTION\n\n";
  result->source = declaration + result->source;
  return true;
}

}  // namespace matiec

#include "compiler/namespace_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <utility>

namespace matiec {
namespace {

enum class TokenKind { identifier, dot, semicolon, colon, other };

struct Token {
  TokenKind kind = TokenKind::other;
  std::string text;
  std::string canonical;
  std::size_t begin = 0;
  std::size_t end = 0;
  std::size_t line = 1;
  std::size_t column = 1;
};

struct Symbol {
  std::string qualified;
  std::string owner;
  std::string lowered;
  NamespaceVisibility visibility = NamespaceVisibility::public_;
};

struct Replacement {
  std::size_t begin = 0;
  std::size_t end = 0;
  std::string text;
};

std::string uppercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
  return value;
}

bool identifier_start(unsigned char ch) {
  return std::isalpha(ch) != 0 || ch == '_';
}

bool identifier_continue(unsigned char ch) {
  return std::isalnum(ch) != 0 || ch == '_';
}

SourceRange token_range(const Token &token, const std::string &path) {
  const SourceLocation begin{path, token.line, token.column, token.begin};
  const SourceLocation end{path, token.line,
                           token.column + token.end - token.begin, token.end};
  return {begin, end};
}

std::vector<Token> tokenize(std::string_view source) {
  std::vector<Token> tokens;
  std::size_t line = 1;
  std::size_t column = 1;
  for (std::size_t pos = 0; pos < source.size();) {
    const std::size_t begin = pos;
    const std::size_t begin_line = line;
    const std::size_t begin_column = column;
    const unsigned char ch = static_cast<unsigned char>(source[pos]);
    if (ch == '\n') { ++pos; ++line; column = 1; continue; }
    if (std::isspace(ch) != 0) { ++pos; ++column; continue; }
    if (pos + 1 < source.size() && source[pos] == '(' && source[pos + 1] == '*') {
      pos += 2; column += 2;
      int depth = 1;
      while (pos < source.size() && depth != 0) {
        if (pos + 1 < source.size() && source[pos] == '(' && source[pos + 1] == '*') {
          ++depth; pos += 2; column += 2;
        } else if (pos + 1 < source.size() && source[pos] == '*' && source[pos + 1] == ')') {
          --depth; pos += 2; column += 2;
        } else if (source[pos] == '\n') {
          ++pos; ++line; column = 1;
        } else {
          ++pos; ++column;
        }
      }
      continue;
    }
    if (ch == '\'' || ch == '"') {
      const char quote = static_cast<char>(ch);
      ++pos; ++column;
      while (pos < source.size()) {
        if (source[pos] == '$' && pos + 1 < source.size()) {
          pos += 2; column += 2; continue;
        }
        if (source[pos] == quote) { ++pos; ++column; break; }
        if (source[pos] == '\n') { ++pos; ++line; column = 1; }
        else { ++pos; ++column; }
      }
      continue;
    }
    Token token;
    token.begin = begin;
    token.line = begin_line;
    token.column = begin_column;
    if (identifier_start(ch)) {
      ++pos; ++column;
      while (pos < source.size() &&
             identifier_continue(static_cast<unsigned char>(source[pos]))) {
        ++pos; ++column;
      }
      token.kind = TokenKind::identifier;
    } else {
      ++pos; ++column;
      token.kind = ch == '.' ? TokenKind::dot
                 : ch == ';' ? TokenKind::semicolon
                 : ch == ':' ? TokenKind::colon : TokenKind::other;
    }
    token.end = pos;
    token.text = std::string(source.substr(begin, pos - begin));
    token.canonical = uppercase(token.text);
    tokens.push_back(std::move(token));
  }
  return tokens;
}

std::size_t line_end(std::string_view source, std::size_t position) {
  const std::size_t found = source.find('\n', position);
  return found == std::string_view::npos ? source.size() : found;
}

bool parse_qualified(const std::vector<Token> &tokens, std::size_t begin,
                     std::size_t *after, std::string *canonical,
                     std::string *display) {
  if (begin >= tokens.size() || tokens[begin].kind != TokenKind::identifier)
    return false;
  std::size_t cursor = begin;
  *canonical = tokens[cursor].canonical;
  *display = tokens[cursor].text;
  ++cursor;
  while (cursor + 1 < tokens.size() && tokens[cursor].kind == TokenKind::dot &&
         tokens[cursor + 1].kind == TokenKind::identifier) {
    *canonical += "." + tokens[cursor + 1].canonical;
    *display += "." + tokens[cursor + 1].text;
    cursor += 2;
  }
  *after = cursor;
  return true;
}

std::string lower_name(const std::string &qualified) {
  std::string value = "MATIECNS";
  std::size_t begin = 0;
  while (begin < qualified.size()) {
    const std::size_t end = qualified.find('.', begin);
    const std::size_t length = (end == std::string::npos ? qualified.size() : end) - begin;
    value += std::to_string(length);
    value.append(qualified, begin, length);
    if (end == std::string::npos) break;
    begin = end + 1;
  }
  return value;
}

bool same_tree(const std::string &requester, const std::string &owner) {
  return requester == owner ||
      (requester.size() > owner.size() &&
       requester.compare(0, owner.size(), owner) == 0 &&
       requester[owner.size()] == '.');
}

void blank_line(std::string_view source, const Token &token,
                std::vector<Replacement> *replacements) {
  replacements->push_back({token.begin, line_end(source, token.end), ""});
}

}  // namespace

bool normalize_experimental_namespaces(std::string_view source,
                                       const std::string &source_path,
                                       DiagnosticEngine &diagnostics,
                                       NamespaceNormalizeResult *result) {
  if (result == nullptr) return false;
  result->source = std::string(source);
  result->declarations.clear();
  result->used_namespaces = false;

  const std::vector<Token> tokens = tokenize(source);
  std::vector<std::string> token_scope(tokens.size());
  std::map<std::string, NamespaceVisibility> namespaces;
  std::map<std::string, std::vector<std::string> > imports;
  std::map<std::string, Symbol> symbols;
  std::set<std::size_t> declaration_tokens;
  std::set<std::size_t> syntax_tokens;
  std::vector<Replacement> replacements;
  std::string scope;
  int type_depth = 0;
  int struct_depth = 0;

  for (std::size_t index = 0; index < tokens.size(); ++index) {
    token_scope[index] = scope;
    if (tokens[index].kind != TokenKind::identifier) continue;
    const std::string &word = tokens[index].canonical;
    if (word == "NAMESPACE") {
      result->used_namespaces = true;
      if (!scope.empty()) {
        diagnostics.error("Nested NAMESPACE blocks are not supported by the MATIEC experimental profile",
                          token_range(tokens[index], source_path));
        continue;
      }
      const std::size_t statement_index = index;
      std::size_t cursor = index + 1;
      NamespaceVisibility visibility = NamespaceVisibility::public_;
      if (cursor < tokens.size() && tokens[cursor].canonical == "INTERNAL") {
        visibility = NamespaceVisibility::internal;
        syntax_tokens.insert(cursor++);
      }
      std::string canonical, display;
      std::size_t after = cursor;
      if (!parse_qualified(tokens, cursor, &after, &canonical, &display)) {
        diagnostics.error("NAMESPACE requires a qualified identifier",
                          token_range(tokens[index], source_path));
        continue;
      }
      if (namespaces.count(canonical) != 0) {
        diagnostics.error("Namespace reopening is not supported: " + display,
                          token_range(tokens[cursor], source_path));
      } else {
        namespaces[canonical] = visibility;
        NamespaceDeclarationAst declaration;
        declaration.name = display;
        declaration.visibility = visibility;
        declaration.range = token_range(tokens[cursor], source_path);
        result->declarations.push_back(std::move(declaration));
      }
      for (std::size_t item = statement_index; item < after; ++item)
        syntax_tokens.insert(item);
      blank_line(source, tokens[statement_index], &replacements);
      scope = canonical;
      index = after - 1;
      continue;
    }
    if (word == "END_NAMESPACE") {
      result->used_namespaces = true;
      syntax_tokens.insert(index);
      blank_line(source, tokens[index], &replacements);
      if (scope.empty()) {
        diagnostics.error("END_NAMESPACE has no matching NAMESPACE",
                          token_range(tokens[index], source_path));
      }
      scope.clear();
      continue;
    }
    if (word == "USING") {
      result->used_namespaces = true;
      const std::size_t statement_index = index;
      syntax_tokens.insert(index);
      std::size_t cursor = index + 1;
      std::string canonical, display;
      std::size_t after = cursor;
      if (!parse_qualified(tokens, cursor, &after, &canonical, &display)) {
        diagnostics.error("USING requires a qualified namespace name",
                          token_range(tokens[index], source_path));
      } else {
        imports[scope].push_back(canonical);
        if (!scope.empty()) {
          for (NamespaceDeclarationAst &declaration : result->declarations) {
            if (uppercase(declaration.name) == scope) {
              declaration.imports.push_back(
                  {display, token_range(tokens[cursor], source_path)});
            }
          }
        }
        for (std::size_t item = cursor; item < after; ++item)
          syntax_tokens.insert(item);
        index = after - 1;
      }
      blank_line(source, tokens[statement_index], &replacements);
      continue;
    }
    if (scope.empty()) continue;
    if (word == "TYPE") { ++type_depth; continue; }
    if (word == "END_TYPE") { if (type_depth > 0) --type_depth; continue; }
    if (word == "STRUCT") { if (type_depth > 0) ++struct_depth; continue; }
    if (word == "END_STRUCT") { if (struct_depth > 0) --struct_depth; continue; }

    bool declaration = false;
    if ((word == "FUNCTION" || word == "FUNCTION_BLOCK" || word == "PROGRAM" ||
         word == "CONFIGURATION") && index + 1 < tokens.size() &&
        tokens[index + 1].kind == TokenKind::identifier) {
      ++index;
      token_scope[index] = scope;
      declaration = true;
    } else if (type_depth > 0 && struct_depth == 0 && index + 1 < tokens.size() &&
               tokens[index + 1].kind == TokenKind::colon) {
      declaration = true;
    }
    if (!declaration) continue;

    const Token &name = tokens[index];
    const std::string qualified = scope + "." + name.canonical;
    if (symbols.count(qualified) != 0) {
      diagnostics.error("Duplicate namespace declaration: " + qualified,
                        token_range(name, source_path));
      continue;
    }
    Symbol symbol;
    symbol.qualified = qualified;
    symbol.owner = scope;
    symbol.lowered = lower_name(qualified);
    symbol.visibility = namespaces[scope];
    symbols[qualified] = symbol;
    declaration_tokens.insert(index);
  }

  if (!scope.empty()) diagnostics.error("NAMESPACE block is missing END_NAMESPACE");
  for (const auto &entry : imports) {
    for (const std::string &name : entry.second) {
      if (namespaces.count(name) == 0)
        diagnostics.error("USING refers to an unknown namespace: " + name);
    }
  }

  for (std::size_t index : declaration_tokens) {
    const std::string name = token_scope[index] + "." + tokens[index].canonical;
    replacements.push_back(
        {tokens[index].begin, tokens[index].end, symbols[name].lowered});
  }

  for (std::size_t index = 0; index < tokens.size(); ++index) {
    if (tokens[index].kind != TokenKind::identifier ||
        declaration_tokens.count(index) != 0 || syntax_tokens.count(index) != 0)
      continue;
    std::size_t after = index;
    std::string qualified, display;
    parse_qualified(tokens, index, &after, &qualified, &display);
    const auto direct = symbols.find(qualified);
    if (after > index + 1) {
      bool known_prefix = false;
      for (const auto &ns : namespaces) {
        if (qualified.size() > ns.first.size() &&
            qualified.compare(0, ns.first.size(), ns.first) == 0 &&
            qualified[ns.first.size()] == '.') {
          known_prefix = true;
          break;
        }
      }
      if (direct != symbols.end()) {
        if (direct->second.visibility == NamespaceVisibility::internal &&
            !same_tree(token_scope[index], direct->second.owner)) {
          diagnostics.error("Namespace declaration is inaccessible: " + display,
                            token_range(tokens[index], source_path));
        } else {
          replacements.push_back(
              {tokens[index].begin, tokens[after - 1].end, direct->second.lowered});
        }
        index = after - 1;
        continue;
      }
      if (known_prefix) {
        diagnostics.error("Unknown qualified namespace declaration: " + display,
                          token_range(tokens[index], source_path));
        index = after - 1;
        continue;
      }
    }

    std::vector<const Symbol *> candidates;
    if (!token_scope[index].empty()) {
      const auto current = symbols.find(token_scope[index] + "." + tokens[index].canonical);
      if (current != symbols.end()) candidates.push_back(&current->second);
    }
    if (candidates.empty()) {
      std::vector<std::string> visible_imports = imports[""];
      const auto local = imports.find(token_scope[index]);
      if (local != imports.end())
        visible_imports.insert(visible_imports.end(), local->second.begin(),
                               local->second.end());
      for (const std::string &imported : visible_imports) {
        const auto found = symbols.find(imported + "." + tokens[index].canonical);
        if (found != symbols.end()) candidates.push_back(&found->second);
      }
    }
    if (candidates.size() > 1) {
      diagnostics.error("Ambiguous namespace lookup for '" + tokens[index].text + "'",
                        token_range(tokens[index], source_path));
    } else if (candidates.size() == 1) {
      const Symbol &found = *candidates.front();
      if (found.visibility == NamespaceVisibility::internal &&
          !same_tree(token_scope[index], found.owner)) {
        diagnostics.error("Namespace declaration is inaccessible: " + found.qualified,
                          token_range(tokens[index], source_path));
      } else {
        replacements.push_back(
            {tokens[index].begin, tokens[index].end, found.lowered});
      }
    }
  }

  if (diagnostics.has_errors()) return false;
  std::sort(replacements.begin(), replacements.end(),
            [](const Replacement &left, const Replacement &right) {
              return left.begin > right.begin;
            });
  for (const Replacement &replacement : replacements) {
    std::string text = replacement.text;
    if (text.empty()) text.assign(replacement.end - replacement.begin, ' ');
    result->source.replace(replacement.begin, replacement.end - replacement.begin,
                           text);
  }
  return true;
}

bool normalize_experimental_namespace_file(
    const std::string &source_path, DiagnosticEngine &diagnostics,
    NamespaceNormalizeResult *result) {
  std::ifstream input(source_path, std::ios::binary);
  if (!input) {
    diagnostics.error("Cannot open source file for namespace analysis: " + source_path);
    return false;
  }
  std::ostringstream contents;
  contents << input.rdbuf();
  return normalize_experimental_namespaces(contents.str(), source_path, diagnostics,
                                            result);
}

}  // namespace matiec

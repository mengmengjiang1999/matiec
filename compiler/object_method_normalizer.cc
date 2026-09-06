#include "compiler/object_method_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <utility>

namespace matiec {
namespace {

struct Line {
  std::string text;
  std::size_t begin = 0;
  std::size_t end = 0;
  std::size_t number = 1;
  bool has_newline = false;
};

struct MethodBlock {
  ObjectMethodAst ast;
  std::size_t begin_line = 0;
  std::size_t end_line = 0;
  std::set<std::string> owner_fields;
  std::set<std::string> locals;
};

std::string uppercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
  return value;
}

std::string trim(std::string value) {
  const std::size_t begin = value.find_first_not_of(" \t\r");
  if (begin == std::string::npos) return "";
  const std::size_t end = value.find_last_not_of(" \t\r");
  return value.substr(begin, end - begin + 1);
}

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

std::string lowered_method_name(const std::string &owner,
                                const std::string &method) {
  const std::string owner_key = uppercase(owner);
  const std::string method_key = uppercase(method);
  return "MATIECMETHOD" + std::to_string(owner_key.size()) + owner_key +
         std::to_string(method_key.size()) + method_key;
}

bool match_line(const std::string &line, const std::regex &pattern,
                std::smatch *match) {
  return std::regex_match(line, *match, pattern);
}

std::string rewrite_identifiers(const std::string &text,
                                const std::map<std::string, std::string> &names) {
  std::string output;
  bool in_comment = false;
  char quote = 0;
  for (std::size_t pos = 0; pos < text.size();) {
    if (in_comment) {
      if (pos + 1 < text.size() && text[pos] == '*' && text[pos + 1] == ')') {
        output += "*)"; pos += 2; in_comment = false;
      } else {
        output += text[pos++];
      }
      continue;
    }
    if (quote != 0) {
      output += text[pos];
      if (text[pos] == '$' && pos + 1 < text.size()) output += text[++pos];
      else if (text[pos] == quote) quote = 0;
      ++pos;
      continue;
    }
    if (pos + 1 < text.size() && text[pos] == '(' && text[pos + 1] == '*') {
      output += "(*"; pos += 2; in_comment = true; continue;
    }
    if (text[pos] == '\'' || text[pos] == '"') {
      quote = text[pos]; output += text[pos++]; continue;
    }
    const unsigned char ch = static_cast<unsigned char>(text[pos]);
    if (std::isalpha(ch) != 0 || ch == '_') {
      const std::size_t begin = pos++;
      while (pos < text.size()) {
        const unsigned char next = static_cast<unsigned char>(text[pos]);
        if (std::isalnum(next) == 0 && next != '_') break;
        ++pos;
      }
      const std::string word = text.substr(begin, pos - begin);
      const auto replacement = names.find(uppercase(word));
      output += replacement == names.end() ? word : replacement->second;
    } else {
      output += text[pos++];
    }
  }
  return output;
}

}  // namespace

bool normalize_experimental_object_methods(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, ObjectMethodNormalizeResult *result) {
  if (result == nullptr) return false;
  result->source = std::string(source);
  result->methods.clear();
  result->used_methods = false;

  const std::regex fb_start(
      "^[ \\t]*FUNCTION_BLOCK[ \\t]+([A-Za-z_][A-Za-z0-9_]*).*$",
      std::regex::icase);
  const std::regex fb_end("^[ \\t]*END_FUNCTION_BLOCK[ \\t]*;?[ \\t\\r\\n]*$",
                          std::regex::icase);
  const std::regex method_start(
      "^[ \\t]*METHOD[ \\t]+(?:(PUBLIC|PRIVATE|PROTECTED|INTERNAL)[ \\t]+)?"
      "([A-Za-z_][A-Za-z0-9_]*)[ \\t]*:[ \\t]*(.+?)[ \\t\\r\\n]*$",
      std::regex::icase);
  const std::regex method_end("^[ \\t]*END_METHOD[ \\t]*;?[ \\t\\r\\n]*$",
                              std::regex::icase);
  const std::regex var_start("^[ \\t]*VAR(?:_[A-Z_]+)?(?:[ \\t].*)?$",
                             std::regex::icase);
  const std::regex var_end("^[ \\t]*END_VAR[ \\t]*;?[ \\t\\r\\n]*$",
                           std::regex::icase);
  const std::regex variable(
      "^[ \\t]*([A-Za-z_][A-Za-z0-9_]*)[ \\t]*:[ \\t]*"
      "([A-Za-z_][A-Za-z0-9_]*).*$",
      std::regex::icase);
  const std::regex unsupported(
      ".*\\b(CLASS|INTERFACE|EXTENDS|IMPLEMENTS|OVERRIDE|ABSTRACT|FINAL|THIS|SUPER)\\b.*",
      std::regex::icase);

  const std::vector<Line> lines = split_lines(source);
  std::vector<MethodBlock> blocks;
  std::map<std::string, std::string> instance_types;
  std::string owner;
  std::set<std::string> owner_fields;
  bool in_owner_vars = false;

  for (std::size_t index = 0; index < lines.size(); ++index) {
    std::smatch match;
    if (match_line(lines[index].text, unsupported, &match)) {
      diagnostics.error("Unsupported object-oriented construct in the MATIEC experimental method subset: " +
                            trim(match[1].str()),
                        line_range(lines[index], source_path));
    }
    if (owner.empty() && match_line(lines[index].text, fb_start, &match)) {
      owner = match[1].str();
      owner_fields.clear();
      in_owner_vars = false;
      continue;
    }
    if (owner.empty()) {
      if (match_line(lines[index].text, variable, &match))
        instance_types[uppercase(match[1].str())] = uppercase(match[2].str());
      continue;
    }
    if (match_line(lines[index].text, fb_end, &match)) {
      owner.clear(); owner_fields.clear(); in_owner_vars = false; continue;
    }
    if (match_line(lines[index].text, var_start, &match)) {
      in_owner_vars = true; continue;
    }
    if (match_line(lines[index].text, var_end, &match)) {
      in_owner_vars = false; continue;
    }
    if (in_owner_vars && match_line(lines[index].text, variable, &match)) {
      owner_fields.insert(uppercase(match[1].str()));
      continue;
    }
    if (!match_line(lines[index].text, method_start, &match)) continue;

    result->used_methods = true;
    const std::string access = uppercase(match[1].str());
    if (!access.empty() && access != "PUBLIC") {
      diagnostics.error("Only PUBLIC methods are supported by the MATIEC experimental method subset",
                        line_range(lines[index], source_path));
    }
    MethodBlock block;
    block.ast.owner = owner;
    block.ast.name = match[2].str();
    block.ast.return_type = trim(match[3].str());
    block.ast.lowered_name = lowered_method_name(owner, block.ast.name);
    block.ast.range = line_range(lines[index], source_path);
    block.begin_line = index;
    block.owner_fields = owner_fields;

    bool in_method_vars = false;
    std::size_t cursor = index + 1;
    for (; cursor < lines.size(); ++cursor) {
      std::smatch inner;
      if (match_line(lines[cursor].text, method_end, &inner)) break;
      if (match_line(lines[cursor].text, var_start, &inner)) {
        in_method_vars = true; continue;
      }
      if (match_line(lines[cursor].text, var_end, &inner)) {
        in_method_vars = false; continue;
      }
      if (in_method_vars && match_line(lines[cursor].text, variable, &inner))
        block.locals.insert(uppercase(inner[1].str()));
    }
    if (cursor == lines.size()) {
      diagnostics.error("METHOD block is missing END_METHOD",
                        line_range(lines[index], source_path));
      break;
    }
    block.end_line = cursor;
    blocks.push_back(std::move(block));
    index = cursor;
  }

  std::map<std::string, const MethodBlock *> method_by_owner_and_name;
  for (const MethodBlock &block : blocks) {
    const std::string key = uppercase(block.ast.owner) + "." + uppercase(block.ast.name);
    if (method_by_owner_and_name.count(key) != 0) {
      diagnostics.error("Duplicate method declaration: " + key, block.ast.range);
    } else {
      method_by_owner_and_name[key] = &block;
      result->methods.push_back(block.ast);
    }
  }
  if (diagnostics.has_errors()) return false;
  if (blocks.empty()) return true;

  std::vector<bool> removed(lines.size(), false);
  std::ostringstream generated;
  for (const MethodBlock &block : blocks) {
    for (std::size_t line = block.begin_line; line <= block.end_line; ++line)
      removed[line] = true;
    generated << "\nFUNCTION " << block.ast.lowered_name << " : "
              << block.ast.return_type << "\n";
    std::string interface_declarations;
    std::string body;
    std::size_t line = block.begin_line + 1;
    while (line < block.end_line) {
      const std::string heading = uppercase(trim(lines[line].text));
      if (heading.empty()) {
        interface_declarations += lines[line].text +
            (lines[line].has_newline ? "\n" : "");
        ++line;
        continue;
      }
      if (heading != "VAR_INPUT" && heading != "VAR_OUTPUT" &&
          heading != "VAR_IN_OUT")
        break;
      for (; line < block.end_line; ++line) {
        interface_declarations += lines[line].text +
            (lines[line].has_newline ? "\n" : "");
        std::smatch declaration_match;
        if (match_line(lines[line].text, var_end, &declaration_match)) {
          ++line;
          break;
        }
      }
    }
    for (; line < block.end_line; ++line)
      body += lines[line].text + (lines[line].has_newline ? "\n" : "");
    std::map<std::string, std::string> names;
    names[uppercase(block.ast.name)] = block.ast.lowered_name;
    for (const std::string &field : block.owner_fields) {
      if (block.locals.count(field) == 0) names[field] = "MATIECSELF." + field;
    }
    generated << rewrite_identifiers(interface_declarations, names)
              << "  VAR_IN_OUT\n    MATIECSELF : " << block.ast.owner
              << ";\n  END_VAR\n"
              << rewrite_identifiers(body, names) << "END_FUNCTION\n";
  }

  std::string base;
  for (std::size_t index = 0; index < lines.size(); ++index) {
    if (removed[index]) {
      if (lines[index].has_newline) base += '\n';
    } else {
      base += lines[index].text;
      if (lines[index].has_newline) base += '\n';
    }
  }

  for (const auto &entry : instance_types) {
    const std::string instance = entry.first;
    const std::string owner_type = entry.second;
    for (const auto &method_entry : method_by_owner_and_name) {
      const MethodBlock &block = *method_entry.second;
      if (uppercase(block.ast.owner) != owner_type) continue;
      const std::regex zero_call("\\b" + instance + "[ \\t]*\\.[ \\t]*" +
                                     uppercase(block.ast.name) +
                                     "[ \\t]*\\([ \\t]*\\)",
                                 std::regex::icase);
      base = std::regex_replace(base, zero_call,
                                block.ast.lowered_name + "(" + instance + ")");
      const std::regex call("\\b" + instance + "[ \\t]*\\.[ \\t]*" +
                                uppercase(block.ast.name) +
                                "[ \\t]*\\(([^()]*)\\)",
                            std::regex::icase);
      base = std::regex_replace(base, call,
                                block.ast.lowered_name + "($1, " + instance + ")");
    }
  }

  result->source = base + generated.str();
  return true;
}

}  // namespace matiec

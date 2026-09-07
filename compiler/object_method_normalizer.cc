#include "compiler/object_method_normalizer.hh"

#include "compiler/diagnostic_engine.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <regex>
#include <set>
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
  std::map<std::string, std::string> owner_fields;
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

}  // namespace

bool normalize_experimental_object_methods(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, ObjectMethodNormalizeResult *result) {
  if (result == nullptr) return false;
  result->source = std::string(source);
  result->methods.clear();
  result->instance_types.clear();
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
  std::map<std::string, std::string> owner_fields;
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
      owner_fields[uppercase(match[1].str())] = match[2].str();
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
    for (const auto &field : owner_fields) {
      if (block.locals.count(field.first) == 0)
        block.ast.owner_fields.push_back(field);
    }
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
  result->instance_types = instance_types;
  return true;
}

}  // namespace matiec

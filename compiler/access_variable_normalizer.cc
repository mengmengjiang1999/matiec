#include "compiler/access_variable_normalizer.hh"

#include "compiler/diagnostic_engine.hh"
#include "compiler/output_manager.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <utility>
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

struct GlobalVariable {
  std::string type;
  bool constant = false;
};

std::string uppercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
  return value;
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

bool matches(const std::string &line, const std::regex &pattern,
             std::smatch *match) {
  return std::regex_match(line, *match, pattern);
}

}  // namespace

bool normalize_experimental_access_variables(
    std::string_view source, const std::string &source_path,
    DiagnosticEngine &diagnostics, AccessVariableNormalizeResult *result) {
  if (result == nullptr) return false;
  result->source = std::string(source);
  result->declarations.clear();
  result->used_access_variables = false;

  const std::regex configuration_start(
      "^[ \\t]*CONFIGURATION[ \\t]+([A-Za-z_][A-Za-z0-9_]*).*$",
      std::regex::icase);
  const std::regex configuration_end(
      "^[ \\t]*END_CONFIGURATION[ \\t]*;?[ \\t\\r]*$", std::regex::icase);
  const std::regex resource_start(
      "^[ \\t]*RESOURCE[ \\t]+[A-Za-z_][A-Za-z0-9_]*[ \\t]+ON\\b.*$",
      std::regex::icase);
  const std::regex resource_end(
      "^[ \\t]*END_RESOURCE[ \\t]*;?[ \\t\\r]*$", std::regex::icase);
  const std::regex global_start(
      "^[ \\t]*VAR_GLOBAL(?:[ \\t]+(CONSTANT))?[ \\t\\r]*$",
      std::regex::icase);
  const std::regex access_start("^[ \\t]*VAR_ACCESS[ \\t\\r]*$",
                                std::regex::icase);
  const std::regex var_end("^[ \\t]*END_VAR[ \\t]*;?[ \\t\\r]*$",
                           std::regex::icase);
  const std::regex global_declaration(
      "^[ \\t]*([A-Za-z_][A-Za-z0-9_]*)[ \\t]*:[ \\t]*"
      "([A-Za-z_][A-Za-z0-9_]*)(?:[ \\t]*:=.*)?;[ \\t\\r]*$",
      std::regex::icase);
  const std::regex access_declaration(
      "^[ \\t]*([A-Za-z_][A-Za-z0-9_]*)[ \\t]*:[ \\t]*"
      "([A-Za-z_][A-Za-z0-9_]*)[ \\t]*:[ \\t]*"
      "([A-Za-z_][A-Za-z0-9_]*)(?:[ \\t]+(READ_ONLY|READ_WRITE))?"
      "[ \\t]*;[ \\t\\r]*$",
      std::regex::icase);

  const std::vector<Line> lines = split_lines(source);
  std::vector<bool> removed(lines.size(), false);
  std::map<std::string, GlobalVariable> globals;
  std::set<std::string> access_names;
  std::string configuration;
  bool in_resource = false;
  bool in_globals = false;
  bool globals_constant = false;

  for (std::size_t index = 0; index < lines.size(); ++index) {
    std::smatch match;
    if (configuration.empty() &&
        matches(lines[index].text, configuration_start, &match)) {
      configuration = match[1].str();
      globals.clear();
      access_names.clear();
      continue;
    }
    if (configuration.empty()) {
      if (matches(lines[index].text, access_start, &match))
        diagnostics.error("VAR_ACCESS is supported only inside CONFIGURATION",
                          line_range(lines[index], source_path));
      continue;
    }
    if (matches(lines[index].text, configuration_end, &match)) {
      configuration.clear();
      globals.clear();
      access_names.clear();
      in_resource = false;
      in_globals = false;
      continue;
    }
    if (matches(lines[index].text, resource_start, &match)) {
      in_resource = true;
      continue;
    }
    if (matches(lines[index].text, resource_end, &match)) {
      in_resource = false;
      continue;
    }
    if (!in_resource && matches(lines[index].text, global_start, &match)) {
      in_globals = true;
      globals_constant = match[1].matched;
      continue;
    }
    if (in_globals && matches(lines[index].text, var_end, &match)) {
      in_globals = false;
      globals_constant = false;
      continue;
    }
    if (in_globals && matches(lines[index].text, global_declaration, &match)) {
      globals[uppercase(match[1].str())] =
          {uppercase(match[2].str()), globals_constant};
      continue;
    }
    if (!matches(lines[index].text, access_start, &match)) continue;

    result->used_access_variables = true;
    removed[index] = true;
    if (in_resource) {
      diagnostics.error("VAR_ACCESS is supported only at CONFIGURATION scope",
                        line_range(lines[index], source_path));
    }

    bool found_declaration = false;
    std::size_t cursor = index + 1;
    for (; cursor < lines.size(); ++cursor) {
      removed[cursor] = true;
      std::smatch declaration_match;
      if (matches(lines[cursor].text, var_end, &declaration_match)) break;
      if (lines[cursor].text.find_first_not_of(" \t\r") == std::string::npos)
        continue;
      found_declaration = true;
      if (!matches(lines[cursor].text, access_declaration,
                   &declaration_match)) {
        diagnostics.error(
            "Unsupported VAR_ACCESS declaration; expected name : configuration_global : type [READ_ONLY|READ_WRITE];",
            line_range(lines[cursor], source_path));
        continue;
      }

      AccessVariableAst declaration;
      declaration.configuration = configuration;
      declaration.name = declaration_match[1].str();
      declaration.path = declaration_match[2].str();
      declaration.type = declaration_match[3].str();
      declaration.direction = declaration_match[4].matched
                                  ? uppercase(declaration_match[4].str())
                                  : "READ_ONLY";
      declaration.range = line_range(lines[cursor], source_path);

      const std::string name_key = uppercase(declaration.name);
      const std::string path_key = uppercase(declaration.path);
      const auto target = globals.find(path_key);
      if (!access_names.insert(name_key).second) {
        diagnostics.error("Duplicate VAR_ACCESS name: " + declaration.name,
                          declaration.range);
      } else if (target == globals.end()) {
        diagnostics.error("Unresolved VAR_ACCESS target: " + declaration.path,
                          declaration.range);
      } else if (uppercase(declaration.type) != target->second.type) {
        diagnostics.error("VAR_ACCESS type does not match target '" +
                              declaration.path + "'",
                          declaration.range);
      } else if (declaration.direction == "READ_WRITE" &&
                 target->second.constant) {
        diagnostics.error("READ_WRITE VAR_ACCESS cannot target CONSTANT variable '" +
                              declaration.path + "'",
                          declaration.range);
      } else {
        result->declarations.push_back(std::move(declaration));
      }
    }
    if (cursor == lines.size()) {
      diagnostics.error("VAR_ACCESS block is missing END_VAR",
                        line_range(lines[index], source_path));
      break;
    }
    if (!found_declaration)
      diagnostics.error("VAR_ACCESS block must declare at least one access name",
                        line_range(lines[index], source_path));
    index = cursor;
  }

  if (diagnostics.has_errors()) return false;
  std::string normalized;
  for (std::size_t index = 0; index < lines.size(); ++index) {
    if (!removed[index]) normalized += lines[index].text;
    if (lines[index].has_newline) normalized += '\n';
  }
  result->source = std::move(normalized);
  return true;
}

bool write_access_variable_metadata(
    const AccessVariableNormalizeResult &result,
    const std::string &output_directory, OutputManager &outputs) {
  if (!result.used_access_variables) return true;
  const std::string path = output_directory.empty()
                               ? "ACCESS.csv"
                               : output_directory + "/ACCESS.csv";
  FileOutputSink &sink = outputs.create_file(path);
  if (!sink.good()) return false;
  if (!outputs.write(sink, "configuration,name,path,type,direction\n").ok)
    return false;
  for (const AccessVariableAst &declaration : result.declarations) {
    const std::string row = declaration.configuration + "," + declaration.name +
                            "," + declaration.path + "," + declaration.type +
                            "," + declaration.direction + "\n";
    if (!outputs.write(sink, row).ok) return false;
  }
  return outputs.flush(sink).ok;
}

}  // namespace matiec

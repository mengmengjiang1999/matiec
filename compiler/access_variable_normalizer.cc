#include "compiler/access_variable_normalizer.hh"

#include "compiler/diagnostic_engine.hh"
#include "compiler/output_manager.hh"

#include <fstream>
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
                     begin, end, number++});
    begin = end;
  }
  if (source.empty()) lines.push_back({"", 0, 0, 1});
  return lines;
}

SourceRange line_range(const Line &line, const std::string &path) {
  return {{path, line.number, 1, line.begin},
          {path, line.number, line.text.size() + 1, line.end}};
}

}  // namespace

bool reject_legacy_access_variables(std::string_view source,
                                    const std::string &source_path,
                                    DiagnosticEngine &diagnostics) {
  const std::regex access_start("^[ \\t]*VAR_ACCESS\\b.*$", std::regex::icase);
  for (const Line &line : split_lines(source)) {
    if (!std::regex_match(line.text, access_start)) continue;
    diagnostics.error(
        "VAR_ACCESS requires --std=iec61131-3:2025-experimental",
        line_range(line, source_path));
    return false;
  }
  return true;
}

bool reject_legacy_access_variables_in_file(
    const std::string &source_path, DiagnosticEngine &diagnostics) {
  std::ifstream input(source_path, std::ios::in | std::ios::binary);
  if (!input) return true;
  std::ostringstream contents;
  contents << input.rdbuf();
  return reject_legacy_access_variables(contents.str(), source_path, diagnostics);
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

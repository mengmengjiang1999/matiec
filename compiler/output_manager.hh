#ifndef MATIEC_COMPILER_OUTPUT_MANAGER_HH
#define MATIEC_COMPILER_OUTPUT_MANAGER_HH

#include "diagnostic_engine.hh"
#include "output_sink.hh"

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace matiec {

class OutputManager {
 public:
  explicit OutputManager(DiagnosticEngine &diagnostics);

  OutputSink &standard_output();
  FileOutputSink &create_file(std::string path);
  MemoryOutputSink &create_memory();

  OutputResult write(OutputSink &sink, std::string_view text);
  OutputResult flush(OutputSink &sink);
  bool has_errors() const;

 private:
  void record_failure(OutputSink &sink, const OutputResult &result);

  DiagnosticEngine &diagnostics_;
  StreamOutputSink standard_output_;
  std::vector<std::unique_ptr<OutputSink>> owned_sinks_;
  std::unordered_set<const OutputSink *> reported_sinks_;
  bool has_errors_ = false;
};

}  // namespace matiec

#endif

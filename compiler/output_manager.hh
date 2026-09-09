#ifndef MATIEC_COMPILER_OUTPUT_MANAGER_HH
#define MATIEC_COMPILER_OUTPUT_MANAGER_HH

#include "diagnostic_engine.hh"
#include "output_sink.hh"

#include <memory>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

namespace matiec {

class OutputManager {
 public:
  using FileSinkFactory =
      std::function<std::unique_ptr<OutputSink>(std::string)>;

  explicit OutputManager(DiagnosticEngine &diagnostics);
  OutputManager(DiagnosticEngine &diagnostics, OutputSink &standard_output);

  OutputSink &standard_output();
  OutputSink &create_file(std::string path);
  MemoryOutputSink &create_memory();
  void set_file_sink_factory(FileSinkFactory factory);

  OutputResult write(OutputSink &sink, std::string_view text);
  OutputResult flush(OutputSink &sink);
  bool has_errors() const;

 private:
  void record_failure(OutputSink &sink, const OutputResult &result);

  DiagnosticEngine &diagnostics_;
  StreamOutputSink default_standard_output_;
  OutputSink *standard_output_;
  std::vector<std::unique_ptr<OutputSink>> owned_sinks_;
  FileSinkFactory file_sink_factory_;
  std::unordered_set<const OutputSink *> reported_sinks_;
  bool has_errors_ = false;
};

}  // namespace matiec

#endif

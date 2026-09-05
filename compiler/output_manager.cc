#include "output_manager.hh"

#include <iostream>
#include <utility>

namespace matiec {

OutputManager::OutputManager(DiagnosticEngine &diagnostics)
    : diagnostics_(diagnostics),
      default_standard_output_(std::cout),
      standard_output_(&default_standard_output_) {}

OutputManager::OutputManager(DiagnosticEngine &diagnostics,
                             OutputSink &standard_output)
    : diagnostics_(diagnostics),
      default_standard_output_(std::cout),
      standard_output_(&standard_output) {}

OutputSink &OutputManager::standard_output() {
  return *standard_output_;
}

FileOutputSink &OutputManager::create_file(std::string path) {
  auto sink = std::make_unique<FileOutputSink>(std::move(path));
  FileOutputSink &result = *sink;
  owned_sinks_.push_back(std::move(sink));
  if (!result.good()) {
    record_failure(result, OutputResult::failure(result.error_message()));
  }
  return result;
}

MemoryOutputSink &OutputManager::create_memory() {
  auto sink = std::make_unique<MemoryOutputSink>();
  MemoryOutputSink &result = *sink;
  owned_sinks_.push_back(std::move(sink));
  return result;
}

OutputResult OutputManager::write(OutputSink &sink, std::string_view text) {
  const OutputResult result = sink.write(text);
  if (!result.ok) record_failure(sink, result);
  return result;
}

OutputResult OutputManager::flush(OutputSink &sink) {
  const OutputResult result = sink.flush();
  if (!result.ok) record_failure(sink, result);
  return result;
}

bool OutputManager::has_errors() const {
  return has_errors_;
}

void OutputManager::record_failure(OutputSink &sink,
                                   const OutputResult &result) {
  has_errors_ = true;
  if (reported_sinks_.insert(&sink).second) {
    diagnostics_.error(
        result.message.empty()
            ? "Error writing generated output"
            : "Error writing generated output: " + result.message);
  }
}

}  // namespace matiec

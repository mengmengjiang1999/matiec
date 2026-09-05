#include "output_sink.hh"

#include <ostream>
#include <utility>

namespace matiec {

OutputResult OutputResult::success() {
  return {};
}

OutputResult OutputResult::failure(std::string message) {
  return {false, std::move(message)};
}

FileOutputSink::FileOutputSink(std::string path)
    : path_(std::move(path)), stream_(path_, std::ios::out | std::ios::binary) {
  if (!stream_) {
    const std::size_t separator = path_.find_last_of("/\\");
    const std::string filename = separator == std::string::npos
                                     ? path_
                                     : path_.substr(separator + 1);
    error_message_ = "Cannot open " + filename + " for write access";
  }
}

OutputResult FileOutputSink::write(std::string_view text) {
  if (!good()) return OutputResult::failure(error_message_);
  stream_.write(text.data(), static_cast<std::streamsize>(text.size()));
  return stream_ ? OutputResult::success() : fail("write");
}

OutputResult FileOutputSink::flush() {
  if (!good()) return OutputResult::failure(error_message_);
  stream_.flush();
  return stream_ ? OutputResult::success() : fail("flush");
}

bool FileOutputSink::good() const {
  return error_message_.empty() && stream_.good();
}

const std::string &FileOutputSink::error_message() const {
  return error_message_;
}

const std::string &FileOutputSink::path() const {
  return path_;
}

OutputResult FileOutputSink::fail(std::string operation) {
  error_message_ = "Unable to " + operation + " generated output '" + path_ +
                   "'";
  return OutputResult::failure(error_message_);
}

OutputResult MemoryOutputSink::write(std::string_view text) {
  contents_.append(text.data(), text.size());
  return OutputResult::success();
}

OutputResult MemoryOutputSink::flush() {
  return OutputResult::success();
}

bool MemoryOutputSink::good() const {
  return true;
}

const std::string &MemoryOutputSink::error_message() const {
  return error_message_;
}

const std::string &MemoryOutputSink::contents() const {
  return contents_;
}

void MemoryOutputSink::clear() {
  contents_.clear();
}

StreamOutputSink::StreamOutputSink(std::ostream &stream) : stream_(stream) {}

OutputResult StreamOutputSink::write(std::string_view text) {
  if (!good()) return OutputResult::failure(error_message_);
  stream_.write(text.data(), static_cast<std::streamsize>(text.size()));
  return stream_ ? OutputResult::success() : fail("write");
}

OutputResult StreamOutputSink::flush() {
  if (!good()) return OutputResult::failure(error_message_);
  stream_.flush();
  return stream_ ? OutputResult::success() : fail("flush");
}

bool StreamOutputSink::good() const {
  return error_message_.empty() && stream_.good();
}

const std::string &StreamOutputSink::error_message() const {
  return error_message_;
}

OutputResult StreamOutputSink::fail(std::string operation) {
  error_message_ = "Unable to " + operation + " generated output stream";
  return OutputResult::failure(error_message_);
}

}  // namespace matiec

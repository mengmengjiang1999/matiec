#ifndef MATIEC_COMPILER_OUTPUT_SINK_HH
#define MATIEC_COMPILER_OUTPUT_SINK_HH

#include <fstream>
#include <iosfwd>
#include <string>
#include <string_view>

namespace matiec {

struct OutputResult {
  bool ok = true;
  std::string message;

  static OutputResult success();
  static OutputResult failure(std::string message);
};

class OutputSink {
 public:
  virtual ~OutputSink() = default;

  virtual OutputResult write(std::string_view text) = 0;
  virtual OutputResult flush() = 0;
  virtual bool good() const = 0;
  virtual const std::string &error_message() const = 0;
};

class FileOutputSink final : public OutputSink {
 public:
  explicit FileOutputSink(std::string path);

  OutputResult write(std::string_view text) override;
  OutputResult flush() override;
  bool good() const override;
  const std::string &error_message() const override;

  const std::string &path() const;

 private:
  OutputResult fail(std::string operation);

  std::string path_;
  std::ofstream stream_;
  std::string error_message_;
};

class MemoryOutputSink final : public OutputSink {
 public:
  OutputResult write(std::string_view text) override;
  OutputResult flush() override;
  bool good() const override;
  const std::string &error_message() const override;

  const std::string &contents() const;
  void clear();

 private:
  std::string contents_;
  std::string error_message_;
};

class StreamOutputSink final : public OutputSink {
 public:
  explicit StreamOutputSink(std::ostream &stream);

  OutputResult write(std::string_view text) override;
  OutputResult flush() override;
  bool good() const override;
  const std::string &error_message() const override;

 private:
  OutputResult fail(std::string operation);

  std::ostream &stream_;
  std::string error_message_;
};

}  // namespace matiec

#endif

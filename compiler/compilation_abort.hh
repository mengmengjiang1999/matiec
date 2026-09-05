#ifndef MATIEC_COMPILER_COMPILATION_ABORT_HH
#define MATIEC_COMPILER_COMPILATION_ABORT_HH

#include <stdexcept>
#include <string>
#include <utility>

namespace matiec {

class CompilationAbort : public std::runtime_error {
 public:
  explicit CompilationAbort(std::string message, bool diagnostic_reported = false)
      : std::runtime_error(std::move(message)),
        diagnostic_reported_(diagnostic_reported) {}

  bool diagnostic_reported() const { return diagnostic_reported_; }

 private:
  bool diagnostic_reported_;
};

}  // namespace matiec

#endif

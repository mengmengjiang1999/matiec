#include "compiler/compilation_abort.hh"

#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

void error_exit(const char *file_name, int line_no, const char *errmsg, ...) {
  std::string message = "Internal compiler error in file ";
  message += file_name;
  message += " at line ";
  message += std::to_string(line_no);

  if (errmsg != NULL) {
    va_list arguments;
    va_start(arguments, errmsg);
    va_list arguments_copy;
    va_copy(arguments_copy, arguments);
    const int size = std::vsnprintf(NULL, 0, errmsg, arguments_copy);
    va_end(arguments_copy);
    if (size > 0) {
      std::vector<char> formatted(static_cast<std::size_t>(size) + 1);
      std::vsnprintf(formatted.data(), formatted.size(), errmsg, arguments);
      message += ": ";
      message += formatted.data();
    }
    va_end(arguments);
  } else {
    message += '.';
  }

  throw matiec::CompilationAbort(std::move(message));
}

#ifndef MATIEC_COMPILER_UTF8_VALIDATION_HH
#define MATIEC_COMPILER_UTF8_VALIDATION_HH

#include <cstddef>
#include <cstdio>
#include <string>

namespace matiec {

struct Utf8Error {
  std::size_t offset = 0;
  std::size_t line = 1;
  std::size_t column = 1;
  std::string reason;
};

bool validate_utf8_bytes(const unsigned char *data, std::size_t size,
                         Utf8Error *error);
bool validate_utf8_file(FILE *file, Utf8Error *error);
bool validate_utf8_file(const std::string &path, Utf8Error *error);

}  // namespace matiec

#endif

#include "compiler/utf8_validation.hh"

#include <vector>

namespace matiec {
namespace {

bool fail(Utf8Error *error, std::size_t offset, std::size_t line,
          std::size_t column, const char *reason) {
  if (error != NULL) {
    error->offset = offset;
    error->line = line;
    error->column = column;
    error->reason = reason;
  }
  return false;
}

bool continuation(unsigned char byte) {
  return byte >= 0x80 && byte <= 0xbf;
}

}  // namespace

bool validate_utf8_bytes(const unsigned char *data, std::size_t size,
                         Utf8Error *error) {
  std::size_t line = 1;
  std::size_t column = 1;
  for (std::size_t index = 0; index < size;) {
    const unsigned char lead = data[index];
    std::size_t width = 1;
    bool valid = true;
    if (lead <= 0x7f) {
      width = 1;
    } else if (lead >= 0xc2 && lead <= 0xdf) {
      width = 2;
      valid = index + width <= size && continuation(data[index + 1]);
    } else if (lead >= 0xe0 && lead <= 0xef) {
      width = 3;
      valid = index + width <= size && continuation(data[index + 1]) &&
              continuation(data[index + 2]);
      if (valid && lead == 0xe0) valid = data[index + 1] >= 0xa0;
      if (valid && lead == 0xed) valid = data[index + 1] <= 0x9f;
    } else if (lead >= 0xf0 && lead <= 0xf4) {
      width = 4;
      valid = index + width <= size && continuation(data[index + 1]) &&
              continuation(data[index + 2]) && continuation(data[index + 3]);
      if (valid && lead == 0xf0) valid = data[index + 1] >= 0x90;
      if (valid && lead == 0xf4) valid = data[index + 1] <= 0x8f;
    } else {
      valid = false;
    }
    if (!valid)
      return fail(error, index, line, column, "ill-formed UTF-8 byte sequence");
    if (lead == '\n') {
      ++line;
      column = 1;
    } else {
      ++column;
    }
    index += width;
  }
  return true;
}

bool validate_utf8_file(FILE *file, Utf8Error *error) {
  if (file == NULL)
    return fail(error, 0, 1, 1, "source file is not open");
  fpos_t original;
  if (fgetpos(file, &original) != 0)
    return fail(error, 0, 1, 1, "cannot inspect source file position");
  rewind(file);
  std::vector<unsigned char> bytes;
  unsigned char buffer[4096];
  while (true) {
    const std::size_t count = fread(buffer, 1, sizeof(buffer), file);
    bytes.insert(bytes.end(), buffer, buffer + count);
    if (count != sizeof(buffer)) break;
  }
  const bool read_ok = !ferror(file);
  clearerr(file);
  const bool restored = fsetpos(file, &original) == 0;
  if (!read_ok || !restored)
    return fail(error, 0, 1, 1, "cannot read source file for UTF-8 validation");
  return validate_utf8_bytes(bytes.data(), bytes.size(), error);
}

bool validate_utf8_file(const std::string &path, Utf8Error *error) {
  FILE *file = fopen(path.c_str(), "rb");
  if (file == NULL)
    return fail(error, 0, 1, 1, "cannot open source file for UTF-8 validation");
  const bool valid = validate_utf8_file(file, error);
  fclose(file);
  return valid;
}

}  // namespace matiec

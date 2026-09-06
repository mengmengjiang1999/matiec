#include "compiler/utf8_validation.hh"

#include <cassert>
#include <cstring>

int main() {
  matiec::Utf8Error error;
  const char valid[] = "ASCII \xe4\xb8\xad\xf0\x9f\x9a\x80";
  assert(matiec::validate_utf8_bytes(
      reinterpret_cast<const unsigned char *>(valid), strlen(valid), &error));
  const unsigned char overlong[] = {0xc0, 0x80};
  assert(!matiec::validate_utf8_bytes(overlong, sizeof(overlong), &error));
  assert(error.offset == 0 && error.line == 1 && error.column == 1);
  const unsigned char surrogate[] = {0xed, 0xa0, 0x80};
  assert(!matiec::validate_utf8_bytes(surrogate, sizeof(surrogate), &error));
  const unsigned char out_of_range[] = {0xf4, 0x90, 0x80, 0x80};
  assert(!matiec::validate_utf8_bytes(out_of_range, sizeof(out_of_range), &error));
  const unsigned char truncated[] = {'a', '\n', 0xe4, 0xb8};
  assert(!matiec::validate_utf8_bytes(truncated, sizeof(truncated), &error));
  assert(error.offset == 2 && error.line == 2 && error.column == 1);
}

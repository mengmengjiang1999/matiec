#include "include/matiec/api.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

namespace {

constexpr std::size_t kModeCount = 5;
constexpr std::size_t kMaxInputBytes = 1u << 20;

struct IncludeState {
  const std::uint8_t *data;
  std::size_t size;
};

[[noreturn]] void api_failure() { std::abort(); }

void require_ok(matiec_status_t status) {
  if (status != MATIEC_STATUS_OK) api_failure();
}

matiec_include_result_t resolve_fuzz_include(
    void *user_data, const char *requested, matiec_source_view_t *source) {
  IncludeState *state = static_cast<IncludeState *>(user_data);
  if (std::string(requested) != "fuzz.st")
    return MATIEC_INCLUDE_USE_FILESYSTEM;
  source->display_name = "memory://fuzz.st";
  source->data = state->data;
  source->size = state->size;
  return MATIEC_INCLUDE_RESOLVED;
}

int discard_output(void *, const char *, const void *, std::size_t) { return 1; }

void configure(matiec_context_t *context, bool syntax_only) {
  const char *library = std::getenv("MATIEC_LIBDIR");
  require_ok(matiec_context_set_include_directory(
      context, library == nullptr ? "lib" : library));
  require_ok(matiec_context_set_syntax_only(context, syntax_only ? 1 : 0));
}

void compile_context(matiec_context_t *context) {
  matiec_result_t result = MATIEC_RESULT_INIT;
  require_ok(matiec_context_compile(context, &result));
}

bool write_all(int descriptor, const std::uint8_t *data, std::size_t size) {
  while (size != 0) {
    const ssize_t written = ::write(descriptor, data, size);
    if (written <= 0) return false;
    data += static_cast<std::size_t>(written);
    size -= static_cast<std::size_t>(written);
  }
  return true;
}

void fuzz_file(const std::uint8_t *data, std::size_t size) {
  char path[] = "/tmp/matiec-fuzz-XXXXXX";
  const int descriptor = mkstemp(path);
  if (descriptor < 0) return;
  const bool written = write_all(descriptor, data, size);
  close(descriptor);

  matiec_context_t *context = nullptr;
  require_ok(matiec_context_create(&context));
  configure(context, true);
  if (written) {
    require_ok(matiec_context_set_source_path(context, path));
    compile_context(context);
  }
  matiec_context_destroy(context);
  unlink(path);
}

void fuzz_memory(const std::uint8_t *data, std::size_t size) {
  matiec_context_t *context = nullptr;
  require_ok(matiec_context_create(&context));
  configure(context, true);
  require_ok(matiec_context_set_source(
      context, "memory://fuzz.st", data, size));
  compile_context(context);
  matiec_context_destroy(context);
}

void fuzz_include(const std::uint8_t *data, std::size_t size) {
  static const char main_source[] =
      "{#include \"fuzz.st\"}\nPROGRAM FuzzMain\nEND_PROGRAM\n";
  IncludeState state{data, size};
  matiec_context_t *context = nullptr;
  require_ok(matiec_context_create(&context));
  configure(context, true);
  require_ok(matiec_context_set_source(
      context, "memory://main.st", main_source, sizeof(main_source) - 1));
  require_ok(matiec_context_set_include_resolver(
      context, resolve_fuzz_include, &state));
  compile_context(context);
  matiec_context_destroy(context);
}

void fuzz_cancel(const std::uint8_t *data, std::size_t size) {
  matiec_context_t *context = nullptr;
  require_ok(matiec_context_create(&context));
  configure(context, true);
  require_ok(matiec_context_set_source(
      context, "memory://cancel.st", data, size));
  require_ok(matiec_context_cancel(context));
  compile_context(context);
  require_ok(matiec_context_reset_cancel(context));
  matiec_context_destroy(context);
}

void fuzz_limits(const std::uint8_t *data, std::size_t size) {
  matiec_limits_t limits = MATIEC_LIMITS_INIT;
  const std::uint8_t selector = size == 0 ? 0 : data[0];
  limits.max_source_bytes = (selector & 1u) == 0 ? size : size / 2 + 1;
  limits.max_diagnostics = static_cast<std::uint64_t>(selector % 8u + 1u);
  limits.max_output_bytes = static_cast<std::uint64_t>(selector % 64u + 1u);

  matiec_context_t *context = nullptr;
  require_ok(matiec_context_create(&context));
  configure(context, false);
  require_ok(matiec_context_set_output_callback(context, discard_output, nullptr));
  require_ok(matiec_context_set_limits(context, &limits));
  require_ok(matiec_context_set_source(
      context, "memory://limits.st", data, size));
  compile_context(context);
  matiec_context_destroy(context);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *data,
                                      std::size_t size) {
  if (size == 0) return 0;
  const std::size_t mode = data[0] % kModeCount;
  ++data;
  size = std::min(size - 1, kMaxInputBytes);
  switch (mode) {
    case 0: fuzz_memory(data, size); break;
    case 1: fuzz_file(data, size); break;
    case 2: fuzz_include(data, size); break;
    case 3: fuzz_cancel(data, size); break;
    case 4: fuzz_limits(data, size); break;
  }
  return 0;
}

#ifndef MATIEC_LIBFUZZER
int main(int argc, char **argv) {
  if (argc == 1) {
    const std::vector<std::uint8_t> input{
        std::istreambuf_iterator<char>(std::cin),
        std::istreambuf_iterator<char>()};
    return LLVMFuzzerTestOneInput(input.data(), input.size());
  }
  for (int index = 1; index < argc; ++index) {
    std::ifstream file(argv[index], std::ios::binary);
    if (!file) return 2;
    const std::vector<std::uint8_t> input{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()};
    LLVMFuzzerTestOneInput(input.data(), input.size());
  }
  return 0;
}
#endif

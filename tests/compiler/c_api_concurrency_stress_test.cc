#include "include/matiec/api.h"

#include <array>
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

namespace {
constexpr std::size_t kContextCount = 8;
constexpr int kRounds = 8;
enum Mode { valid, invalid, resolved_include, cancelled, resolver_exception };

struct CallbackState {
  std::atomic<std::size_t> diagnostics{0};
  std::atomic<std::size_t> resolver_calls{0};
  Mode mode = valid;
};

void receive_diagnostic(void *user_data,
                        const matiec_diagnostic_t *diagnostic) {
  auto *state = static_cast<CallbackState *>(user_data);
  assert(diagnostic != nullptr && diagnostic->code != nullptr);
  state->diagnostics.fetch_add(1, std::memory_order_relaxed);
}

matiec_include_result_t resolve_include(void *user_data, const char *requested,
                                        matiec_source_view_t *source) {
  static const char contents[] = "(* isolated virtual include *)\n";
  auto *state = static_cast<CallbackState *>(user_data);
  state->resolver_calls.fetch_add(1, std::memory_order_relaxed);
  if (std::strcmp(requested, "virtual.st") != 0)
    return MATIEC_INCLUDE_USE_FILESYSTEM;
  if (state->mode == resolver_exception)
    throw std::runtime_error("injected resolver failure");
  source->display_name = "memory://virtual.st";
  source->data = contents;
  source->size = sizeof(contents) - 1;
  return MATIEC_INCLUDE_RESOLVED;
}

int throwing_output(void *, const char *, const void *, std::size_t) {
  throw std::runtime_error("injected output failure");
}

const char *source_for(Mode mode) {
  switch (mode) {
    case invalid:
      return "PROGRAM Broken\nVAR value : ; END_VAR\nEND_PROGRAM\n";
    case resolved_include:
    case resolver_exception:
      return "{#include \"virtual.st\"}\nPROGRAM Included\n"
             "VAR value : INT; END_VAR\nvalue := 1;\nEND_PROGRAM\n";
    default:
      return "PROGRAM Healthy\nVAR value : INT; END_VAR\n"
             "value := 1;\nEND_PROGRAM\n";
  }
}

bool should_succeed(Mode mode) {
  return mode == valid || mode == resolved_include;
}
}  // namespace

int main() {
  const char *library = std::getenv("MATIEC_LIBDIR");
  assert(library != nullptr);
  const std::array<Mode, kContextCount> modes = {
      valid, invalid, resolved_include, cancelled,
      resolver_exception, valid, invalid, resolved_include};
  std::array<CallbackState, kContextCount> states;
  std::array<matiec_context_t *, kContextCount> contexts{};
  std::array<matiec_result_t, kContextCount> results;

  for (std::size_t index = 0; index < kContextCount; ++index) {
    states[index].mode = modes[index];
    assert(matiec_context_create(&contexts[index]) == MATIEC_STATUS_OK);
    assert(matiec_context_set_include_directory(contexts[index], library) ==
           MATIEC_STATUS_OK);
    assert(matiec_context_set_syntax_only(contexts[index], 1) ==
           MATIEC_STATUS_OK);
    assert(matiec_context_set_diagnostic_callback(
               contexts[index], receive_diagnostic, &states[index]) ==
           MATIEC_STATUS_OK);
    if (modes[index] == resolved_include || modes[index] == resolver_exception)
      assert(matiec_context_set_include_resolver(
                 contexts[index], resolve_include, &states[index]) ==
             MATIEC_STATUS_OK);
  }

  for (int round = 0; round < kRounds; ++round) {
    for (std::size_t index = 0; index < kContextCount; ++index) {
      const std::string name = "memory://stress-" + std::to_string(index) +
                               "-" + std::to_string(round) + ".st";
      const char *source = source_for(modes[index]);
      assert(matiec_context_set_source(contexts[index], name.c_str(), source,
                                       std::strlen(source)) == MATIEC_STATUS_OK);
      results[index] = MATIEC_RESULT_INIT;
      if (modes[index] == cancelled)
        assert(matiec_context_cancel(contexts[index]) == MATIEC_STATUS_OK);
    }
    assert(matiec_compile_batch(contexts.data(), contexts.size(), 4,
                                results.data()) == MATIEC_STATUS_OK);
    for (std::size_t index = 0; index < kContextCount; ++index) {
      assert((results[index].succeeded != 0u) ==
             should_succeed(modes[index]));
      if (!should_succeed(modes[index]))
        assert(results[index].error_count > 0u);
      if (modes[index] == cancelled)
        assert(matiec_context_reset_cancel(contexts[index]) ==
               MATIEC_STATUS_OK);
    }
  }

  for (std::size_t index = 0; index < kContextCount; ++index) {
    if (modes[index] == cancelled || modes[index] == resolver_exception)
      assert(states[index].diagnostics.load(std::memory_order_relaxed) >=
             static_cast<std::size_t>(kRounds));
    if (modes[index] == resolved_include || modes[index] == resolver_exception)
      assert(states[index].resolver_calls.load(std::memory_order_relaxed) >=
             static_cast<std::size_t>(kRounds));
    matiec_context_destroy(contexts[index]);
  }

  const char source[] =
      "PROGRAM OutputFailure\nVAR value : INT; END_VAR\n"
      "value := 1;\nEND_PROGRAM\n";
  matiec_context_t *context = nullptr;
  matiec_result_t result = MATIEC_RESULT_INIT;
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_source(context, "memory://output-failure.st",
                                   source, sizeof(source) - 1) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_output_callback(context, throwing_output, nullptr) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u && result.error_count > 0u);
  matiec_context_destroy(context);
  return 0;
}

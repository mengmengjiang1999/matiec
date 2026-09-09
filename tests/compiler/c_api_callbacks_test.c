#include "include/matiec/api.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct callback_state {
  size_t diagnostic_count;
  size_t output_chunks;
  size_t output_bytes;
  int reject_output;
};

static void receive_diagnostic(void *user_data,
                               const matiec_diagnostic_t *diagnostic) {
  struct callback_state *state = (struct callback_state *)user_data;
  assert(diagnostic != NULL);
  assert(diagnostic->message != NULL);
  ++state->diagnostic_count;
}

static int receive_output(void *user_data, const char *path,
                          const void *data, size_t size) {
  struct callback_state *state = (struct callback_state *)user_data;
  assert(path != NULL);
  assert(data != NULL || size == 0);
  ++state->output_chunks;
  state->output_bytes += size;
  return !state->reject_output;
}

static matiec_context_t *new_context(const char *library) {
  matiec_context_t *context = NULL;
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  return context;
}

int main(void) {
  const char *library = getenv("MATIEC_LIBDIR");
  const char valid_source[] =
      "PROGRAM CallbackMain\n"
      "VAR value : INT; END_VAR\n"
      "value := 42;\n"
      "END_PROGRAM\n";
  struct callback_state diagnostic_state = {0, 0, 0, 0};
  struct callback_state output_state = {0, 0, 0, 0};
  matiec_result_t result = MATIEC_RESULT_INIT;
  matiec_diagnostic_t diagnostic = MATIEC_DIAGNOSTIC_INIT;
  matiec_context_t *context;

  assert(library != NULL);
  context = new_context(library);
  assert(matiec_context_set_syntax_only(context, 1) == MATIEC_STATUS_OK);
  assert(matiec_context_set_diagnostic_callback(
             context, receive_diagnostic, &diagnostic_state) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u);
  assert(diagnostic_state.diagnostic_count > 0u);
  assert(matiec_context_diagnostic_count(context) ==
         diagnostic_state.diagnostic_count);
  assert(matiec_context_get_diagnostic(context, 0, &diagnostic) ==
         MATIEC_STATUS_OK);
  assert(diagnostic.severity == MATIEC_DIAGNOSTIC_ERROR ||
         diagnostic.severity == MATIEC_DIAGNOSTIC_FATAL);
  assert(diagnostic.message[0] != '\0');
  matiec_context_destroy(context);

  context = new_context(library);
  assert(matiec_context_set_output_callback(
             context, receive_output, &output_state) == MATIEC_STATUS_OK);
  assert(matiec_context_set_source(context, "memory://callback.st",
                                   valid_source,
                                   strlen(valid_source)) == MATIEC_STATUS_OK);
  result = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 1u);
  assert(output_state.output_chunks > 0u);
  assert(output_state.output_bytes > 0u);
  matiec_context_destroy(context);

  output_state = (struct callback_state){0, 0, 0, 1};
  context = new_context(library);
  assert(matiec_context_set_output_callback(
             context, receive_output, &output_state) == MATIEC_STATUS_OK);
  assert(matiec_context_set_source(context, "memory://rejected.st",
                                   valid_source,
                                   strlen(valid_source)) == MATIEC_STATUS_OK);
  result = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u);
  assert(result.error_count > 0u);
  matiec_context_destroy(context);
  return 0;
}

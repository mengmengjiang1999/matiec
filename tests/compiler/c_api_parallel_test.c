#include "include/matiec/api.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static matiec_context_t *configured_context(const char *library,
                                            const char *name) {
  const char source[] =
      "PROGRAM ParallelMain\n"
      "VAR value : INT; END_VAR\n"
      "value := 42;\n"
      "END_PROGRAM\n";
  matiec_context_t *context = NULL;
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_syntax_only(context, 1) == MATIEC_STATUS_OK);
  if (name != NULL)
    assert(matiec_context_set_source(context, name, source,
                                     strlen(source)) == MATIEC_STATUS_OK);
  return context;
}

int main(void) {
  const char *library = getenv("MATIEC_LIBDIR");
  matiec_context_t *contexts[2];
  matiec_result_t results[2] = {MATIEC_RESULT_INIT, MATIEC_RESULT_INIT};
  matiec_context_t *duplicate[2];
  matiec_result_t duplicate_results[2] = {
      MATIEC_RESULT_INIT, MATIEC_RESULT_INIT};

  assert(library != NULL);
  assert(matiec_compile_batch(NULL, 0, 0, NULL) == MATIEC_STATUS_OK);
  contexts[0] = configured_context(library, "memory://parallel-ok.st");
  contexts[1] = configured_context(library, NULL);
  assert(matiec_compile_batch(contexts, 2, 0, results) == MATIEC_STATUS_OK);
  assert(results[0].succeeded == 1u);
  assert(results[1].succeeded == 0u);
  assert(results[1].error_count > 0u);
  matiec_context_destroy(contexts[0]);
  matiec_context_destroy(contexts[1]);

  contexts[0] = configured_context(library, "memory://duplicate.st");
  duplicate[0] = contexts[0];
  duplicate[1] = contexts[0];
  assert(matiec_compile_batch(duplicate, 2, 2, duplicate_results) ==
         MATIEC_STATUS_OK);
  assert(duplicate_results[0].succeeded == 0u);
  assert(duplicate_results[1].succeeded == 0u);
  assert(duplicate_results[0].error_count > 0u);
  assert(matiec_context_diagnostic_count(contexts[0]) > 0u);
  matiec_context_destroy(contexts[0]);
  return 0;
}

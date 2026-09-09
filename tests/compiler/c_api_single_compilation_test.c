#include "include/matiec/api.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  const char *library = getenv("MATIEC_LIBDIR");
  const char valid_source[] =
      "PROGRAM MemoryMain\n"
      "VAR value : INT; END_VAR\n"
      "value := 42;\n"
      "END_PROGRAM\n";
  const char invalid_source[] = "PROGRAM Broken\n";
  matiec_context_t *context = NULL;
  matiec_result_t result = MATIEC_RESULT_INIT;

  assert(library != NULL);
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(context != NULL);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_syntax_only(context, 1) == MATIEC_STATUS_OK);
  assert(matiec_context_set_language_profile(context, MATIEC_PROFILE_LEGACY) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_output_language(context, MATIEC_OUTPUT_C) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_source(context, "memory://valid.st", valid_source,
                                   strlen(valid_source)) == MATIEC_STATUS_OK);
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 1u);
  assert(result.error_count == 0u);

  assert(matiec_context_set_source(context, "memory://invalid.st",
                                   invalid_source,
                                   strlen(invalid_source)) == MATIEC_STATUS_OK);
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u);
  assert(result.error_count > 0u);

  assert(matiec_context_set_source(context, NULL, valid_source,
                                   strlen(valid_source)) ==
         MATIEC_STATUS_INVALID_ARGUMENT);
  assert(strcmp(matiec_context_last_error(context),
                "Source display name is required") == 0);
  matiec_context_destroy(context);
  return 0;
}

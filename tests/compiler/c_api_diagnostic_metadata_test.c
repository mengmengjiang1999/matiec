#include "include/matiec/api.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static void assert_code_shape(const char *code) {
  assert(code != NULL);
  assert(strncmp(code, "MATIEC-", 7) == 0);
  assert(strlen(code) == 12u);
}

int main(void) {
  const char *library = getenv("MATIEC_LIBDIR");
  matiec_context_t *context = NULL;
  matiec_result_t result = MATIEC_RESULT_INIT;
  matiec_diagnostic_t diagnostic = MATIEC_DIAGNOSTIC_INIT;
  matiec_diagnostic_t legacy = MATIEC_DIAGNOSTIC_INIT;

  assert(library != NULL);
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u);
  assert(matiec_context_get_diagnostic(context, 0, &diagnostic) ==
         MATIEC_STATUS_OK);
  assert(strcmp(diagnostic.code, "MATIEC-E1001") == 0);
  assert(diagnostic.phase == MATIEC_DIAGNOSTIC_PHASE_SOURCE);
  assert(diagnostic.range_valid == 0u);

  legacy.struct_size = MATIEC_DIAGNOSTIC_LEGACY_SIZE;
  legacy.code = (const char *)1;
  legacy.range_valid = 77u;
  assert(matiec_context_get_diagnostic(context, 0, &legacy) ==
         MATIEC_STATUS_OK);
  assert(legacy.message != NULL);
  assert(legacy.code == (const char *)1);
  assert(legacy.range_valid == 77u);
  matiec_context_destroy(context);

  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_language_profile(
             context, MATIEC_PROFILE_IEC61131_3_2025_EXPERIMENTAL) ==
         MATIEC_STATUS_OK);
  {
    const unsigned char malformed[] = {0xc3};
    assert(matiec_context_set_source(context, "memory://bad-utf8.st",
                                     malformed, sizeof(malformed)) ==
           MATIEC_STATUS_OK);
  }
  result = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u);
  diagnostic = (matiec_diagnostic_t)MATIEC_DIAGNOSTIC_INIT;
  assert(matiec_context_get_diagnostic(context, 0, &diagnostic) ==
         MATIEC_STATUS_OK);
  assert_code_shape(diagnostic.code);
  assert(strcmp(diagnostic.code, "MATIEC-E1004") == 0);
  assert(diagnostic.phase == MATIEC_DIAGNOSTIC_PHASE_SOURCE);
  assert(diagnostic.range_valid == 1u);
  assert(diagnostic.offset_valid == 1u);
  assert(diagnostic.begin_offset == 0u && diagnostic.end_offset == 0u);
  matiec_context_destroy(context);
  return 0;
}

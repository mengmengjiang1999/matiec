#include <matiec/api.h>

#include <assert.h>
#include <string.h>

int main(int argc, char **argv) {
  const char source[] =
      "PROGRAM InstalledMain\n"
      "VAR value : INT; END_VAR\n"
      "value := 42;\n"
      "END_PROGRAM\n";
  matiec_context_t *context = NULL;
  matiec_result_t result = MATIEC_RESULT_INIT;

  assert(argc == 2);
  assert(matiec_api_version() == MATIEC_API_VERSION);
  assert(strcmp(matiec_api_version_string(), "1.5.0") == 0);
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_source(context, "memory://installed.st", source,
                                   strlen(source)) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, argv[1]) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_syntax_only(context, 1) == MATIEC_STATUS_OK);
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 1u);
  matiec_context_destroy(context);
  return 0;
}

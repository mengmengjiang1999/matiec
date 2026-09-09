#include "include/matiec/api.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct resolver_state {
  char storage[256];
  size_t calls;
  int invalid_leaf;
};

static matiec_include_result_t resolve_include(
    void *user_data, const char *requested, matiec_source_view_t *source) {
  struct resolver_state *state = (struct resolver_state *)user_data;
  ++state->calls;
  if (strcmp(requested, "a.st") == 0) {
    strcpy(state->storage, "{#include \"b.st\"}\n");
    source->display_name = "memory://a.st";
  } else if (strcmp(requested, "b.st") == 0) {
    strcpy(state->storage, state->invalid_leaf
                               ? "TYPE Broken : ; END_TYPE\n"
                               : "(* virtual leaf *)\n");
    source->display_name = "memory://b.st";
  } else {
    return strcmp(requested, "missing.st") == 0
               ? MATIEC_INCLUDE_NOT_FOUND
               : MATIEC_INCLUDE_USE_FILESYSTEM;
  }
  source->data = state->storage;
  source->size = strlen(state->storage);
  return MATIEC_INCLUDE_RESOLVED;
}

static matiec_context_t *new_context(const char *library,
                                     struct resolver_state *state,
                                     const char *source) {
  matiec_context_t *context = NULL;
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_syntax_only(context, 1) == MATIEC_STATUS_OK);
  assert(matiec_context_set_source(context, "memory://main.st", source,
                                   strlen(source)) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_resolver(context, resolve_include, state) ==
         MATIEC_STATUS_OK);
  return context;
}

int main(void) {
  const char *library = getenv("MATIEC_LIBDIR");
  const char source[] =
      "{#include \"a.st\"}\n"
      "PROGRAM Main\nVAR value : INT; END_VAR\nvalue := 1;\nEND_PROGRAM\n";
  const char missing[] =
      "{#include \"missing.st\"}\nPROGRAM Missing\n"
      "VAR value : INT; END_VAR\nvalue := 1;\nEND_PROGRAM\n";
  const char plain_source[] =
      "PROGRAM Plain\n"
      "VAR value : INT; END_VAR\nvalue := 1;\nEND_PROGRAM\n";
  struct resolver_state ok = {{0}, 0, 0};
  struct resolver_state bad = {{0}, 0, 1};
  matiec_context_t *contexts[2];
  matiec_result_t results[2] = {MATIEC_RESULT_INIT, MATIEC_RESULT_INIT};
  matiec_context_t *context;

  assert(library != NULL);
  contexts[0] = new_context(library, &ok, source);
  contexts[1] = new_context(library, &bad, source);
  assert(matiec_compile_batch(contexts, 2, 2, results) == MATIEC_STATUS_OK);
  assert(results[0].succeeded == 1u);
  assert(results[1].succeeded == 0u);
  assert(ok.calls >= 2u && bad.calls >= 2u);
  matiec_context_destroy(contexts[0]);
  matiec_context_destroy(contexts[1]);

  ok = (struct resolver_state){{0}, 0, 0};
  context = new_context(library, &ok, missing);
  results[0] = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &results[0]) == MATIEC_STATUS_OK);
  assert(results[0].succeeded == 0u);
  assert(matiec_context_diagnostic_count(context) > 0u);
  matiec_context_destroy(context);

  ok = (struct resolver_state){{0}, 0, 0};
  context = new_context(library, &ok, plain_source);
  assert(matiec_context_set_include_resolver(context, NULL, NULL) ==
         MATIEC_STATUS_OK);
  results[0] = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &results[0]) == MATIEC_STATUS_OK);
  assert(results[0].succeeded == 1u);
  assert(ok.calls == 0u);
  matiec_context_destroy(context);

  return 0;
}

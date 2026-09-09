#include "include/matiec/api.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static const char valid_source[] =
    "PROGRAM LimitsMain\n"
    "VAR value : INT; END_VAR\n"
    "value := 42;\n"
    "END_PROGRAM\n";

struct async_state {
  pthread_mutex_t mutex;
  pthread_cond_t condition;
  int resolver_entered;
  int resume_resolver;
  matiec_context_t *context;
  matiec_result_t result;
};

static matiec_context_t *new_context(const char *library, int syntax_only) {
  matiec_context_t *context = NULL;
  assert(matiec_context_create(&context) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_directory(context, library) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_syntax_only(context, syntax_only) ==
         MATIEC_STATUS_OK);
  return context;
}

static int has_diagnostic(matiec_context_t *context, const char *fragment) {
  size_t index;
  for (index = 0; index < matiec_context_diagnostic_count(context); ++index) {
    matiec_diagnostic_t diagnostic = MATIEC_DIAGNOSTIC_INIT;
    assert(matiec_context_get_diagnostic(context, index, &diagnostic) ==
           MATIEC_STATUS_OK);
    if (strstr(diagnostic.message, fragment) != NULL) return 1;
  }
  return 0;
}

static matiec_include_result_t blocking_resolver(
    void *user_data, const char *requested, matiec_source_view_t *source) {
  static const char included[] = "(* cancelled include *)\n";
  struct async_state *state = (struct async_state *)user_data;
  if (strcmp(requested, "slow.st") != 0)
    return MATIEC_INCLUDE_USE_FILESYSTEM;
  pthread_mutex_lock(&state->mutex);
  state->resolver_entered = 1;
  pthread_cond_signal(&state->condition);
  while (!state->resume_resolver)
    pthread_cond_wait(&state->condition, &state->mutex);
  pthread_mutex_unlock(&state->mutex);
  source->display_name = "memory://slow.st";
  source->data = included;
  source->size = strlen(included);
  return MATIEC_INCLUDE_RESOLVED;
}

static matiec_include_result_t oversized_resolver(
    void *user_data, const char *requested, matiec_source_view_t *source) {
  static const char included[] =
      "(* deliberately larger than the primary source byte budget: "
      "012345678901234567890123456789012345678901234567890123456789 *)\n";
  (void)user_data;
  if (strcmp(requested, "slow.st") != 0)
    return MATIEC_INCLUDE_USE_FILESYSTEM;
  source->display_name = "memory://large.st";
  source->data = included;
  source->size = strlen(included);
  return MATIEC_INCLUDE_RESOLVED;
}

static void *compile_in_thread(void *argument) {
  struct async_state *state = (struct async_state *)argument;
  assert(matiec_context_compile(state->context, &state->result) ==
         MATIEC_STATUS_OK);
  return NULL;
}

static int accept_output(void *user_data, const char *path,
                         const void *data, size_t size) {
  (void)user_data;
  (void)path;
  (void)data;
  (void)size;
  return 1;
}

int main(void) {
  const char *library = getenv("MATIEC_LIBDIR");
  const char including[] =
      "{#include \"slow.st\"}\nPROGRAM Main\nEND_PROGRAM\n";
  matiec_limits_t limits = MATIEC_LIMITS_INIT;
  matiec_result_t result = MATIEC_RESULT_INIT;
  matiec_context_t *context;
  pthread_t worker;
  struct async_state async = {
      PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, NULL,
      MATIEC_RESULT_INIT};

  assert(library != NULL);

  context = new_context(library, 1);
  assert(matiec_context_set_source(context, "memory://cancel.st", valid_source,
                                   strlen(valid_source)) == MATIEC_STATUS_OK);
  assert(matiec_context_cancel(context) == MATIEC_STATUS_OK);
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u && has_diagnostic(context, "cancelled"));
  assert(matiec_context_reset_cancel(context) == MATIEC_STATUS_OK);
  result = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 1u);
  matiec_context_destroy(context);

  async.context = new_context(library, 1);
  assert(matiec_context_set_source(async.context, "memory://async.st",
                                   including, strlen(including)) ==
         MATIEC_STATUS_OK);
  assert(matiec_context_set_include_resolver(
             async.context, blocking_resolver, &async) == MATIEC_STATUS_OK);
  assert(pthread_create(&worker, NULL, compile_in_thread, &async) == 0);
  pthread_mutex_lock(&async.mutex);
  while (!async.resolver_entered)
    pthread_cond_wait(&async.condition, &async.mutex);
  assert(matiec_context_cancel(async.context) == MATIEC_STATUS_OK);
  async.resume_resolver = 1;
  pthread_cond_signal(&async.condition);
  pthread_mutex_unlock(&async.mutex);
  assert(pthread_join(worker, NULL) == 0);
  assert(async.result.succeeded == 0u);
  assert(has_diagnostic(async.context, "cancelled"));
  matiec_context_destroy(async.context);
  pthread_cond_destroy(&async.condition);
  pthread_mutex_destroy(&async.mutex);

  context = new_context(library, 1);
  limits.max_source_bytes = 4;
  assert(matiec_context_set_limits(context, &limits) == MATIEC_STATUS_OK);
  assert(matiec_context_set_source(context, "memory://large.st", valid_source,
                                   strlen(valid_source)) == MATIEC_STATUS_OK);
  result = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u && has_diagnostic(context, "Source byte"));
  matiec_context_destroy(context);

  context = new_context(library, 1);
  assert(matiec_context_set_source(context, "memory://include.st", including,
                                   strlen(including)) == MATIEC_STATUS_OK);
  assert(matiec_context_set_include_resolver(
             context, oversized_resolver, NULL) == MATIEC_STATUS_OK);
  limits.max_source_bytes = strlen(including);
  assert(matiec_context_set_limits(context, &limits) == MATIEC_STATUS_OK);
  result = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u && has_diagnostic(context, "Source byte"));
  matiec_context_destroy(context);

  context = new_context(library, 0);
  assert(matiec_context_set_source(context, "memory://output.st", valid_source,
                                   strlen(valid_source)) == MATIEC_STATUS_OK);
  assert(matiec_context_set_output_callback(context, accept_output, NULL) ==
         MATIEC_STATUS_OK);
  limits = (matiec_limits_t)MATIEC_LIMITS_INIT;
  limits.max_output_bytes = 1;
  assert(matiec_context_set_limits(context, &limits) == MATIEC_STATUS_OK);
  result = (matiec_result_t)MATIEC_RESULT_INIT;
  assert(matiec_context_compile(context, &result) == MATIEC_STATUS_OK);
  assert(result.succeeded == 0u && has_diagnostic(context, "output byte"));
  matiec_context_destroy(context);

  {
    matiec_context_t *batch[2];
    matiec_result_t results[2] = {MATIEC_RESULT_INIT, MATIEC_RESULT_INIT};
    batch[0] = new_context(library, 1);
    batch[1] = new_context(library, 1);
    assert(matiec_context_set_source(batch[0], "memory://bounded.st",
                                     valid_source, strlen(valid_source)) ==
           MATIEC_STATUS_OK);
    assert(matiec_context_set_source(batch[1], "memory://normal.st",
                                     valid_source, strlen(valid_source)) ==
           MATIEC_STATUS_OK);
    limits = (matiec_limits_t)MATIEC_LIMITS_INIT;
    limits.max_source_bytes = 1;
    limits.max_diagnostics = 1;
    assert(matiec_context_set_limits(batch[0], &limits) == MATIEC_STATUS_OK);
    assert(matiec_compile_batch(batch, 2, 2, results) == MATIEC_STATUS_OK);
    assert(results[0].succeeded == 0u);
    assert(matiec_context_diagnostic_count(batch[0]) <= 1u);
    assert(results[1].succeeded == 1u);
    matiec_context_destroy(batch[0]);
    matiec_context_destroy(batch[1]);
  }
  return 0;
}

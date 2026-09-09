#include <matiec/api.h>

static uint32_t (*api_version_fn)(void) = &matiec_api_version;
static const char *(*api_version_string_fn)(void) =
    &matiec_api_version_string;
static matiec_status_t (*context_create_fn)(matiec_context_t **) =
    &matiec_context_create;
static void (*context_destroy_fn)(matiec_context_t *) =
    &matiec_context_destroy;
static matiec_status_t (*set_source_fn)(matiec_context_t *, const char *,
                                        const void *, size_t) =
    &matiec_context_set_source;
static matiec_status_t (*set_source_path_fn)(matiec_context_t *, const char *) =
    &matiec_context_set_source_path;
static matiec_status_t (*set_include_fn)(matiec_context_t *, const char *) =
    &matiec_context_set_include_directory;
static matiec_status_t (*set_output_directory_fn)(matiec_context_t *,
                                                   const char *) =
    &matiec_context_set_output_directory;
static matiec_status_t (*set_syntax_only_fn)(matiec_context_t *, int) =
    &matiec_context_set_syntax_only;
static matiec_status_t (*set_profile_fn)(matiec_context_t *,
                                         matiec_language_profile_t) =
    &matiec_context_set_language_profile;
static matiec_status_t (*set_language_fn)(matiec_context_t *,
                                          matiec_output_language_t) =
    &matiec_context_set_output_language;
static matiec_status_t (*compile_fn)(matiec_context_t *, matiec_result_t *) =
    &matiec_context_compile;
static size_t (*diagnostic_count_fn)(const matiec_context_t *) =
    &matiec_context_diagnostic_count;
static matiec_status_t (*get_diagnostic_fn)(matiec_context_t *, size_t,
                                            matiec_diagnostic_t *) =
    &matiec_context_get_diagnostic;
static matiec_status_t (*set_diagnostic_callback_fn)(
    matiec_context_t *, matiec_diagnostic_callback_t, void *) =
    &matiec_context_set_diagnostic_callback;
static matiec_status_t (*set_output_callback_fn)(
    matiec_context_t *, matiec_output_callback_t, void *) =
    &matiec_context_set_output_callback;
static matiec_status_t (*set_include_resolver_fn)(
    matiec_context_t *, matiec_include_resolver_callback_t, void *) =
    &matiec_context_set_include_resolver;
static matiec_status_t (*set_limits_fn)(matiec_context_t *,
                                        const matiec_limits_t *) =
    &matiec_context_set_limits;
static matiec_status_t (*cancel_fn)(matiec_context_t *) =
    &matiec_context_cancel;
static matiec_status_t (*reset_cancel_fn)(matiec_context_t *) =
    &matiec_context_reset_cancel;
static matiec_status_t (*compile_batch_fn)(matiec_context_t *const *, size_t,
                                           size_t, matiec_result_t *) =
    &matiec_compile_batch;
static const char *(*last_error_fn)(const matiec_context_t *) =
    &matiec_context_last_error;

int main(void) {
  matiec_result_t result = MATIEC_RESULT_INIT;
  matiec_diagnostic_t diagnostic = MATIEC_DIAGNOSTIC_INIT;
  matiec_source_view_t source = MATIEC_SOURCE_VIEW_INIT;
  matiec_limits_t limits = MATIEC_LIMITS_INIT;
  (void)api_version_fn;
  (void)api_version_string_fn;
  (void)context_create_fn;
  (void)context_destroy_fn;
  (void)set_source_fn;
  (void)set_source_path_fn;
  (void)set_include_fn;
  (void)set_output_directory_fn;
  (void)set_syntax_only_fn;
  (void)set_profile_fn;
  (void)set_language_fn;
  (void)compile_fn;
  (void)diagnostic_count_fn;
  (void)get_diagnostic_fn;
  (void)set_diagnostic_callback_fn;
  (void)set_output_callback_fn;
  (void)set_include_resolver_fn;
  (void)set_limits_fn;
  (void)cancel_fn;
  (void)reset_cancel_fn;
  (void)compile_batch_fn;
  (void)last_error_fn;
  return result.struct_size == sizeof(result) &&
                 diagnostic.struct_size == sizeof(diagnostic) &&
                 source.struct_size == sizeof(source) &&
                 limits.struct_size == sizeof(limits)
             ? 0
             : 1;
}

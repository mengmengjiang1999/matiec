#ifndef MATIEC_API_H
#define MATIEC_API_H

#include <stddef.h>
#include <stdint.h>

#define MATIEC_API_VERSION_MAJOR 1u
#define MATIEC_API_VERSION_MINOR 4u
#define MATIEC_API_VERSION_PATCH 0u

#define MATIEC_API_VERSION_ENCODE(major, minor, patch) \
  ((((uint32_t)(major)) << 24) | (((uint32_t)(minor)) << 16) | \
   ((uint32_t)(patch)))

#define MATIEC_API_VERSION \
  MATIEC_API_VERSION_ENCODE(MATIEC_API_VERSION_MAJOR, \
                            MATIEC_API_VERSION_MINOR, \
                            MATIEC_API_VERSION_PATCH)

#if defined(_WIN32) && defined(MATIEC_BUILD_SHARED)
# if defined(MATIEC_BUILDING_LIBRARY)
#  define MATIEC_API __declspec(dllexport)
# else
#  define MATIEC_API __declspec(dllimport)
# endif
#elif defined(__GNUC__) || defined(__clang__)
# define MATIEC_API __attribute__((visibility("default")))
#else
# define MATIEC_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct matiec_context matiec_context_t;

typedef enum matiec_status {
  MATIEC_STATUS_OK = 0,
  MATIEC_STATUS_INVALID_ARGUMENT = 1,
  MATIEC_STATUS_OUT_OF_MEMORY = 2,
  MATIEC_STATUS_INTERNAL_ERROR = 3
} matiec_status_t;

typedef enum matiec_language_profile {
  MATIEC_PROFILE_LEGACY = 0,
  MATIEC_PROFILE_IEC61131_3_2025_EXPERIMENTAL = 1
} matiec_language_profile_t;

typedef enum matiec_output_language {
  MATIEC_OUTPUT_C = 0,
  MATIEC_OUTPUT_IEC = 1
} matiec_output_language_t;

typedef struct matiec_result {
  uint32_t struct_size;
  uint32_t succeeded;
  uint64_t error_count;
  uint64_t warning_count;
} matiec_result_t;

typedef enum matiec_diagnostic_severity {
  MATIEC_DIAGNOSTIC_NOTE = 0,
  MATIEC_DIAGNOSTIC_WARNING = 1,
  MATIEC_DIAGNOSTIC_ERROR = 2,
  MATIEC_DIAGNOSTIC_FATAL = 3
} matiec_diagnostic_severity_t;

typedef struct matiec_diagnostic {
  uint32_t struct_size;
  matiec_diagnostic_severity_t severity;
  const char *message;
  const char *file;
  uint64_t line;
  uint64_t column;
  uint64_t end_line;
  uint64_t end_column;
} matiec_diagnostic_t;

#define MATIEC_DIAGNOSTIC_INIT \
  { (uint32_t)sizeof(matiec_diagnostic_t), MATIEC_DIAGNOSTIC_ERROR, NULL, NULL, \
    0u, 0u, 0u, 0u }

typedef void (*matiec_diagnostic_callback_t)(
    void *user_data, const matiec_diagnostic_t *diagnostic);
typedef int (*matiec_output_callback_t)(
    void *user_data, const char *path, const void *data, size_t size);

typedef enum matiec_include_result {
  MATIEC_INCLUDE_NOT_FOUND = 0,
  MATIEC_INCLUDE_RESOLVED = 1,
  MATIEC_INCLUDE_USE_FILESYSTEM = 2,
  MATIEC_INCLUDE_ERROR = -1
} matiec_include_result_t;

typedef struct matiec_source_view {
  uint32_t struct_size;
  const char *display_name;
  const void *data;
  size_t size;
} matiec_source_view_t;

#define MATIEC_SOURCE_VIEW_INIT \
  { (uint32_t)sizeof(matiec_source_view_t), NULL, NULL, 0u }

typedef matiec_include_result_t (*matiec_include_resolver_callback_t)(
    void *user_data, const char *requested_name, matiec_source_view_t *source);

#define MATIEC_RESULT_INIT \
  { (uint32_t)sizeof(matiec_result_t), 0u, 0u, 0u }

MATIEC_API uint32_t matiec_api_version(void);
MATIEC_API const char *matiec_api_version_string(void);
MATIEC_API matiec_status_t matiec_context_create(matiec_context_t **context);
MATIEC_API void matiec_context_destroy(matiec_context_t *context);
MATIEC_API matiec_status_t matiec_context_set_source(
    matiec_context_t *context, const char *display_name,
    const void *source, size_t source_size);
MATIEC_API matiec_status_t matiec_context_set_source_path(
    matiec_context_t *context, const char *source_path);
MATIEC_API matiec_status_t matiec_context_set_include_directory(
    matiec_context_t *context, const char *include_directory);
MATIEC_API matiec_status_t matiec_context_set_output_directory(
    matiec_context_t *context, const char *output_directory);
MATIEC_API matiec_status_t matiec_context_set_syntax_only(
    matiec_context_t *context, int enabled);
MATIEC_API matiec_status_t matiec_context_set_language_profile(
    matiec_context_t *context, matiec_language_profile_t profile);
MATIEC_API matiec_status_t matiec_context_set_output_language(
    matiec_context_t *context, matiec_output_language_t language);
MATIEC_API matiec_status_t matiec_context_compile(
    matiec_context_t *context, matiec_result_t *result);
MATIEC_API size_t matiec_context_diagnostic_count(
    const matiec_context_t *context);
MATIEC_API matiec_status_t matiec_context_get_diagnostic(
    matiec_context_t *context, size_t index, matiec_diagnostic_t *diagnostic);
MATIEC_API matiec_status_t matiec_context_set_diagnostic_callback(
    matiec_context_t *context, matiec_diagnostic_callback_t callback,
    void *user_data);
MATIEC_API matiec_status_t matiec_context_set_output_callback(
    matiec_context_t *context, matiec_output_callback_t callback,
    void *user_data);
MATIEC_API matiec_status_t matiec_context_set_include_resolver(
    matiec_context_t *context, matiec_include_resolver_callback_t callback,
    void *user_data);
MATIEC_API matiec_status_t matiec_compile_batch(
    matiec_context_t *const *contexts, size_t context_count,
    size_t max_concurrency, matiec_result_t *results);
MATIEC_API const char *matiec_context_last_error(
    const matiec_context_t *context);

#ifdef __cplusplus
}
#endif

#endif

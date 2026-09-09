#ifndef MATIEC_API_H
#define MATIEC_API_H

#include <stddef.h>
#include <stdint.h>

#define MATIEC_API_VERSION_MAJOR 1u
#define MATIEC_API_VERSION_MINOR 1u
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
MATIEC_API const char *matiec_context_last_error(
    const matiec_context_t *context);

#ifdef __cplusplus
}
#endif

#endif

#ifndef MATIEC_API_H
#define MATIEC_API_H

#include <stdint.h>

#define MATIEC_API_VERSION_MAJOR 1u
#define MATIEC_API_VERSION_MINOR 0u
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

MATIEC_API uint32_t matiec_api_version(void);
MATIEC_API const char *matiec_api_version_string(void);

#ifdef __cplusplus
}
#endif

#endif

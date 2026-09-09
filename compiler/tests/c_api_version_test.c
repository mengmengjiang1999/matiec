#include "include/matiec/api.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

int main(void) {
  assert(MATIEC_API_VERSION_MAJOR == 1u);
  assert(MATIEC_API_VERSION_MINOR == 5u);
  assert(MATIEC_API_VERSION_PATCH == 0u);
  assert(matiec_api_version() == MATIEC_API_VERSION);
  assert(matiec_api_version() ==
         MATIEC_API_VERSION_ENCODE(1u, 5u, 0u));
  assert(strcmp(matiec_api_version_string(), "1.5.0") == 0);
  return 0;
}

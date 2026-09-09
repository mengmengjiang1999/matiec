#include "include/matiec/api.h"

extern "C" {

uint32_t matiec_api_version(void) { return MATIEC_API_VERSION; }

const char *matiec_api_version_string(void) { return "1.5.0"; }

}  // extern "C"

#ifndef MATIEC_STAGE3_SEMANTIC_DIAGNOSTIC_MACROS_HH
#define MATIEC_STAGE3_SEMANTIC_DIAGNOSTIC_MACROS_HH

#ifndef STAGE3_AFTER_ERROR
#define STAGE3_AFTER_ERROR ((void)0)
#endif

#define STAGE3_ERROR(error_level, symbol1, symbol2, ...) \
  do { \
    if (current_display_error_level >= (error_level)) { \
      diagnostics_.error((symbol1), (symbol2), __VA_ARGS__); \
      STAGE3_AFTER_ERROR; \
      error_count++; \
    } \
  } while (0)

#define STAGE3_WARNING(symbol1, symbol2, ...) \
  do { \
    diagnostics_.warning((symbol1), (symbol2), __VA_ARGS__); \
    warning_found = true; \
  } while (0)

#endif

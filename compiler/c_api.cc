#include "include/matiec/api.h"

#include "compiler/compilation_context.hh"
#include "compiler/compiler.hh"

#include <exception>
#include <new>
#include <string>

struct matiec_context {
  matiec::CompilationContext value;
  std::string last_error;
};

namespace {
matiec_status_t invalid(matiec_context_t *context, const char *message) {
  if (context != nullptr) context->last_error = message;
  return MATIEC_STATUS_INVALID_ARGUMENT;
}

template <typename Operation>
matiec_status_t guarded(matiec_context_t *context, Operation operation) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  try {
    context->last_error.clear();
    operation();
    return MATIEC_STATUS_OK;
  } catch (const std::bad_alloc &) {
    context->last_error = "Out of memory";
    return MATIEC_STATUS_OUT_OF_MEMORY;
  } catch (const std::exception &error) {
    context->last_error = error.what();
    return MATIEC_STATUS_INTERNAL_ERROR;
  } catch (...) {
    context->last_error = "Unknown internal error";
    return MATIEC_STATUS_INTERNAL_ERROR;
  }
}
}  // namespace

extern "C" {
matiec_status_t matiec_context_create(matiec_context_t **context) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  *context = nullptr;
  try {
    *context = new matiec_context;
    return MATIEC_STATUS_OK;
  } catch (const std::bad_alloc &) {
    return MATIEC_STATUS_OUT_OF_MEMORY;
  } catch (...) {
    return MATIEC_STATUS_INTERNAL_ERROR;
  }
}

void matiec_context_destroy(matiec_context_t *context) { delete context; }

matiec_status_t matiec_context_set_source(
    matiec_context_t *context, const char *display_name,
    const void *source, size_t source_size) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (display_name == nullptr || display_name[0] == '\0')
    return invalid(context, "Source display name is required");
  if (source == nullptr && source_size != 0)
    return invalid(context, "Source bytes are required for a non-empty source");
  return guarded(context, [&] {
    const char *bytes = static_cast<const char *>(source);
    context->value.set_source(
        display_name,
        std::string(bytes == nullptr ? "" : bytes, source_size));
  });
}

matiec_status_t matiec_context_set_source_path(
    matiec_context_t *context, const char *source_path) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (source_path == nullptr || source_path[0] == '\0')
    return invalid(context, "Source path is required");
  return guarded(context,
                 [&] { context->value.set_source_path(source_path); });
}

matiec_status_t matiec_context_set_include_directory(
    matiec_context_t *context, const char *include_directory) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (include_directory == nullptr)
    return invalid(context, "Include directory is required");
  return guarded(context, [&] {
    context->value.options().include_directory = include_directory;
  });
}

matiec_status_t matiec_context_set_output_directory(
    matiec_context_t *context, const char *output_directory) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (output_directory == nullptr)
    return invalid(context, "Output directory is required");
  return guarded(context, [&] {
    context->value.options().output_directory = output_directory;
  });
}

matiec_status_t matiec_context_set_syntax_only(
    matiec_context_t *context, int enabled) {
  return guarded(context, [&] {
    context->value.options().syntax_only = enabled != 0;
  });
}

matiec_status_t matiec_context_set_language_profile(
    matiec_context_t *context, matiec_language_profile_t profile) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (profile != MATIEC_PROFILE_LEGACY &&
      profile != MATIEC_PROFILE_IEC61131_3_2025_EXPERIMENTAL)
    return invalid(context, "Unknown language profile");
  return guarded(context, [&] {
    context->value.options().language_profile =
        profile == MATIEC_PROFILE_LEGACY
            ? matiec::LanguageProfile::legacy
            : matiec::LanguageProfile::iec61131_3_2025_experimental;
  });
}

matiec_status_t matiec_context_set_output_language(
    matiec_context_t *context, matiec_output_language_t language) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (language != MATIEC_OUTPUT_C && language != MATIEC_OUTPUT_IEC)
    return invalid(context, "Unknown output language");
  return guarded(context, [&] {
    context->value.options().output_language =
        language == MATIEC_OUTPUT_C ? matiec::OutputLanguage::c
                                    : matiec::OutputLanguage::iec;
  });
}

matiec_status_t matiec_context_compile(matiec_context_t *context,
                                       matiec_result_t *result) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (result == nullptr) return invalid(context, "Result is required");
  if (result->struct_size < sizeof(matiec_result_t))
    return invalid(context, "Result structure is too small");
  return guarded(context, [&] {
    context->value.diagnostics().clear();
    const matiec::CompilationResult compiled =
        matiec::Compiler().compile(context->value);
    result->succeeded = compiled.succeeded() ? 1u : 0u;
    result->error_count = compiled.error_count;
    result->warning_count = compiled.warning_count;
  });
}

const char *matiec_context_last_error(const matiec_context_t *context) {
  return context == nullptr ? nullptr : context->last_error.c_str();
}
}  // extern "C"

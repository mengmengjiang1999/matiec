#include "include/matiec/api.h"

#include "compiler/compilation_context.hh"
#include "compiler/compiler.hh"
#include "compiler/output_sink.hh"

#include <exception>
#include <functional>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <vector>

struct matiec_context {
  matiec::CompilationContext value;
  std::string last_error;
  matiec_diagnostic_callback_t diagnostic_callback = nullptr;
  void *diagnostic_user_data = nullptr;
  matiec_output_callback_t output_callback = nullptr;
  void *output_user_data = nullptr;
};

namespace {
class CallbackOutputSink final : public matiec::OutputSink {
 public:
  CallbackOutputSink(std::string path, matiec_output_callback_t callback,
                     void *user_data)
      : path_(std::move(path)), callback_(callback), user_data_(user_data) {}

  matiec::OutputResult write(std::string_view text) override {
    if (!good()) return matiec::OutputResult::failure(error_message_);
    delivered_ = true;
    return deliver(text.data(), text.size());
  }

  matiec::OutputResult flush() override {
    if (!good()) return matiec::OutputResult::failure(error_message_);
    return delivered_ ? matiec::OutputResult::success() : deliver(nullptr, 0);
  }

  bool good() const override { return error_message_.empty(); }
  const std::string &error_message() const override { return error_message_; }

 private:
  matiec::OutputResult deliver(const void *data, std::size_t size) {
    try {
      if (callback_(user_data_, path_.c_str(), data, size) != 0)
        return matiec::OutputResult::success();
      error_message_ = "Output callback rejected generated data for '" + path_ +
                       "'";
    } catch (...) {
      error_message_ = "Output callback failed for '" + path_ + "'";
    }
    return matiec::OutputResult::failure(error_message_);
  }

  std::string path_;
  matiec_output_callback_t callback_;
  void *user_data_;
  std::string error_message_;
  bool delivered_ = false;
};

matiec_diagnostic_severity_t convert_severity(
    matiec::DiagnosticSeverity severity) {
  switch (severity) {
    case matiec::DiagnosticSeverity::note:
      return MATIEC_DIAGNOSTIC_NOTE;
    case matiec::DiagnosticSeverity::warning:
      return MATIEC_DIAGNOSTIC_WARNING;
    case matiec::DiagnosticSeverity::fatal:
      return MATIEC_DIAGNOSTIC_FATAL;
    case matiec::DiagnosticSeverity::error:
    default:
      return MATIEC_DIAGNOSTIC_ERROR;
  }
}

void fill_diagnostic(const matiec::Diagnostic &source,
                     matiec_diagnostic_t *destination) {
  destination->severity = convert_severity(source.severity);
  destination->message = source.message.c_str();
  if (source.range.valid()) {
    destination->file = source.range.begin.file.c_str();
    destination->line = source.range.begin.line;
    destination->column = source.range.begin.column;
    destination->end_line = source.range.end.line;
    destination->end_column = source.range.end.column;
  } else {
    destination->file = nullptr;
    destination->line = 0;
    destination->column = 0;
    destination->end_line = 0;
    destination->end_column = 0;
  }
}

void deliver_diagnostics(matiec_context_t *context) {
  if (context->diagnostic_callback == nullptr) return;
  for (const matiec::Diagnostic &item :
       context->value.diagnostics().diagnostics()) {
    matiec_diagnostic_t view = MATIEC_DIAGNOSTIC_INIT;
    fill_diagnostic(item, &view);
    context->diagnostic_callback(context->diagnostic_user_data, &view);
  }
}

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
    deliver_diagnostics(context);
  });
}

size_t matiec_context_diagnostic_count(const matiec_context_t *context) {
  return context == nullptr ? 0 : context->value.diagnostics().diagnostics().size();
}

matiec_status_t matiec_context_get_diagnostic(
    matiec_context_t *context, size_t index, matiec_diagnostic_t *diagnostic) {
  if (context == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
  if (diagnostic == nullptr)
    return invalid(context, "Diagnostic view is required");
  if (diagnostic->struct_size < sizeof(matiec_diagnostic_t))
    return invalid(context, "Diagnostic structure is too small");
  const std::vector<matiec::Diagnostic> &items =
      context->value.diagnostics().diagnostics();
  if (index >= items.size())
    return invalid(context, "Diagnostic index is out of range");
  return guarded(context, [&] { fill_diagnostic(items[index], diagnostic); });
}

matiec_status_t matiec_context_set_diagnostic_callback(
    matiec_context_t *context, matiec_diagnostic_callback_t callback,
    void *user_data) {
  return guarded(context, [&] {
    context->diagnostic_callback = callback;
    context->diagnostic_user_data = user_data;
  });
}

matiec_status_t matiec_context_set_output_callback(
    matiec_context_t *context, matiec_output_callback_t callback,
    void *user_data) {
  return guarded(context, [&] {
    context->output_callback = callback;
    context->output_user_data = user_data;
    if (callback == nullptr) {
      context->value.outputs().set_file_sink_factory({});
      return;
    }
    context->value.outputs().set_file_sink_factory(
        [context](std::string path) -> std::unique_ptr<matiec::OutputSink> {
          return std::make_unique<CallbackOutputSink>(
              std::move(path), context->output_callback,
              context->output_user_data);
        });
  });
}

matiec_status_t matiec_compile_batch(
    matiec_context_t *const *contexts, size_t context_count,
    size_t max_concurrency, matiec_result_t *results) {
  if (context_count == 0) return MATIEC_STATUS_OK;
  if (contexts == nullptr || results == nullptr)
    return MATIEC_STATUS_INVALID_ARGUMENT;
  for (size_t index = 0; index < context_count; ++index) {
    if (contexts[index] == nullptr) return MATIEC_STATUS_INVALID_ARGUMENT;
    if (results[index].struct_size < sizeof(matiec_result_t)) {
      contexts[index]->last_error = "Result structure is too small";
      return MATIEC_STATUS_INVALID_ARGUMENT;
    }
  }

  try {
    std::vector<std::reference_wrapper<matiec::CompilationContext>> jobs;
    jobs.reserve(context_count);
    for (size_t index = 0; index < context_count; ++index) {
      contexts[index]->last_error.clear();
      contexts[index]->value.diagnostics().clear();
      jobs.push_back(std::ref(contexts[index]->value));
    }
    const std::vector<matiec::CompilationResult> compiled =
        matiec::Compiler().compile_parallel(jobs, max_concurrency);
    for (size_t index = 0; index < context_count; ++index) {
      results[index].succeeded = compiled[index].succeeded() ? 1u : 0u;
      results[index].error_count = compiled[index].error_count;
      results[index].warning_count = compiled[index].warning_count;
      deliver_diagnostics(contexts[index]);
    }
    return MATIEC_STATUS_OK;
  } catch (const std::bad_alloc &) {
    for (size_t index = 0; index < context_count; ++index)
      contexts[index]->last_error = "Out of memory";
    return MATIEC_STATUS_OUT_OF_MEMORY;
  } catch (const std::exception &error) {
    for (size_t index = 0; index < context_count; ++index)
      contexts[index]->last_error = error.what();
    return MATIEC_STATUS_INTERNAL_ERROR;
  } catch (...) {
    for (size_t index = 0; index < context_count; ++index)
      contexts[index]->last_error = "Unknown internal error";
    return MATIEC_STATUS_INTERNAL_ERROR;
  }
}

const char *matiec_context_last_error(const matiec_context_t *context) {
  return context == nullptr ? nullptr : context->last_error.c_str();
}
}  // extern "C"

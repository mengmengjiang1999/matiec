#ifndef MATIEC_COMPILER_LANGUAGE_FEATURE_CATALOG_HH
#define MATIEC_COMPILER_LANGUAGE_FEATURE_CATALOG_HH

#include <string>
#include <vector>

namespace matiec {

enum class LanguageFeatureFamily {
  lexer,
  datatype,
  pou,
  structured_text,
  instruction_list,
  sequential_function_chart,
  configuration,
  reference,
  extension
};

enum class LanguageFeatureStatus {
  implemented,
  partially_implemented,
  not_implemented,
  planned
};

enum class LanguageEvidenceLevel {
  implementation_verified,
  official_public,
  cross_vendor_provisional,
  unverified
};

struct LanguageFeatureRecord {
  const char *id;
  const char *name;
  LanguageFeatureFamily family;
  LanguageFeatureStatus status;
  const char *activation;
  LanguageEvidenceLevel implementation_evidence;
  LanguageEvidenceLevel standard_evidence;
  const char *evidence_references;
  const char *test_paths;
  const char *unresolved_questions;
  bool explicit_test_gap;
};

const char *language_feature_family_name(LanguageFeatureFamily family);
const char *language_feature_status_name(LanguageFeatureStatus status);
const char *language_evidence_level_name(LanguageEvidenceLevel level);

const std::vector<LanguageFeatureRecord> &language_feature_catalog();
std::vector<std::string> validate_language_feature_catalog(
    const std::vector<LanguageFeatureRecord> &catalog);

}  // namespace matiec

#endif

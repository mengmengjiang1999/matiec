#include "compiler/language_feature_catalog.hh"

#include <cassert>
#include <string>
#include <vector>

int main() {
  const std::vector<matiec::LanguageFeatureRecord> &catalog =
      matiec::language_feature_catalog();
  assert(!catalog.empty());
  assert(matiec::validate_language_feature_catalog(catalog).empty());

  matiec::LanguageFeatureRecord invalid = catalog.front();
  invalid.id = "";
  invalid.implementation_evidence =
      static_cast<matiec::LanguageEvidenceLevel>(999);
  std::vector<matiec::LanguageFeatureRecord> invalid_catalog{invalid};
  const std::vector<std::string> errors =
      matiec::validate_language_feature_catalog(invalid_catalog);
  assert(errors.size() == 2);

  invalid = catalog.front();
  invalid.test_paths = "";
  invalid.explicit_test_gap = false;
  invalid_catalog.assign(1, invalid);
  assert(matiec::validate_language_feature_catalog(invalid_catalog).size() == 1);

  assert(std::string(matiec::language_feature_family_name(
             matiec::LanguageFeatureFamily::instruction_list)) ==
         "instruction-list");
  assert(std::string(matiec::language_feature_status_name(
             matiec::LanguageFeatureStatus::planned)) == "planned");
  assert(std::string(matiec::language_evidence_level_name(
             matiec::LanguageEvidenceLevel::official_public)) ==
         "official-public");
  return 0;
}

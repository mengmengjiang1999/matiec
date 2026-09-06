#include "compiler/language_feature_catalog.hh"

#include <set>

namespace matiec {
namespace {

bool present(const char *value) {
  return value != nullptr && value[0] != '\0';
}

bool valid_family(LanguageFeatureFamily value) {
  switch (value) {
    case LanguageFeatureFamily::lexer:
    case LanguageFeatureFamily::datatype:
    case LanguageFeatureFamily::pou:
    case LanguageFeatureFamily::structured_text:
    case LanguageFeatureFamily::instruction_list:
    case LanguageFeatureFamily::sequential_function_chart:
    case LanguageFeatureFamily::configuration:
    case LanguageFeatureFamily::reference:
    case LanguageFeatureFamily::extension:
      return true;
  }
  return false;
}

bool valid_status(LanguageFeatureStatus value) {
  switch (value) {
    case LanguageFeatureStatus::implemented:
    case LanguageFeatureStatus::partially_implemented:
    case LanguageFeatureStatus::not_implemented:
    case LanguageFeatureStatus::planned:
      return true;
  }
  return false;
}

bool valid_evidence(LanguageEvidenceLevel value) {
  switch (value) {
    case LanguageEvidenceLevel::implementation_verified:
    case LanguageEvidenceLevel::official_public:
    case LanguageEvidenceLevel::cross_vendor_provisional:
    case LanguageEvidenceLevel::unverified:
      return true;
  }
  return false;
}

}  // namespace

const char *language_feature_family_name(LanguageFeatureFamily family) {
  switch (family) {
    case LanguageFeatureFamily::lexer: return "lexer";
    case LanguageFeatureFamily::datatype: return "datatype";
    case LanguageFeatureFamily::pou: return "pou";
    case LanguageFeatureFamily::structured_text: return "structured-text";
    case LanguageFeatureFamily::instruction_list: return "instruction-list";
    case LanguageFeatureFamily::sequential_function_chart: return "sfc";
    case LanguageFeatureFamily::configuration: return "configuration";
    case LanguageFeatureFamily::reference: return "reference";
    case LanguageFeatureFamily::extension: return "extension";
  }
  return "unknown";
}

const char *language_feature_status_name(LanguageFeatureStatus status) {
  switch (status) {
    case LanguageFeatureStatus::implemented: return "implemented";
    case LanguageFeatureStatus::partially_implemented: return "partially-implemented";
    case LanguageFeatureStatus::not_implemented: return "not-implemented";
    case LanguageFeatureStatus::planned: return "planned";
  }
  return "unknown";
}

const char *language_evidence_level_name(LanguageEvidenceLevel level) {
  switch (level) {
    case LanguageEvidenceLevel::implementation_verified: return "implementation-verified";
    case LanguageEvidenceLevel::official_public: return "official-public";
    case LanguageEvidenceLevel::cross_vendor_provisional: return "cross-vendor-provisional";
    case LanguageEvidenceLevel::unverified: return "unverified";
  }
  return "unknown";
}

const std::vector<LanguageFeatureRecord> &language_feature_catalog() {
  using Family = LanguageFeatureFamily;
  using Status = LanguageFeatureStatus;
  using Evidence = LanguageEvidenceLevel;
  static const std::vector<LanguageFeatureRecord> catalog = {
    {"LEX-CASE-ASCII", "ASCII case-insensitive identifiers", Family::lexer, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_flex.ll", "tests/syntax/identifier/runtests", "Exact 2025 identifier rules are unverified", false},
    {"LEX-COMMENT", "IEC block comments", Family::lexer, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_flex.ll", "tests/syntax/identifier/runtests", "Exact 2025 rules are unverified", false},
    {"LEX-NESTED-COMMENT", "Nested block comments", Family::extension, Status::implemented, "-n", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_flex.ll; main.cc", "tests/cli/runtests", "Exact 2025 rules are unverified", false},
    {"LEX-INCLUDE-PRAGMA", "MATIEC include pragma", Family::extension, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_flex.ll", "tests/initialization/runtests", "MATIEC extension; not attributed to IEC", false},
    {"TYPE-ELEMENTARY-V2", "Legacy elementary datatypes", Family::datatype, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy; lib", "tests/initialization/runtests; tests/generated_c/runtests", "2025 deltas are unverified", false},
    {"TYPE-DERIVED", "Aliases, ranges, enums, arrays and structures", Family::datatype, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy", "tests/initialization/runtests", "2025 deltas are unverified", false},
    {"TYPE-SAFE", "PLCopen SAFE datatypes", Family::extension, Status::implemented, "-s", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy; main.cc", "", "No focused maintained test", true},
    {"TYPE-REFERENCE", "REF_TO, REF, DREF, dereference and NULL", Family::reference, Status::partially_implemented, "-r; experimental profile for declaration initialization", Evidence::implementation_verified, Evidence::cross_vendor_provisional, "stage1_2/iec_bison.yy; stage3; stage4/generate_c; public vendor documentation", "tests/cli/profile-fixtures/reference*.st; tests/cli/runtests", "Normative 2025 nullability and lifetime rules are unverified; no flow-sensitive lifetime analysis", false},
    {"TYPE-REFERENCE-EXT", "Non-standard broad reference forms", Family::extension, Status::implemented, "-R", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy; main.cc", "tests/cli/profile-fixtures/reference-broad-extension.st; tests/cli/runtests", "MATIEC extension; not attributed to IEC", false},
    {"POU-FUNCTION", "Functions", Family::pou, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy", "tests/syntax/identifier/identifier_as_function1.test", "2025 deltas are unverified", false},
    {"POU-FB", "Function blocks and instances", Family::pou, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy", "tests/initialization/ok_function_block_instance.st", "2025 deltas are unverified", false},
    {"POU-PROGRAM", "Programs", Family::pou, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy", "tests/initialization/ok_array_scalar.st", "2025 deltas are unverified", false},
    {"LANG-ST", "Structured Text", Family::structured_text, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::official_public, "stage1_2/iec_bison.yy; IEC public 2025 product page", "tests/initialization/ok_array_scalar.st", "Edition deltas are not fully known", false},
    {"LANG-IL", "Instruction List", Family::instruction_list, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::unverified, "stage1_2/iec_bison.yy", "tests/characterization/instruction_list.il", "Precise 2025 Annex B status is unverified", false},
    {"LANG-SFC-TEXT", "Textual Sequential Function Chart", Family::sequential_function_chart, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::official_public, "stage1_2/iec_bison.yy; IEC public 2025 product page", "tests/syntax/sfc/sfc.txt", "Detailed 2025 deltas are unverified", false},
    {"CONFIG-RESOURCE-TASK", "Configurations, resources and tasks", Family::configuration, Status::implemented, "legacy default", Evidence::implementation_verified, Evidence::official_public, "stage1_2/iec_bison.yy; IEC public 2025 product page", "tests/syntax/configuration/configuration.txt", "Detailed 2025 deltas are unverified", false},
    {"CONFIG-VAR-ACCESS", "Access variables", Family::configuration, Status::partially_implemented, "iec61131-3:2025-experimental", Evidence::implementation_verified, Evidence::cross_vendor_provisional, "compiler/access_variable_normalizer.cc; public Beckhoff and OPC Foundation material", "compiler/tests/access_variable_normalizer_test.cc; tests/cli/runtests", "Only same-configuration simple VAR_GLOBAL paths and ACCESS.csv export are implemented; hierarchical paths, direct addresses, structured elements, and normative 2025 behavior remain unverified", false},
    {"ED3-NAMESPACE", "Namespaces and qualified names", Family::pou, Status::partially_implemented, "iec61131-3:2025-experimental", Evidence::implementation_verified, Evidence::cross_vendor_provisional, "compiler/namespace_normalizer.cc; public vendor material", "compiler/tests/namespace_normalizer_test.cc; tests/cli/runtests", "MATIEC provisional subset only; normative 2025 lookup, visibility, aliases, reopening, shadowing, and ABI rules are unverified", false},
    {"ED3-OO", "Object-oriented elements", Family::pou, Status::partially_implemented, "iec61131-3:2025-experimental", Evidence::implementation_verified, Evidence::cross_vendor_provisional, "compiler/object_method_normalizer.cc; public Edition 3 vendor material", "compiler/tests/object_method_normalizer_test.cc; tests/cli/runtests", "Only public function-block methods with static dispatch are implemented; classes, interfaces, inheritance, overrides, properties, and normative 2025 behavior remain unverified", false},
    {"ED4-UTF8", "UTF-8 source and STRING literals", Family::datatype, Status::partially_implemented, "iec61131-3:2025-experimental", Evidence::implementation_verified, Evidence::official_public, "compiler/utf8_validation.cc; stage1_2/iec_flex.ll; IEC public 2025 product page", "compiler/tests/utf8_validation_test.cc; tests/cli/runtests", "Normative indexing, normalization, WSTRING representation, and associated function signatures are unverified", false},
    {"ED4-ASSERT", "ASSERT(IN : BOOL) : VOID", Family::pou, Status::partially_implemented, "iec61131-3:2025-experimental", Evidence::implementation_verified, Evidence::cross_vendor_provisional, "compiler/modern_library_normalizer.cc; IEC public 2025 preview; public specialist analysis", "compiler/tests/modern_library_normalizer_test.cc; tests/cli/runtests", "Standalone single-line calls only; released/no-op runtime policy; development notification and normative ABI remain unverified", false}
  };
  return catalog;
}

std::vector<std::string> validate_language_feature_catalog(
    const std::vector<LanguageFeatureRecord> &catalog) {
  std::vector<std::string> errors;
  std::set<std::string> ids;
  for (std::size_t index = 0; index < catalog.size(); ++index) {
    const LanguageFeatureRecord &entry = catalog[index];
    const std::string label = present(entry.id) ? entry.id : "entry " + std::to_string(index);
    if (!present(entry.id)) errors.push_back(label + ": missing id");
    else if (!ids.insert(entry.id).second) errors.push_back(label + ": duplicate id");
    if (!present(entry.name)) errors.push_back(label + ": missing name");
    if (!valid_family(entry.family)) errors.push_back(label + ": unknown family");
    if (!valid_status(entry.status)) errors.push_back(label + ": unknown status");
    if (!present(entry.activation)) errors.push_back(label + ": missing activation");
    if (!valid_evidence(entry.implementation_evidence)) errors.push_back(label + ": unknown implementation evidence");
    if (!valid_evidence(entry.standard_evidence)) errors.push_back(label + ": unknown standard evidence");
    if (!present(entry.evidence_references)) errors.push_back(label + ": missing evidence references");
    if (!present(entry.unresolved_questions)) errors.push_back(label + ": missing unresolved questions");
    if ((entry.status == LanguageFeatureStatus::implemented ||
         entry.status == LanguageFeatureStatus::partially_implemented) &&
        !present(entry.test_paths) && !entry.explicit_test_gap) {
      errors.push_back(label + ": missing tests or explicit coverage gap");
    }
  }
  return errors;
}

}  // namespace matiec

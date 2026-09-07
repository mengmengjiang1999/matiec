#ifndef MATIEC_COMPILER_SOURCE_MANAGER_HH
#define MATIEC_COMPILER_SOURCE_MANAGER_HH

#include <string>

namespace matiec {

class SourceManager {
 public:
  void set_file(std::string path);
  void set_memory(std::string display_name, std::string bytes);

  bool has_source() const;
  bool is_memory_backed() const;
  const std::string &display_name() const;
  const std::string &bytes() const;

  bool load(std::string *contents, std::string *error) const;

 private:
  std::string display_name_;
  std::string bytes_;
  bool memory_backed_ = false;
};

}  // namespace matiec

#endif

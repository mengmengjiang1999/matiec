#include "compiler/source_manager.hh"

#include <fstream>
#include <sstream>
#include <utility>

namespace matiec {

void SourceManager::set_file(std::string path) {
  display_name_ = std::move(path);
  bytes_.clear();
  memory_backed_ = false;
}

void SourceManager::set_memory(std::string display_name, std::string bytes) {
  display_name_ = std::move(display_name);
  bytes_ = std::move(bytes);
  memory_backed_ = true;
}

bool SourceManager::has_source() const { return !display_name_.empty(); }

bool SourceManager::is_memory_backed() const { return memory_backed_; }

const std::string &SourceManager::display_name() const { return display_name_; }

const std::string &SourceManager::bytes() const { return bytes_; }

bool SourceManager::load(std::string *contents, std::string *error) const {
  if (contents == nullptr || error == nullptr) return false;
  if (!has_source()) {
    *error = "no source was provided";
    return false;
  }
  if (memory_backed_) {
    *contents = bytes_;
    error->clear();
    return true;
  }
  std::ifstream input(display_name_, std::ios::binary);
  if (!input) {
    *error = "cannot open source file";
    return false;
  }
  std::ostringstream buffer;
  buffer << input.rdbuf();
  if (!input.good() && !input.eof()) {
    *error = "cannot read source file";
    return false;
  }
  *contents = buffer.str();
  error->clear();
  return true;
}

}  // namespace matiec

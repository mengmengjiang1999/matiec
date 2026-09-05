#include "compiler/ast_arena.hh"

#include <cstring>
#include <memory>

namespace matiec {

AstArena::~AstArena() {
  clear();
}

const char *AstArena::copy_string(const char *value) {
  if (value == nullptr) return nullptr;
  return copy_string(value, std::strlen(value));
}

const char *AstArena::copy_string(const char *value, std::size_t length) {
  if (value == nullptr) return nullptr;

  std::unique_ptr<char[]> copy(new char[length + 1]);
  std::memcpy(copy.get(), value, length);
  copy[length] = '\0';

  char *result = copy.get();
  allocations_.push_back(Allocation{result, &destroy_string});
  copy.release();
  return result;
}

std::size_t AstArena::allocation_count() const {
  return allocations_.size();
}

void AstArena::clear() {
  for (std::vector<Allocation>::reverse_iterator allocation = allocations_.rbegin();
       allocation != allocations_.rend(); ++allocation) {
    allocation->destroy(allocation->address);
  }
  allocations_.clear();
}

void AstArena::destroy_string(void *address) {
  delete[] static_cast<char *>(address);
}

}  // namespace matiec

#include "compiler/ast_arena.hh"

#include <cstring>
#include <cstdlib>
#include <memory>

namespace matiec {

namespace {

thread_local AstArena *current_ast_arena = nullptr;

}  // namespace

AstArena::~AstArena() {
  clear();
}

char *AstArena::copy_string(const char *value) {
  if (value == nullptr) return nullptr;
  return copy_string(value, std::strlen(value));
}

char *AstArena::copy_string(const char *value, std::size_t length) {
  if (value == nullptr) return nullptr;

  std::unique_ptr<char[]> copy(new char[length + 1]);
  std::memcpy(copy.get(), value, length);
  copy[length] = '\0';

  char *result = copy.get();
  adopt(result, &destroy_string);
  copy.release();
  return result;
}

void AstArena::adopt(void *address, void (*destroy)(void *)) {
  const std::pair<std::unordered_set<void *>::iterator, bool> inserted =
      addresses_.insert(address);
  if (!inserted.second) return;
  try {
    allocations_.push_back(Allocation{address, destroy});
  } catch (...) {
    addresses_.erase(address);
    throw;
  }
}

void AstArena::release(void *address) {
  if (clearing_) return;
  if (addresses_.erase(address) == 0) return;
  for (std::vector<Allocation>::iterator allocation = allocations_.begin();
       allocation != allocations_.end(); ++allocation) {
    if (allocation->address == address) {
      allocations_.erase(allocation);
      return;
    }
  }
}

std::size_t AstArena::allocation_count() const {
  return allocations_.size();
}

void AstArena::clear() {
  clearing_ = true;
  while (!allocations_.empty()) {
    const Allocation allocation = allocations_.back();
    allocations_.pop_back();
    allocation.destroy(allocation.address);
  }
  addresses_.clear();
  clearing_ = false;
}

void AstArena::destroy_string(void *address) {
  delete[] static_cast<char *>(address);
}

ActiveAstArenaScope::ActiveAstArenaScope(AstArena &arena)
    : previous_(current_ast_arena) {
  current_ast_arena = &arena;
}

ActiveAstArenaScope::~ActiveAstArenaScope() {
  current_ast_arena = previous_;
}

AstArena *active_ast_arena() {
  return current_ast_arena;
}

char *retain_ast_string(const char *value) {
  if (value == nullptr) return nullptr;
  if (current_ast_arena != nullptr) return current_ast_arena->copy_string(value);

  const std::size_t length = std::strlen(value);
  char *copy = static_cast<char *>(std::malloc(length + 1));
  if (copy != nullptr) std::memcpy(copy, value, length + 1);
  return copy;
}

}  // namespace matiec

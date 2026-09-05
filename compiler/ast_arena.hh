#ifndef MATIEC_COMPILER_AST_ARENA_HH
#define MATIEC_COMPILER_AST_ARENA_HH

#include <cstddef>
#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

namespace matiec {

class AstArena {
 public:
  AstArena() = default;
  ~AstArena();

  AstArena(const AstArena &) = delete;
  AstArena &operator=(const AstArena &) = delete;
  AstArena(AstArena &&) = delete;
  AstArena &operator=(AstArena &&) = delete;

  template <typename T, typename... Args>
  T *make(Args &&...args) {
    std::unique_ptr<T> object(new T(std::forward<Args>(args)...));
    T *result = object.get();
    adopt(result, &destroy<T>);
    object.release();
    return result;
  }

  char *copy_string(const char *value);
  char *copy_string(const char *value, std::size_t length);

  void adopt(void *address, void (*destroy)(void *));
  void release(void *address);

  std::size_t allocation_count() const;
  void clear();

 private:
  struct Allocation {
    void *address;
    void (*destroy)(void *);
  };

  template <typename T>
  static void destroy(void *address) {
    delete static_cast<T *>(address);
  }

  static void destroy_string(void *address);

  std::vector<Allocation> allocations_;
  std::unordered_set<void *> addresses_;
  bool clearing_ = false;
};

class ActiveAstArenaScope {
 public:
  explicit ActiveAstArenaScope(AstArena &arena);
  ~ActiveAstArenaScope();

  ActiveAstArenaScope(const ActiveAstArenaScope &) = delete;
  ActiveAstArenaScope &operator=(const ActiveAstArenaScope &) = delete;

 private:
  AstArena *previous_;
};

AstArena *active_ast_arena();
char *retain_ast_string(const char *value);

}  // namespace matiec

#endif

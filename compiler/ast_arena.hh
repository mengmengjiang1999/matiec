#ifndef MATIEC_COMPILER_AST_ARENA_HH
#define MATIEC_COMPILER_AST_ARENA_HH

#include <cstddef>
#include <memory>
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
    allocations_.push_back(Allocation{result, &destroy<T>});
    object.release();
    return result;
  }

  const char *copy_string(const char *value);
  const char *copy_string(const char *value, std::size_t length);

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
};

}  // namespace matiec

#endif

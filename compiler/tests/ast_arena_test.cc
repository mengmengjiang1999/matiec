#include "compiler/ast_arena.hh"
#include "compiler/compilation_context.hh"

#include <cassert>
#include <cstring>

namespace {

struct LifetimeProbe {
  LifetimeProbe(int identifier, int *destruction_order, int *destruction_count)
      : identifier(identifier),
        destruction_order(destruction_order),
        destruction_count(destruction_count) {}

  virtual ~LifetimeProbe() {
    destruction_order[*destruction_count] = identifier;
    ++*destruction_count;
  }

  int identifier;
  int *destruction_order;
  int *destruction_count;
};

struct DerivedProbe : public LifetimeProbe {
  DerivedProbe(int identifier, int *destruction_order, int *destruction_count,
               bool *derived_destroyed)
      : LifetimeProbe(identifier, destruction_order, destruction_count),
        derived_destroyed(derived_destroyed) {}

  ~DerivedProbe() override { *derived_destroyed = true; }

  bool *derived_destroyed;
};

}  // namespace

int main() {
  int destruction_order[2] = {0, 0};
  int destruction_count = 0;
  bool derived_destroyed = false;

  {
    matiec::CompilationContext context;
    LifetimeProbe *first = context.ast_arena().make<LifetimeProbe>(
        1, destruction_order, &destruction_count);
    LifetimeProbe *second = context.ast_arena().make<DerivedProbe>(
        2, destruction_order, &destruction_count, &derived_destroyed);
    const char *token = context.ast_arena().copy_string("stable-token");
    const char *prefix = context.ast_arena().copy_string("prefix-suffix", 6);

    assert(first->identifier == 1);
    assert(second->identifier == 2);
    assert(std::strcmp(token, "stable-token") == 0);
    assert(std::strcmp(prefix, "prefix") == 0);
    assert(context.ast_arena().allocation_count() == 4);
  }

  assert(derived_destroyed);
  assert(destruction_count == 2);
  assert(destruction_order[0] == 2);
  assert(destruction_order[1] == 1);

  matiec::CompilationContext first_context;
  matiec::CompilationContext second_context;
  first_context.ast_arena().make<LifetimeProbe>(
      3, destruction_order, &destruction_count);
  second_context.ast_arena().make<LifetimeProbe>(
      4, destruction_order, &destruction_count);
  first_context.ast_arena().clear();
  assert(destruction_count == 3);
  assert(second_context.ast_arena().allocation_count() == 1);
  return 0;
}

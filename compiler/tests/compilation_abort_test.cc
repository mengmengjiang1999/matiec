#include "compiler/compilation_abort.hh"
#include "main.hh"

#include <cassert>
#include <string>

int main() {
  try {
    error_exit("semantic.cc", 27, "invalid state %d", 4);
    assert(false);
  } catch (const matiec::CompilationAbort &abort) {
    assert(!abort.diagnostic_reported());
    assert(std::string(abort.what()) ==
           "Internal compiler error in file semantic.cc at line 27: invalid state 4");
  }
  return 0;
}

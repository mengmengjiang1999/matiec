#include "compiler/source_manager.hh"

#include <cassert>
#include <cstdio>
#include <string>

int main() {
  matiec::SourceManager source;
  assert(!source.has_source());

  source.set_memory("memory://main.st", "PROGRAM Main\nEND_PROGRAM\n");
  assert(source.has_source());
  assert(source.is_memory_backed());
  assert(source.display_name() == "memory://main.st");
  std::string contents;
  std::string error;
  assert(source.load(&contents, &error));
  assert(contents == "PROGRAM Main\nEND_PROGRAM\n");

  char path[] = "/tmp/matiec-source-manager-XXXXXX";
  const int descriptor = mkstemp(path);
  assert(descriptor >= 0);
  FILE *file = fdopen(descriptor, "wb");
  assert(file != nullptr);
  const std::string file_contents = "PROGRAM FileMain\nEND_PROGRAM\n";
  assert(std::fwrite(file_contents.data(), 1, file_contents.size(), file) ==
         file_contents.size());
  assert(std::fclose(file) == 0);

  source.set_file(std::string(path));
  assert(!source.is_memory_backed());
  assert(source.load(&contents, &error));
  assert(contents == file_contents);
  assert(std::remove(path) == 0);
  return 0;
}

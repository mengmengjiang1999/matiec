#include "compiler/output_sink.hh"
#include "compiler/output_manager.hh"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>

int main() {
  matiec::MemoryOutputSink memory;
  assert(memory.write("PROGRAM ").ok);
  assert(memory.write("demo").ok);
  assert(memory.flush().ok);
  assert(memory.contents() == "PROGRAM demo");
  assert(memory.good());
  memory.clear();
  assert(memory.contents().empty());

  const std::string path = "output_sink_test.tmp";
  {
    matiec::FileOutputSink file(path);
    assert(file.good());
    assert(file.write("generated\n").ok);
    assert(file.flush().ok);
  }
  std::ifstream generated(path, std::ios::binary);
  const std::string contents((std::istreambuf_iterator<char>(generated)),
                             std::istreambuf_iterator<char>());
  assert(contents == "generated\n");
  generated.close();
  assert(std::remove(path.c_str()) == 0);

  matiec::FileOutputSink invalid("missing-output-directory/file.c");
  assert(!invalid.good());
  assert(!invalid.write("ignored").ok);
  assert(!invalid.flush().ok);
  assert(!invalid.error_message().empty());

  matiec::DiagnosticEngine diagnostics;
  matiec::OutputManager outputs(diagnostics);
  matiec::MemoryOutputSink &managed_memory = outputs.create_memory();
  assert(outputs.write(managed_memory, "managed").ok);
  assert(outputs.flush(managed_memory).ok);
  assert(managed_memory.contents() == "managed");
  outputs.create_file("missing-managed-output/file.c");
  assert(outputs.has_errors());
  assert(diagnostics.error_count() == 1);
  return 0;
}

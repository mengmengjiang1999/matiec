#include "compiler/compilation_context.hh"
#include "compiler/parser_symbol_tables.hh"

#include <cassert>

int main() {
  matiec::CompilationContext first;
  matiec::CompilationContext second;

  first.options().pre_parsing = true;
  first.options().language_profile =
      matiec::LanguageProfile::iec61131_3_2025_experimental;
  first.options().include_directory = "first-lib";
  first.set_source_path("first.st");
  first.diagnostics().error("first failure");

  second.options().relaxed_datatype_model = true;
  second.options().include_directory = "second-lib";
  second.set_source_path("second.st");
  matiec::NamespaceDeclarationAst first_namespace;
  first_namespace.name = "First";
  first.experimental_syntax().namespaces.push_back(first_namespace);

  matiec::MemoryOutputSink &first_output = first.outputs().create_memory();
  matiec::MemoryOutputSink &second_output = second.outputs().create_memory();
  assert(first.outputs().write(first_output, "first").ok);
  assert(second.outputs().write(second_output, "second").ok);

  assert(first.options().pre_parsing);
  assert(first.options().language_profile ==
         matiec::LanguageProfile::iec61131_3_2025_experimental);
  assert(second.options().language_profile == matiec::LanguageProfile::legacy);
  assert(!second.options().pre_parsing);
  assert(!first.options().relaxed_datatype_model);
  assert(second.options().relaxed_datatype_model);
  assert(first.options().include_directory == "first-lib");
  assert(second.options().include_directory == "second-lib");
  assert(first.source_path() == "first.st");
  assert(second.source_path() == "second.st");
  assert(first.experimental_syntax().namespaces.size() == 1);
  assert(second.experimental_syntax().namespaces.empty());
  first.experimental_syntax().clear();
  assert(first.experimental_syntax().namespaces.empty());
  assert(first.analysis().size() == 0);
  assert(second.analysis().size() == 0);
  first.declaration_symbols().function_blocks.insert("FirstBlock", nullptr);
  assert(first.declaration_symbols().function_blocks.count("FirstBlock") == 1);
  assert(second.declaration_symbols().function_blocks.count("FirstBlock") == 0);
  {
    matiec::ActiveDeclarationSymbolTablesScope first_symbols(
        first.declaration_symbols());
    assert(&matiec::active_declaration_symbol_tables() ==
           &first.declaration_symbols());
    {
      matiec::ActiveDeclarationSymbolTablesScope second_symbols(
          second.declaration_symbols());
      assert(&matiec::active_declaration_symbol_tables() ==
             &second.declaration_symbols());
    }
    assert(&matiec::active_declaration_symbol_tables() ==
           &first.declaration_symbols());
  }
  first.parser_state().options.pre_parsing = true;
  first.parser_state().goto_body = true;
  first.parser_state().symbols().library_elements.insert("FirstProgram", 1);
  assert(!second.parser_state().options.pre_parsing);
  assert(!second.parser_state().goto_body);
  assert(second.parser_state().symbols().library_elements.find("FirstProgram") ==
         second.parser_state().symbols().library_elements.end());
  first.parser_state().reset_for_parse();
  assert(!first.parser_state().goto_body);
  assert(first.parser_state().symbols().library_elements.find("FirstProgram") ==
         first.parser_state().symbols().library_elements.end());
  {
    matiec::ActiveParserStateScope first_scope(first.parser_state());
    assert(&matiec::active_parser_state() == &first.parser_state());
    {
      matiec::ActiveParserStateScope second_scope(second.parser_state());
      assert(&matiec::active_parser_state() == &second.parser_state());
    }
    assert(&matiec::active_parser_state() == &first.parser_state());
  }
  assert(first.diagnostics().has_errors());
  assert(!second.diagnostics().has_errors());
  assert(first_output.contents() == "first");
  assert(second_output.contents() == "second");
  return 0;
}

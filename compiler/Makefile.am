include ../common.mk

AM_CPPFLAGS = -I$(top_srcdir)

noinst_LIBRARIES = libcompiler.a
libcompiler_a_SOURCES = \
	access_variable_normalizer.cc \
	access_variable_ast.cc \
	analysis_store.cc \
	ast_arena.cc \
	compilation_abort.cc \
	compilation_context.cc \
	compiler.cc \
	compiler_types.cc \
	diagnostic_engine.cc \
	legacy_global_state_adapter.cc \
	language_feature_catalog.cc \
	modern_library_registry.cc \
	namespace_ast_analysis.cc \
	namespace_normalizer.cc \
	object_method_ast_analysis.cc \
	object_method_call_lowering.cc \
	object_method_compatibility_ast.cc \
	output_manager.cc \
	output_sink.cc \
	parser_state.cc \
	semantic_pass.cc \
	semantic_pass_manager.cc \
	source_manager.cc \
	utf8_validation.cc

check_PROGRAMS = access_variable_normalizer_test analysis_store_test ast_arena_test compilation_abort_test compilation_context_test compiler_types_test diagnostic_engine_test language_feature_catalog_test namespace_normalizer_test output_sink_test semantic_pass_test source_manager_test utf8_validation_test
TESTS = $(check_PROGRAMS)

COMPILER_TEST_LDADD = libcompiler.a ../absyntax/libabsyntax.a libcompiler.a

access_variable_normalizer_test_SOURCES = tests/access_variable_normalizer_test.cc
access_variable_normalizer_test_LDADD = $(COMPILER_TEST_LDADD)

analysis_store_test_SOURCES = tests/analysis_store_test.cc
analysis_store_test_LDADD = $(COMPILER_TEST_LDADD)

ast_arena_test_SOURCES = tests/ast_arena_test.cc
ast_arena_test_LDADD = $(COMPILER_TEST_LDADD)

compilation_abort_test_SOURCES = tests/compilation_abort_test.cc
compilation_abort_test_LDADD = $(COMPILER_TEST_LDADD)

compilation_context_test_SOURCES = tests/compilation_context_test.cc
compilation_context_test_LDADD = $(COMPILER_TEST_LDADD)

compiler_types_test_SOURCES = tests/compiler_types_test.cc
compiler_types_test_LDADD = $(COMPILER_TEST_LDADD)

diagnostic_engine_test_SOURCES = tests/diagnostic_engine_test.cc
diagnostic_engine_test_LDADD = $(COMPILER_TEST_LDADD)

language_feature_catalog_test_SOURCES = tests/language_feature_catalog_test.cc
language_feature_catalog_test_LDADD = $(COMPILER_TEST_LDADD)

namespace_normalizer_test_SOURCES = tests/namespace_normalizer_test.cc
namespace_normalizer_test_LDADD = $(COMPILER_TEST_LDADD)

output_sink_test_SOURCES = tests/output_sink_test.cc
output_sink_test_LDADD = $(COMPILER_TEST_LDADD)

semantic_pass_test_SOURCES = tests/semantic_pass_test.cc
semantic_pass_test_LDADD = $(COMPILER_TEST_LDADD)

source_manager_test_SOURCES = tests/source_manager_test.cc
source_manager_test_LDADD = $(COMPILER_TEST_LDADD)

utf8_validation_test_SOURCES = tests/utf8_validation_test.cc
utf8_validation_test_LDADD = $(COMPILER_TEST_LDADD)

include ../common.mk

AM_CPPFLAGS = -I$(top_srcdir)

noinst_LIBRARIES = libcompiler.a
libcompiler_a_SOURCES = \
	access_variable_normalizer.cc \
	ast_arena.cc \
	compilation_context.cc \
	compiler.cc \
	compiler_types.cc \
	diagnostic_engine.cc \
	legacy_global_state_adapter.cc \
	language_feature_catalog.cc \
	modern_library_normalizer.cc \
	namespace_normalizer.cc \
	object_method_normalizer.cc \
	output_manager.cc \
	output_sink.cc \
	semantic_pass.cc \
	semantic_pass_manager.cc \
	utf8_validation.cc

check_PROGRAMS = access_variable_normalizer_test ast_arena_test compilation_abort_test compilation_context_test compiler_types_test diagnostic_engine_test language_feature_catalog_test modern_library_normalizer_test namespace_normalizer_test object_method_normalizer_test output_sink_test semantic_pass_test utf8_validation_test
TESTS = $(check_PROGRAMS)

access_variable_normalizer_test_SOURCES = tests/access_variable_normalizer_test.cc
access_variable_normalizer_test_LDADD = libcompiler.a

ast_arena_test_SOURCES = tests/ast_arena_test.cc
ast_arena_test_LDADD = libcompiler.a

compilation_abort_test_SOURCES = tests/compilation_abort_test.cc compilation_abort.cc

compilation_context_test_SOURCES = tests/compilation_context_test.cc
compilation_context_test_LDADD = libcompiler.a

compiler_types_test_SOURCES = tests/compiler_types_test.cc
compiler_types_test_LDADD = libcompiler.a

diagnostic_engine_test_SOURCES = tests/diagnostic_engine_test.cc
diagnostic_engine_test_LDADD = libcompiler.a

language_feature_catalog_test_SOURCES = tests/language_feature_catalog_test.cc
language_feature_catalog_test_LDADD = libcompiler.a

modern_library_normalizer_test_SOURCES = tests/modern_library_normalizer_test.cc
modern_library_normalizer_test_LDADD = libcompiler.a

namespace_normalizer_test_SOURCES = tests/namespace_normalizer_test.cc
namespace_normalizer_test_LDADD = libcompiler.a

object_method_normalizer_test_SOURCES = tests/object_method_normalizer_test.cc
object_method_normalizer_test_LDADD = libcompiler.a

output_sink_test_SOURCES = tests/output_sink_test.cc
output_sink_test_LDADD = libcompiler.a

semantic_pass_test_SOURCES = tests/semantic_pass_test.cc
semantic_pass_test_LDADD = libcompiler.a

utf8_validation_test_SOURCES = tests/utf8_validation_test.cc
utf8_validation_test_LDADD = libcompiler.a

include ../common.mk

AM_CPPFLAGS = -I$(top_srcdir)

noinst_LIBRARIES = libcompiler.a
libcompiler_a_SOURCES = \
	ast_arena.cc \
	compilation_context.cc \
	compiler.cc \
	compiler_types.cc \
	diagnostic_engine.cc \
	legacy_global_state_adapter.cc \
	output_manager.cc \
	output_sink.cc \
	semantic_pass.cc \
	semantic_pass_manager.cc

check_PROGRAMS = ast_arena_test compilation_abort_test compilation_context_test compiler_types_test diagnostic_engine_test output_sink_test semantic_pass_test
TESTS = $(check_PROGRAMS)

ast_arena_test_SOURCES = tests/ast_arena_test.cc
ast_arena_test_LDADD = libcompiler.a

compilation_abort_test_SOURCES = tests/compilation_abort_test.cc compilation_abort.cc

compilation_context_test_SOURCES = tests/compilation_context_test.cc
compilation_context_test_LDADD = libcompiler.a

compiler_types_test_SOURCES = tests/compiler_types_test.cc
compiler_types_test_LDADD = libcompiler.a

diagnostic_engine_test_SOURCES = tests/diagnostic_engine_test.cc
diagnostic_engine_test_LDADD = libcompiler.a

output_sink_test_SOURCES = tests/output_sink_test.cc
output_sink_test_LDADD = libcompiler.a

semantic_pass_test_SOURCES = tests/semantic_pass_test.cc
semantic_pass_test_LDADD = libcompiler.a

include ../common.mk

AM_CPPFLAGS = -I$(top_srcdir)

noinst_LIBRARIES = libcompiler.a
libcompiler_a_SOURCES = \
	compilation_context.cc \
	compiler.cc \
	compiler_types.cc \
	diagnostic_engine.cc \
	legacy_global_state_adapter.cc

check_PROGRAMS = compilation_abort_test compilation_context_test compiler_types_test diagnostic_engine_test
TESTS = $(check_PROGRAMS)

compilation_abort_test_SOURCES = tests/compilation_abort_test.cc compilation_abort.cc

compilation_context_test_SOURCES = tests/compilation_context_test.cc
compilation_context_test_LDADD = libcompiler.a

compiler_types_test_SOURCES = tests/compiler_types_test.cc
compiler_types_test_LDADD = libcompiler.a

diagnostic_engine_test_SOURCES = tests/diagnostic_engine_test.cc
diagnostic_engine_test_LDADD = libcompiler.a

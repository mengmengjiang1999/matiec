include ../common.mk

AM_CPPFLAGS = -I$(top_srcdir)

noinst_LIBRARIES = libcompiler.a
libcompiler_a_SOURCES = compiler_types.cc

check_PROGRAMS = compiler_types_test
TESTS = $(check_PROGRAMS)

compiler_types_test_SOURCES = tests/compiler_types_test.cc
compiler_types_test_LDADD = libcompiler.a

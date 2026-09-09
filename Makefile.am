include common.mk

bin_PROGRAMS = iec2c iec2iec

SUBDIRS = compiler absyntax absyntax_utils stage1_2 stage3 stage4

TESTS_ENVIRONMENT = \
	CC='$(CC)' \
	IEC2C='$(abs_top_builddir)/iec2c' \
	IEC2IEC='$(abs_top_builddir)/iec2iec' \
	MATIEC_LIBDIR='$(abs_top_srcdir)/lib' \
	SEQUENTIAL_VALID_INPUT='$(abs_top_srcdir)/tests/initialization/ok_array_scalar.st'

TESTS = \
	modern_library_registry_test \
	flow_analysis_store_test \
	constant_analysis_store_test \
	datatype_analysis_store_test \
	resolution_analysis_store_test \
	enumeration_analysis_store_test \
	generator_analysis_store_test \
	sequential_compilation_test \
	concurrent_frontend_test \
	parallel_compilation_test \
	in_memory_generator_test \
	tests/generated_c/runtests \
	tests/initialization/runtests \
	tests/cli/runtests \
	tests/architecture/runtests \
	tests/syntax/identifier/runtests \
	tests/characterization/runtests

EXTRA_DIST = \
	$(TESTS) \
	tests/characterization/expected.sha256 \
	tests/characterization/instruction_list.il \
	include/matiec/api.h \
	tests/cli/profile-fixtures/reference.st \
	tests/cli/profile-fixtures/reference-null.st \
	tests/cli/profile-fixtures/reference-incompatible.st \
	tests/cli/profile-fixtures/reference-broad-extension.st \
	tests/cli/profile-fixtures/reference-local-lifetime.st \
	tests/cli/profile-fixtures/utf8.st \
	tests/cli/profile-fixtures/namespace.st \
	tests/cli/profile-fixtures/namespace-using.st \
	tests/cli/profile-fixtures/namespace-ambiguous.st \
	tests/cli/profile-fixtures/namespace-internal.st \
	tests/cli/profile-fixtures/namespace-internal-local.st \
	tests/cli/profile-fixtures/object-method.st \
	tests/cli/profile-fixtures/object-method-private.st \
	tests/cli/profile-fixtures/object-method-unknown.st \
	compiler/experimental_syntax_model.hh \
	compiler/parser_symbol_tables.hh \
	tests/generated_c/runtests \
	tests/initialization/ok_array_scalar.st \
	tests/syntax/sfc/sfc.txt \
	tests/sanitizers/runtests \
	config/revision.sh \
	config/thread_localize_generated_frontend.pl \
	.git_archival.txt

.PHONY: check-asan check-ubsan

check-asan:
	tests/sanitizers/runtests address

check-ubsan:
	tests/sanitizers/runtests undefined

ACLOCAL_AMFLAGS=-I config

MATIEC_REVISION = $(shell $(SHELL) $(top_srcdir)/config/revision.sh $(top_srcdir))
AM_CXXFLAGS += -DMATIEC_REVISION="\"${MATIEC_REVISION}\""

iec2c_LDADD = compiler/libcompiler.a \
	stage1_2/libstage1_2.a \
	stage1_2/libstage1_2_generated.a \
	stage1_2/libstage1_2_parser_support.a \
	stage3/libstage3.a \
	compiler/libcompiler.a \
	stage4/libstage4_common.a \
	stage4/generate_c/libstage4_c.a \
	absyntax/libabsyntax.a \
	absyntax_utils/libabsyntax_utils.a \
	compiler/libcompiler.a

iec2iec_LDADD = compiler/libcompiler.a \
	stage1_2/libstage1_2.a \
	stage1_2/libstage1_2_generated.a \
	stage1_2/libstage1_2_parser_support.a \
	stage3/libstage3.a \
	compiler/libcompiler.a \
	stage4/libstage4_common.a \
	stage4/generate_iec/libstage4_iec.a \
	absyntax/libabsyntax.a \
	absyntax_utils/libabsyntax_utils.a \
	compiler/libcompiler.a

iec2c_SOURCES = main.cc

iec2iec_SOURCES = main.cc

check_PROGRAMS = modern_library_registry_test flow_analysis_store_test constant_analysis_store_test datatype_analysis_store_test resolution_analysis_store_test enumeration_analysis_store_test generator_analysis_store_test sequential_compilation_test concurrent_frontend_test parallel_compilation_test in_memory_generator_test

modern_library_registry_test_SOURCES = \
	tests/compiler/modern_library_registry_test.cc
modern_library_registry_test_LDADD = $(iec2c_LDADD)

flow_analysis_store_test_SOURCES = \
	tests/compiler/flow_analysis_store_test.cc
flow_analysis_store_test_LDADD = $(iec2c_LDADD)

constant_analysis_store_test_SOURCES = \
	tests/compiler/constant_analysis_store_test.cc
constant_analysis_store_test_LDADD = $(iec2c_LDADD)

datatype_analysis_store_test_SOURCES = \
	tests/compiler/datatype_analysis_store_test.cc
datatype_analysis_store_test_LDADD = $(iec2c_LDADD)

resolution_analysis_store_test_SOURCES = \
	tests/compiler/resolution_analysis_store_test.cc
resolution_analysis_store_test_LDADD = $(iec2c_LDADD)

enumeration_analysis_store_test_SOURCES = \
	tests/compiler/enumeration_analysis_store_test.cc
enumeration_analysis_store_test_LDADD = $(iec2c_LDADD)

generator_analysis_store_test_SOURCES = \
	tests/compiler/generator_analysis_store_test.cc
generator_analysis_store_test_LDADD = $(iec2c_LDADD)

sequential_compilation_test_SOURCES = \
	tests/compiler/sequential_compilation_test.cc
sequential_compilation_test_LDADD = $(iec2c_LDADD)

concurrent_frontend_test_SOURCES = \
	tests/compiler/concurrent_frontend_test.cc
concurrent_frontend_test_LDADD = $(iec2c_LDADD)

parallel_compilation_test_SOURCES = \
	tests/compiler/parallel_compilation_test.cc
parallel_compilation_test_LDADD = $(iec2c_LDADD)

in_memory_generator_test_SOURCES = \
	tests/compiler/in_memory_generator_test.cc
in_memory_generator_test_LDADD = $(iec2c_LDADD)

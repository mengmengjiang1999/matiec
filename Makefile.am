include common.mk

bin_PROGRAMS = iec2c iec2iec

SUBDIRS = compiler absyntax absyntax_utils stage1_2 stage3 stage4

TESTS_ENVIRONMENT = \
	CC='$(CC)' \
	CPPFLAGS='$(CPPFLAGS)' \
	CFLAGS='$(CFLAGS)' \
	CXXFLAGS='$(CXXFLAGS)' \
	LDFLAGS='$(LDFLAGS)' \
	IEC2C='$(abs_top_builddir)/iec2c' \
	IEC2IEC='$(abs_top_builddir)/iec2iec' \
	MATIEC_LIBDIR='$(abs_top_srcdir)/lib' \
	PACKAGE_INCLUDEDIR='$(includedir)' \
	PACKAGE_LIBDIR='$(libdir)' \
	PACKAGE_DATADIR='$(datadir)' \
	PACKAGE_SHARED_REAL='$(MATIEC_SHARED_REAL)' \
	PACKAGE_SHARED_ABI='$(MATIEC_SHARED_ABI)' \
	PACKAGE_SHARED_LINK='$(MATIEC_SHARED_LINK)' \
	PACKAGE_CONSUMER='$(abs_top_srcdir)/tests/package/consumer.c' \
	MATIEC_API_ARCHIVE='$(abs_top_builddir)/libmatiec.a' \
	MATIEC_API_SHARED='$(abs_top_builddir)/$(MATIEC_SHARED_REAL)' \
	MATIEC_API_SYMBOLS='$(abs_top_srcdir)/tests/api/public-symbols.txt' \
	MATIEC_API_HEADER_TEST='$(abs_top_srcdir)/tests/api/header_compatibility.c' \
	MATIEC_API_INCLUDE='$(abs_top_srcdir)/include' \
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
	c_api_single_compilation_test \
	c_api_callbacks_test \
	c_api_parallel_test \
	c_api_virtual_include_test \
	c_api_limits_test \
	c_api_diagnostic_metadata_test \
	c_api_concurrency_stress_test \
	tests/generated_c/runtests \
	tests/initialization/runtests \
	tests/cli/runtests \
	tests/architecture/runtests \
	tests/syntax/identifier/runtests \
	tests/characterization/runtests \
	tests/api/runtests \
	tests/package/runtests

EXTRA_DIST = \
	$(TESTS) \
	tests/characterization/expected.sha256 \
	tests/characterization/instruction_list.il \
	include/matiec/api.h \
	matiec.pc.in \
	config/merge-static-libraries.sh \
	tests/package/consumer.c \
	tests/package/runtests \
	tests/api/header_compatibility.c \
	tests/api/public-symbols.txt \
	tests/api/runtests \
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

.PHONY: check-api check-asan check-ubsan check-tsan

check-api: all
	$(TESTS_ENVIRONMENT) $(SHELL) $(top_srcdir)/tests/api/runtests
	$(TESTS_ENVIRONMENT) $(SHELL) $(top_srcdir)/tests/package/runtests

check-asan:
	tests/sanitizers/runtests address

check-ubsan:
	tests/sanitizers/runtests undefined

check-tsan:
	tests/sanitizers/runtests thread

ACLOCAL_AMFLAGS=-I config

matiecincludedir = $(includedir)/matiec
matiecinclude_HEADERS = include/matiec/api.h

pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = matiec.pc

matieclibdir = $(datadir)/matiec/lib
dist_matieclib_DATA = \
	lib/bistable.txt \
	lib/counter.txt \
	lib/derivative_st.txt \
	lib/edge_detection.txt \
	lib/hysteresis_st.txt \
	lib/ieclib.txt \
	lib/integral_st.txt \
	lib/pid_st.txt \
	lib/ramp_st.txt \
	lib/rtc.txt \
	lib/sema.txt \
	lib/standard_FB.txt \
	lib/standard_functions.txt \
	lib/timer.txt

matiecruntimedir = $(matieclibdir)/C
dist_matiecruntime_DATA = \
	lib/C/accessor.h \
	lib/C/iec_std_FB.h \
	lib/C/iec_std_FB_no_ENENO.h \
	lib/C/iec_std_functions.h \
	lib/C/iec_std_lib.h \
	lib/C/iec_types.h \
	lib/C/iec_types_all.h

MATIEC_COMPONENT_ARCHIVES = \
	compiler/libcompiler.a \
	stage1_2/libstage1_2.a \
	stage1_2/libstage1_2_generated.a \
	stage1_2/libstage1_2_parser_support.a \
	stage3/libstage3.a \
	stage4/libstage4_common.a \
	stage4/generate_c/libstage4_c.a \
	absyntax/libabsyntax.a \
	absyntax_utils/libabsyntax_utils.a

matiecarchivedir = $(libdir)
nodist_matiecarchive_DATA = libmatiec.a
matiecshareddir = $(libdir)
nodist_matiecshared_DATA = $(MATIEC_SHARED_REAL)
CLEANFILES = libmatiec.a $(MATIEC_SHARED_REAL)

libmatiec.a: $(MATIEC_COMPONENT_ARCHIVES) $(top_srcdir)/config/merge-static-libraries.sh
	$(SHELL) $(top_srcdir)/config/merge-static-libraries.sh "$(AR)" "$@" $(MATIEC_COMPONENT_ARCHIVES)
	$(RANLIB) "$@"

$(MATIEC_SHARED_REAL): $(MATIEC_COMPONENT_ARCHIVES)
	if test "$(MATIEC_SHARED_MODE)" = darwin; then \
	  $(CXX) $(CXXFLAGS) $(LDFLAGS) -dynamiclib -Wl,-install_name,$(libdir)/$(MATIEC_SHARED_ABI) \
	    -Wl,-all_load $(MATIEC_COMPONENT_ARCHIVES) -pthread $(LIBS) -o "$@"; \
	else \
	  $(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -Wl,-soname,$(MATIEC_SHARED_ABI) \
	    -Wl,--whole-archive $(MATIEC_COMPONENT_ARCHIVES) -Wl,--no-whole-archive \
	    -pthread $(LIBS) -o "$@"; \
	fi

install-data-hook:
	cd "$(DESTDIR)$(libdir)" && \
	  rm -f "$(MATIEC_SHARED_ABI)" "$(MATIEC_SHARED_LINK)" && \
	  $(LN_S) "$(MATIEC_SHARED_REAL)" "$(MATIEC_SHARED_ABI)" && \
	  $(LN_S) "$(MATIEC_SHARED_ABI)" "$(MATIEC_SHARED_LINK)"

uninstall-hook:
	rm -f "$(DESTDIR)$(libdir)/$(MATIEC_SHARED_ABI)" \
	  "$(DESTDIR)$(libdir)/$(MATIEC_SHARED_LINK)"

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

check_PROGRAMS = modern_library_registry_test flow_analysis_store_test constant_analysis_store_test datatype_analysis_store_test resolution_analysis_store_test enumeration_analysis_store_test generator_analysis_store_test sequential_compilation_test concurrent_frontend_test parallel_compilation_test in_memory_generator_test c_api_single_compilation_test c_api_callbacks_test c_api_parallel_test c_api_virtual_include_test c_api_limits_test c_api_diagnostic_metadata_test c_api_concurrency_stress_test

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

c_api_single_compilation_test_SOURCES = \
	tests/compiler/c_api_single_compilation_test.c
c_api_single_compilation_test_LINK = $(CXXLINK)
c_api_single_compilation_test_LDADD = $(iec2c_LDADD)

c_api_callbacks_test_SOURCES = tests/compiler/c_api_callbacks_test.c
c_api_callbacks_test_LINK = $(CXXLINK)
c_api_callbacks_test_LDADD = $(iec2c_LDADD)

c_api_parallel_test_SOURCES = tests/compiler/c_api_parallel_test.c
c_api_parallel_test_LINK = $(CXXLINK)
c_api_parallel_test_LDADD = $(iec2c_LDADD)

c_api_virtual_include_test_SOURCES = \
	tests/compiler/c_api_virtual_include_test.c
c_api_virtual_include_test_LINK = $(CXXLINK)
c_api_virtual_include_test_LDADD = $(iec2c_LDADD)

c_api_limits_test_SOURCES = tests/compiler/c_api_limits_test.c
c_api_limits_test_LINK = $(CXXLINK)
c_api_limits_test_LDADD = $(iec2c_LDADD)

c_api_diagnostic_metadata_test_SOURCES = \
	tests/compiler/c_api_diagnostic_metadata_test.c
c_api_diagnostic_metadata_test_LINK = $(CXXLINK)
c_api_diagnostic_metadata_test_LDADD = $(iec2c_LDADD)

c_api_concurrency_stress_test_SOURCES = \
	tests/compiler/c_api_concurrency_stress_test.cc
c_api_concurrency_stress_test_LDADD = $(iec2c_LDADD)

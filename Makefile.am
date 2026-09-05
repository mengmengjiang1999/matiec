include common.mk

bin_PROGRAMS = iec2c iec2iec

SUBDIRS = compiler absyntax absyntax_utils stage1_2 stage3 stage4

TESTS_ENVIRONMENT = \
	CC='$(CC)' \
	IEC2C='$(abs_top_builddir)/iec2c' \
	IEC2IEC='$(abs_top_builddir)/iec2iec' \
	MATIEC_LIBDIR='$(abs_top_srcdir)/lib'

TESTS = \
	tests/initialization/runtests \
	tests/cli/runtests \
	tests/syntax/identifier/runtests \
	tests/characterization/runtests

EXTRA_DIST = \
	$(TESTS) \
	tests/characterization/expected.sha256 \
	tests/characterization/instruction_list.il \
	tests/initialization/ok_array_scalar.st \
	tests/syntax/sfc/sfc.txt \
	tests/sanitizers/runtests \
	config/revision.sh \
	.git_archival.txt

.PHONY: check-asan check-ubsan

check-asan:
	tests/sanitizers/runtests address

check-ubsan:
	tests/sanitizers/runtests undefined

ACLOCAL_AMFLAGS=-I config

MATIEC_REVISION = $(shell $(SHELL) $(top_srcdir)/config/revision.sh $(top_srcdir))
AM_CXXFLAGS += -DMATIEC_REVISION="\"${MATIEC_REVISION}\""

iec2c_LDADD = stage1_2/libstage1_2.a \
	stage1_2/libstage1_2_generated.a \
	stage1_2/libstage1_2_parser_support.a \
	stage3/libstage3.a \
	stage4/generate_c/libstage4_c.a \
	absyntax/libabsyntax.a \
	absyntax_utils/libabsyntax_utils.a 

iec2iec_LDADD = stage1_2/libstage1_2.a \
	stage1_2/libstage1_2_generated.a \
	stage1_2/libstage1_2_parser_support.a \
	stage3/libstage3.a \
	stage4/generate_iec/libstage4_iec.a \
	absyntax/libabsyntax.a \
	absyntax_utils/libabsyntax_utils.a 

iec2c_SOURCES = main.cc

iec2iec_SOURCES = main.cc

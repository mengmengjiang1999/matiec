include ../common.mk

## Flags for yacc syntax parser generator (bison)
AM_YFLAGS = -d -Wno-yacc
## Flags for lex lexer generator (flex)
AM_LFLAGS = --warn -o$(LEX_OUTPUT_ROOT).c

# Make sure this header file is generated first (by bison), as it is included
# by other C++ code that will also be compiled.
FRONTEND_ISOLATION_STAMP = .generated-frontend-isolation.stamp
BUILT_SOURCES = iec_bison.hh $(FRONTEND_ISOLATION_STAMP)

%.hh: %.h
	cp $< $@

CLEANFILES = \
	iec_flex.cc \
	iec_bison.cc \
	iec_bison.hh \
	$(FRONTEND_ISOLATION_STAMP)

$(FRONTEND_ISOLATION_STAMP): iec_flex.cc iec_bison.cc iec_bison.hh $(top_srcdir)/config/thread_localize_generated_frontend.pl
	$(top_srcdir)/config/thread_localize_generated_frontend.pl iec_flex.cc iec_bison.cc iec_bison.hh
	touch $@

libstage1_2_generated_a-iec_flex.$(OBJEXT) \
libstage1_2_generated_a-iec_bison.$(OBJEXT): $(FRONTEND_ISOLATION_STAMP)

lib_LIBRARIES = libstage1_2.a
noinst_LIBRARIES = libstage1_2_generated.a libstage1_2_parser_support.a

libstage1_2_generated_a_SOURCES = \
	iec_flex.ll \
	iec_bison.yy

libstage1_2_generated_a_CPPFLAGS = -DDEFAULT_LIBDIR='"lib"' -I../../absyntax -DYY_BUF_SIZE=65536
libstage1_2_generated_a_CXXFLAGS = -std=gnu++17 -g -w -fpermissive

libstage1_2_parser_support_a_SOURCES = create_enumtype_conversion_functions.cc
libstage1_2_parser_support_a_CPPFLAGS = -DDEFAULT_LIBDIR='"lib"' -I../../absyntax -DYY_BUF_SIZE=65536

libstage1_2_a_SOURCES = stage1_2.cc

libstage1_2_a_CPPFLAGS = -DDEFAULT_LIBDIR='"lib"' -I../../absyntax -DYY_BUF_SIZE=65536

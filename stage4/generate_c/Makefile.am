include ../../common.mk

lib_LIBRARIES = libstage4_c.a

libstage4_c_a_SOURCES = \
	generate_c.cc \
	generate_c_configbody.cc \
	generate_c_typedecl.cc \
	generate_c_sfcdecl.cc \
	generate_c_vardecl.cc \
	generate_location_list.cc \
	generate_var_list.cc \
	generate_c_st.cc \
	generate_c_il.cc \
	generate_c_inlinefcall.cc \
	generate_c_sfc.cc

libstage4_c_a_LIBADD = ../stage4.o

libstage4_c_a_CPPFLAGS = -I../../../absyntax

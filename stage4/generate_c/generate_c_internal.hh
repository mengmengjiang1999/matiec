#ifndef MATIEC_STAGE4_GENERATE_C_INTERNAL_HH
#define MATIEC_STAGE4_GENERATE_C_INTERNAL_HH

#include "../stage4.hh"
#include "../../absyntax/visitor.hh"
#include "../../absyntax_utils/absyntax_utils.hh"
#include "../../compiler/compilation_abort.hh"
#include "../../main.hh"
#include "../../util/dsymtable.hh"
#include "../../util/symtable.hh"

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <string.h>
#include <strings.h>
#include <typeinfo>

class generate_c_base_c;
class generate_c_base_and_typeid_c;
class generate_c_typedecl_c;
class generate_c_implicit_typedecl_c;
class generate_c_vardecl_c;
class generate_c_sfcdecl_c;
class generate_c_st_c;
class generate_c_il_c;
class generate_c_inlinefcall_c;
class generate_c_sfc_c;

#ifdef DEBUG
#define TRACE(classname) printf("\n____%s____\n",classname)
#else
#define TRACE(classname)
#endif

#define STAGE4_ERROR(symbol1, symbol2, ...) \
  do { \
    stage4err("while generating C code", symbol1, symbol2, __VA_ARGS__); \
    throw matiec::CompilationAbort("C code generation failed", true); \
  } while (0)

#define VALID_CVALUE(dtype, symbol) ((symbol)->const_value._##dtype.is_valid())
#define GET_CVALUE(dtype, symbol) ((symbol)->const_value._##dtype.get())

#define FB_FUNCTION_SUFFIX "_body__"
#define FB_INIT_SUFFIX "_init__"
#define FB_RUN_SUFFIX "_run__"
#define FB_DATASTRUCTURE_SUFFIX "_data__"
#define FB_FUNCTION_PARAM "data__"
#define SFC_STEP_ACTION_PREFIX "__SFC_"

#define DECLARE_VAR "__DECLARE_VAR"
#define DECLARE_GLOBAL "__DECLARE_GLOBAL"
#define DECLARE_GLOBAL_FB "__DECLARE_GLOBAL_FB"
#define DECLARE_GLOBAL_LOCATION "__DECLARE_GLOBAL_LOCATION"
#define DECLARE_GLOBAL_LOCATED "__DECLARE_GLOBAL_LOCATED"
#define DECLARE_EXTERNAL "__DECLARE_EXTERNAL"
#define DECLARE_EXTERNAL_FB "__DECLARE_EXTERNAL_FB"
#define DECLARE_LOCATED "__DECLARE_LOCATED"
#define DECLARE_GLOBAL_PROTOTYPE "__DECLARE_GLOBAL_PROTOTYPE"
#define DECLARE_GLOBAL_PROTOTYPE_FB "__DECLARE_GLOBAL_PROTOTYPE_FB"

#define INIT_VAR "__INIT_VAR"
#define INIT_GLOBAL "__INIT_GLOBAL"
#define INIT_GLOBAL_FB "__INIT_GLOBAL_FB"
#define INIT_GLOBAL_LOCATED "__INIT_GLOBAL_LOCATED"
#define INIT_EXTERNAL "__INIT_EXTERNAL"
#define INIT_EXTERNAL_FB "__INIT_EXTERNAL_FB"
#define INIT_LOCATED "__INIT_LOCATED"
#define INIT_LOCATED_VALUE "__INIT_LOCATED_VALUE"

#define GET_VAR "__GET_VAR"
#define GET_EXTERNAL "__GET_EXTERNAL"
#define GET_EXTERNAL_FB "__GET_EXTERNAL_FB"
#define GET_LOCATED "__GET_LOCATED"
#define GET_VAR_REF "__GET_VAR_REF"
#define GET_EXTERNAL_REF "__GET_EXTERNAL_REF"
#define GET_EXTERNAL_FB_REF "__GET_EXTERNAL_FB_REF"
#define GET_LOCATED_REF "__GET_LOCATED_REF"
#define GET_VAR_DREF "__GET_VAR_DREF"
#define GET_EXTERNAL_DREF "__GET_EXTERNAL_DREF"
#define GET_EXTERNAL_FB_DREF "__GET_EXTERNAL_FB_DREF"
#define GET_LOCATED_DREF "__GET_LOCATED_DREF"
#define GET_VAR_BY_REF "__GET_VAR_BY_REF"
#define GET_EXTERNAL_BY_REF "__GET_EXTERNAL_BY_REF"
#define GET_EXTERNAL_FB_BY_REF "__GET_EXTERNAL_FB_BY_REF"
#define GET_LOCATED_BY_REF "__GET_LOCATED_BY_REF"

#define SET_VAR "__SET_VAR"
#define SET_EXTERNAL "__SET_EXTERNAL"
#define SET_EXTERNAL_FB "__SET_EXTERNAL_FB"
#define SET_LOCATED "__SET_LOCATED"
#define INITIAL_VALUE "__INITIAL_VALUE"

#define VAR_LEADER "__"
#define TEMP_VAR VAR_LEADER "TMP_"
#define SOURCE_VAR VAR_LEADER "SRC_"
#define END_LABEL VAR_LEADER "end"

extern int generate_line_directives__;

#endif

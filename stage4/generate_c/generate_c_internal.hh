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
class generate_c_vardecl_impl_c;
class generate_c_sfcdecl_impl_c;
class generate_c_st_c;
class generate_c_il_c;
class generate_c_inlinefcall_c;
class generate_c_sfc_c;

typedef struct {
  identifier_c *symbol;
} VARIABLE;

class generate_c_vardecl_c {
  private:
    generate_c_vardecl_impl_c *implementation_;

  public:
    static const unsigned int none_vt     = 0x0000;
    static const unsigned int input_vt    = 0x0001;
    static const unsigned int output_vt   = 0x0002;
    static const unsigned int inoutput_vt = 0x0004;
    static const unsigned int private_vt  = 0x0008;
    static const unsigned int temp_vt     = 0x0010;
    static const unsigned int external_vt = 0x0020;
    static const unsigned int global_vt   = 0x0040;
    static const unsigned int located_vt  = 0x0080;
    static const unsigned int program_vt  = 0x0100;
    static const unsigned int en_vt       = 0x0200;
    static const unsigned int eno_vt      = 0x0400;
    static const unsigned int resource_vt = 0x8000;

    typedef enum {
      finterface_vf,
      foutputassign_vf,
      local_vf,
      localinit_vf,
      init_vf,
      constructorinit_vf,
      globalinit_vf,
      globalprototype_vf,
      location_list_vf
    } varformat_t;

    generate_c_vardecl_c(stage4out_c *s4o_ptr, varformat_t varformat,
                         unsigned int vartype, symbol_c *res_name = NULL);
    ~generate_c_vardecl_c(void);
    void print(symbol_c *symbol, symbol_c *scope = NULL,
               const char *variable_prefix = NULL);
};

class generate_c_sfcdecl_c {
  private:
    generate_c_sfcdecl_impl_c *implementation_;

  public:
    typedef enum {
      sfcdecl_sd,
      sfcinit_sd,
      stepcount_sd,
      stepdef_sd,
      stepundef_sd,
      actiondef_sd,
      actionundef_sd,
      actioncount_sd,
      transitioncount_sd
    } sfcdeclaration_t;

    generate_c_sfcdecl_c(stage4out_c *s4o_ptr, symbol_c *scope,
                         const char *variable_prefix = NULL);
    ~generate_c_sfcdecl_c(void);
    void generate(symbol_c *symbol, sfcdeclaration_t declaration_type);
};

class generate_c_type_generators_impl_c;

class generate_c_type_generators_c {
  private:
    generate_c_type_generators_impl_c *implementation_;

  public:
    explicit generate_c_type_generators_c(stage4out_c *s4o_ptr);
    ~generate_c_type_generators_c(void);
    visitor_c &explicit_declarations(void);
    visitor_c &implicit_declarations(void);
};

/* A helper shared by the language-specific generators. */
class analyse_variable_c: public search_visitor_c {
  private:
    symbol_c *last_fb;
    symbol_c *first_non_fb_identifier;

  public:
    analyse_variable_c(void) : last_fb(NULL), first_non_fb_identifier(NULL) {}

    static bool is_complex_type(symbol_c *symbol) {
      if (NULL == symbol) ERROR;
      if (!get_datatype_info_c::is_type_valid(symbol->datatype)) return false;
      return (get_datatype_info_c::is_structure(symbol->datatype)
           || get_datatype_info_c::is_array(symbol->datatype));
    }

    static symbol_c *find_first_nonfb(symbol_c *symbol) {
      if (NULL == symbol) ERROR;
      analyse_variable_c visitor;
      return (symbol_c *)symbol->accept(visitor);
    }

    static bool contains_complex_type(symbol_c *symbol) {
      if (NULL == symbol) ERROR;
      if (!get_datatype_info_c::is_type_valid(symbol->datatype)) ERROR;
      symbol_c *first_non_fb = find_first_nonfb(symbol);
      return is_complex_type(first_non_fb->datatype);
    }

    static search_var_instance_decl_c::vt_t first_nonfb_vardecltype(symbol_c *symbol, symbol_c *scope) {
      if (NULL == symbol) ERROR;
      if (!get_datatype_info_c::is_type_valid(symbol->datatype)) ERROR;

      analyse_variable_c visitor;
      symbol_c *first_non_fb = (symbol_c *)symbol->accept(visitor);
      if (NULL != visitor.last_fb) {
        scope = visitor.last_fb->datatype;
        symbol = visitor.first_non_fb_identifier;
      }

      search_var_instance_decl_c search_var_instance_decl(scope);
      return search_var_instance_decl.get_vartype(symbol);
    }

    void *visit(symbolic_variable_c *symbol) {
      if (!get_datatype_info_c::is_type_valid(symbol->datatype)) ERROR;
      if (!get_datatype_info_c::is_function_block(symbol->datatype)) {
        first_non_fb_identifier = symbol;
        return (void *)symbol;
      }
      last_fb = symbol;
      return NULL;
    }

    void *visit(structured_variable_c *symbol) {
      symbol_c *res = (symbol_c *)symbol->record_variable->accept(*this);
      if (NULL != res) return res;

      if (!get_datatype_info_c::is_type_valid(symbol->datatype)) ERROR;
      if (!get_datatype_info_c::is_function_block(symbol->datatype)) {
        first_non_fb_identifier = symbol->field_selector;
        return (void *)symbol;
      }
      last_fb = symbol;
      return NULL;
    }

    void *visit(array_variable_c *symbol) {
      void *res = symbol->subscripted_variable->accept(*this);
      if (NULL != res) return res;
      return (void *)symbol;
    }
};

class il_default_variable_c;

class il_default_variable_visitor_c {
  public:
    virtual void *visit(il_default_variable_c *symbol) = 0;
    virtual ~il_default_variable_visitor_c(void) {}
};

class il_default_variable_c: public symbol_c {
  public:
    symbol_c *var_name;

    il_default_variable_c(const char *var_name_str, symbol_c *current_type);
    virtual void *accept(visitor_c &visitor);
};

/* Narrow adapter used by SFC without exposing the concrete IL generator. */
class generate_c_il_adapter_c {
  private:
    generate_c_il_c *implementation_;
    generate_c_il_adapter_c(const generate_c_il_adapter_c &);
    generate_c_il_adapter_c &operator=(const generate_c_il_adapter_c &);

  public:
    generate_c_il_adapter_c(stage4out_c *s4o_ptr, symbol_c *name, symbol_c *scope,
                            const char *variable_prefix = NULL);
    ~generate_c_il_adapter_c(void);
    visitor_c &visitor(void);
    void declare_implicit_variable_back(void);
    void print_implicit_variable_back(void);
};

void print_function_parameter_data_types(stage4out_c *s4o_ptr, symbol_c *declaration);
void generate_c_structure_initialization(stage4out_c *s4o_ptr, symbol_c *type,
                                         symbol_c *initialization);
void generate_c_array_initialization(stage4out_c *s4o_ptr, symbol_c *type,
                                     symbol_c *initialization);
void generate_c_initial_value(stage4out_c *s4o_ptr, symbol_c *type,
                              symbol_c *initialization, visitor_c &fallback);

visitor_c *new_generate_c_st_generator(stage4out_c *s4o_ptr, symbol_c *name,
                                       symbol_c *scope, const char *variable_prefix = NULL);
visitor_c *new_generate_c_inlinefcall_generator(stage4out_c *s4o_ptr, symbol_c *name,
                                                symbol_c *scope, const char *variable_prefix = NULL);
visitor_c *new_generate_c_sfc_generator(stage4out_c *s4o_ptr, symbol_c *name,
                                        symbol_c *scope, const char *variable_prefix = NULL);
visitor_c *new_generate_c_body_generator(stage4out_c *s4o_ptr, symbol_c *name,
                                         symbol_c *scope, const char *variable_prefix = NULL);
visitor_c *new_generate_c_implicit_typedecl_generator(stage4out_c *s4o_ptr);
void generate_c_variable_list(stage4out_c *s4o_ptr, symbol_c *root);
void generate_c_location_list(stage4out_c *s4o_ptr, symbol_c *root);

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

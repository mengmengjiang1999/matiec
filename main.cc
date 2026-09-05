/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *  Copyright (C) 2003-2011  Mario de Sousa (msousa@fe.up.pt)
 *  Copyright (C) 2007-2011  Laurent Bessard and Edouard Tisserant
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */

/*
 * An IEC 61131-3 compiler.
 *
 * Based on the
 * FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)
 *
 */

/*
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *********                                              *********
 *********                                              *********
 *********   O V E R A L L    A R C H I T E C T U R E   *********
 *********                                              *********
 *********                                              *********
 ****************************************************************
 ****************************************************************
 ****************************************************************

 The compiler works in 4(+1) stages:
 Stage 1   - Lexical analyser      - implemented with flex (iec.flex)
 Stage 2   - Syntax parser         - implemented with bison (iec.y)
 Stage 3   - Semantics analyser    - not yet implemented
 Stage 4   - Code generator        - implemented in C++
 Stage 4+1 - Binary code generator - gcc, javac, etc...


 Data structures passed between stages, in global variables:
 1->2   : tokens (int), and token values (char *)
 2->1   : symbol tables (defined in symtable.hh)
 2->3   : abstract syntax tree (tree of C++ classes, in absyntax.hh file)
 3->4   : Same as 2->3
 4->4+1 : file with program in c, java, etc...


 The compiler works in several passes:
 Pass 1: executes stages 1 and 2 simultaneously
 Pass 2: executes stage 3
 Pass 3: executes stage 4
 Pass 4: executes stage 4+1
*/



#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>


#include "config/config.h"
#include "compiler/compilation_context.hh"
#include "compiler/compiler.hh"
#include "stage4/stage4.hh"
#include "main.hh"


#ifndef MATIEC_REVISION
   #define MATIEC_REVISION "unknown"
#endif



void error_exit(const char *file_name, int line_no, const char *errmsg, ...) {
  va_list argptr;
  va_start(argptr, errmsg); /* second argument is last fixed pamater of error_exit() */

  fprintf(stderr, "\nInternal compiler error in file %s at line %d", file_name, line_no);
  if (errmsg != NULL) {
    fprintf(stderr, ": ");
    vfprintf(stderr, errmsg, argptr);
  } else {
    fprintf(stderr, ".");
  }
  fprintf(stderr, "\n");
  va_end(argptr);
    
  exit(EXIT_FAILURE);
}


static void printusage(const char *cmd) {
  printf("\nsyntax: %s [<options>] [-O <output_options>] [-I <include_directory>] [-T <target_directory>] <input_file>\n", cmd);
  printf(" -h : show this help message\n");
  printf(" -v : print version number\n");  
  printf(" -y : stop after syntax analysis\n");
  printf(" -f : display full token location on error messages\n");
  printf(" -p : allow use of forward references                (a non-standard extension?)\n");  
  printf(" -l : use a relaxed datatype equivalence model       (a non-standard extension?)\n");  
  printf(" -s : allow use of safe datatypes (SAFEBOOL, etc.)   (defined in PLCOpen Safety)\n"); // PLCopen TC5 "Safety Software Technical Specification - Part 1" v1.0
  printf(" -n : allow use of nested comments                   (an IEC 61131-3 v3 feature)\n");
  printf(" -r : allow use of references (REF_TO, REF, ^, NULL) (an IEC 61131-3 v3 feature)\n");
  printf(" -R : allow use of REF_TO ANY datatypes              (a non-standard extension!)\n");
  printf("        as well as REF_TO in ARRAYs and STRUCTs      (a non-standard extension!)\n");
  printf(" -a : allow use of non-literals in array size limits (a non-standard extension!)\n");
  printf(" -i : allow POUs with no in out and inout parameters (a non-standard extension!)\n");
  printf(" -b : allow functions returning VOID                 (a non-standard extension!)\n");
  printf(" -e : disable generation of implicit EN and ENO parameters.\n");
  printf(" -c : create conversion functions for enumerated data types\n");
  printf(" -O : options for output (code generation) stage. Available options for %s are...\n", cmd);
  stage4_print_options();
  printf("\n");
  printf("%s - Copyright (C) 2003-2014 \n"
         "This program comes with ABSOLUTELY NO WARRANTY!\n"
         "This is free software licensed under GPL v3, and you are welcome to redistribute it under the conditions specified by this license.\n", PACKAGE_NAME);
}


int main(int argc, char **argv) {
  matiec::CompilationContext context;
  matiec::CompilerOptions &options = context.options();
  int optres, errflg = 0;
  int path_len;
  
  /******************************************/
  /*   Parse command line options...        */
  /******************************************/
  while ((optres = getopt(argc, argv, ":nehvyfplsrRabicI:T:O:")) != -1) {
    switch(optres) {
    case 'h':
      printusage(argv[0]);
      return 0;
    case 'v':
      fprintf(stdout, "%s version %s\n" "revision: %s\n", PACKAGE_NAME, PACKAGE_VERSION, MATIEC_REVISION);
      return 0;
    case 'y': options.syntax_only = true;                       break;
    case 'l': options.relaxed_datatype_model = true;            break;
    case 'p': options.pre_parsing = true;                       break;
    case 'f': options.full_token_location = true;               break;
    case 's': options.safe_extensions = true;                   break;
    case 'R': options.reference_extensions = true;
              options.nonstandard_reference_extensions = true; break;
    case 'r': options.reference_extensions = true;              break;
    case 'a': options.nonliteral_array_size = true;              break;
    case 'b': options.allow_void_datatype = true;                break;
    case 'i': options.allow_missing_var_in = true;               break;
    case 'c': options.conversion_functions = true;               break;
    case 'n': options.nested_comments = true;                    break;
    case 'e': options.disable_implicit_en_eno = true;            break;
    case 'I':
      /* NOTE: To improve the usability under windows:
       *       We delete last char's path if it ends with "\".
       *       In this way compiler front-end accepts paths with or without
       *       slash terminator.
       */
      path_len = strlen(optarg);
      if (path_len == 0) {
        fprintf(stderr, "Option -I requires a non-empty directory\n");
        errflg++;
        break;
      }
      path_len--;
      if (optarg[path_len] == '\\') optarg[path_len]= '\0';
      options.include_directory = optarg;
      break;
    case 'T':
      /* NOTE: see note above */
      path_len = strlen(optarg);
      if (path_len == 0) {
        fprintf(stderr, "Option -T requires a non-empty directory\n");
        errflg++;
        break;
      }
      path_len--;
      if (optarg[path_len] == '\\') optarg[path_len]= '\0';
      options.output_directory = optarg;
      break;
    case 'O':
      options.generator_options = optarg;
      if (stage4_parse_options(optarg) < 0) errflg++;
      break;
    case ':':       /* -I, -T, or -O without operand */
      fprintf(stderr, "Option -%c requires an operand\n", optopt);
      errflg++;
      break;
    case '?':
      fprintf(stderr, "Unrecognized option: -%c\n", optopt);
      errflg++;
      break;
    default:
      fprintf(stderr, "Unknown error while parsing command line options.");
      errflg++;
      break;
    }
  }

  if (optind == argc) {
    fprintf(stderr, "Missing input file\n");
    errflg++;
  }

  if ((argc - optind) > 1) {
    fprintf(stderr, "Too many input files\n");
    errflg++;
  }

  if (errflg) {
    printusage(argv[0]);
    return EXIT_FAILURE;
  }

  context.set_source_path(argv[optind]);
  const matiec::CompilationResult result = matiec::Compiler().compile(context);
  context.diagnostics().render(std::cerr);
  return result.succeeded() ? EXIT_SUCCESS : EXIT_FAILURE;
}

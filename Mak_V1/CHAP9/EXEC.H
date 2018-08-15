/****************************************************************/
/*                                                              */
/*      E X E C U T O R   (Header)                              */
/*                                                              */
/*      FILE:       exec.h                                      */
/*                                                              */
/*      MODULE:     executor                                    */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#ifndef exec_h
#define exec_h

#include "common.h"

#define STATEMENT_MARKER 0x70

/*--------------------------------------------------------------*/
/*  Runtime stack                                               */
/*--------------------------------------------------------------*/

typedef union {
    int     integer;
    float   real;
    char    byte;
    ADDRESS address;
} STACK_ITEM, *STACK_ITEM_PTR;

typedef struct {
    STACK_ITEM function_value;
    STACK_ITEM static_link;
    STACK_ITEM dynamic_link;
    STACK_ITEM return_address;
} *STACK_FRAME_HEADER_PTR;

/*--------------------------------------------------------------*/
/*  Functions                                                   */
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR get_symtab_cptr();
int             get_statement_cmarker();
TYPE_STRUCT_PTR exec_routine_call();
TYPE_STRUCT_PTR exec_expression(), exec_variable();

		/************************/
		/*                      */
		/*      Macros          */
		/*                      */
		/************************/

/*--------------------------------------------------------------*/
/*  get_ctoken          Extract the next token code from the    */
/*                      current code segment.                   */
/*--------------------------------------------------------------*/

#define get_ctoken()    ctoken = *code_segmentp++

/*--------------------------------------------------------------*/
/*  pop                 Pop the runtime stack.                  */
/*--------------------------------------------------------------*/

#define pop()           --tos

/*--------------------------------------------------------------*/
/*  Tracing routine calls       Unless the following statements */
/*                              are preceded by                 */
/*                                                              */
/*                                      #define trace           */
/*                                                              */
/*                              calls to the tracing routines   */
/*                              are not compiled.               */
/*--------------------------------------------------------------*/

#ifndef trace
#define trace_routine_entry(idp)
#define trace_routine_exit(idp)
#define trace_statement_execution()
#define trace_data_store(idp, idp_tp, targetp, target_tp)
#define trace_data_fetch(idp, tp, datap)
#endif

#endif

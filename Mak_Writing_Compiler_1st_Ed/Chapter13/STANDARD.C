/****************************************************************/
/*                                                              */
/*      S T A N D A R D   R O U T I N E   P A R S E R           */
/*                                                              */
/*      Parsing routines for calls to standard procedures and   */
/*      functions.                                              */
/*                                                              */
/*      FILE:       standard.c                                  */
/*                                                              */
/*      MODULE:     parser                                      */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"
#include "symtab.h"
#include "parser.h"
#include "code.h"

#define DEFAULT_NUMERIC_FIELD_WIDTH     10
#define DEFAULT_PRECISION               2

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE       token;
extern char             word_string[];
extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;
extern TYPE_STRUCT      dummy_type;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern int              label_index;
extern char             asm_buffer[];
extern char             *asm_bufferp;
extern FILE             *code_file;

extern TOKEN_CODE       follow_parm_list[];
extern TOKEN_CODE       statement_end_list[];

/*--------------------------------------------------------------*/
/*  Forwards                                                    */
/*--------------------------------------------------------------*/

TYPE_STRUCT_PTR eof_eoln(), abs_sqr(),
		arctan_cos_exp_ln_sin_sqrt(),
		pred_succ(), chr(), odd(), ord(),
		round_trunc();

/*--------------------------------------------------------------*/
/*  standard_routine_call   Process a call to a standard        */
/*                          procedure or function.  Return a    */
/*                          pointer to the type structure of    */
/*                          the call.                           */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
standard_routine_call(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    switch (rtn_idp->defn.info.routine.key) {

	case WRITE:
	case WRITELN:   write_writeln(rtn_idp);     return(NULL);

	default:
	    error(UNIMPLEMENTED_FEATURE);
	    exit(-UNIMPLEMENTED_FEATURE);
    }
}

/*--------------------------------------------------------------*/
/*  write_writeln           Process a call to write or writeln. */
/*                          Each actual parameter can be:       */
/*                                                              */
/*                              <expr>                          */
/*                                                              */
/*                          or:                                 */
/*                                                              */
/*                              <epxr> : <expr>                 */
/*                                                              */
/*                          or:                                 */
/*                                                              */
/*                              <expr> : <expr> : <expr>        */
/*--------------------------------------------------------------*/

write_writeln(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR actual_parm_tp;     /* actual parm type */
    TYPE_STRUCT_PTR field_width_tp, precision_tp;

    /*
    --  Parameters are optional for writeln.
    */
    if (token == LPAREN) {
	do {
	    /*
	    --  Value <expr>
	    */
	    get_token();
	    actual_parm_tp = base_type(expression());

	    /*
	    --  Push the scalar value to be written onto the stack.
	    --  A string value is already on the stack.
	    */
	    if (actual_parm_tp->form != ARRAY_FORM)
		emit_push_operand(actual_parm_tp);

	    if ((actual_parm_tp->form != SCALAR_FORM) &&
		(actual_parm_tp != boolean_typep) &&
		((actual_parm_tp->form != ARRAY_FORM) ||
		 (actual_parm_tp->info.array.elmt_typep !=
						    char_typep)))
		error(INVALID_EXPRESSION);

	    /*
	    --  Optional field width <expr>
	    --  Push onto the stack.
	    */
	    if (token == COLON) {
		get_token();
		field_width_tp = base_type(expression());
		emit_1(PUSH, reg(AX));

		if (field_width_tp != integer_typep)
		    error(INCOMPATIBLE_TYPES);

		/*
		--  Optional precision <expr>
		--  Push onto the stack if the value to be printed
		--  is of type real.
		*/
		if (token == COLON) {
		    get_token();
		    precision_tp = base_type(expression());

		    if (actual_parm_tp == real_typep)
			emit_1(PUSH, reg(AX));

		    if (precision_tp != integer_typep)
			error(INCOMPATIBLE_TYPES);
		}
		else if (actual_parm_tp == real_typep) {
		    emit_2(MOVE, reg(AX),
			   integer_lit(DEFAULT_PRECISION));
		    emit_1(PUSH, reg(AX));
		}
	    }
	    else {
		if (actual_parm_tp == integer_typep) {
		    emit_2(MOVE, reg(AX),
			   integer_lit(DEFAULT_NUMERIC_FIELD_WIDTH));
		    emit_1(PUSH, reg(AX));
		}
		else if (actual_parm_tp == real_typep) {
		    emit_2(MOVE, reg(AX),
			   integer_lit(DEFAULT_NUMERIC_FIELD_WIDTH));
		    emit_1(PUSH, reg(AX));
		    emit_2(MOVE, reg(AX),
			   integer_lit(DEFAULT_PRECISION));
		    emit_1(PUSH, reg(AX));
		}
		else {
		    emit_2(MOVE, reg(AX), integer_lit(0));
		    emit_1(PUSH, reg(AX));
		}
	    }

	    if (actual_parm_tp == integer_typep) {
		emit_1(CALL, name_lit(WRITE_INTEGER));
		emit_2(ADD, reg(SP), integer_lit(4));
	    }
	    else if (actual_parm_tp == real_typep) {
		emit_1(CALL, name_lit(WRITE_REAL));
		emit_2(ADD, reg(SP), integer_lit(8));
	    }
	    else if (actual_parm_tp == boolean_typep) {
		emit_1(CALL, name_lit(WRITE_BOOLEAN));
		emit_2(ADD, reg(SP), integer_lit(4));
	    }
	    else if (actual_parm_tp == char_typep) {
		emit_1(CALL, name_lit(WRITE_CHAR));
		emit_2(ADD, reg(SP), integer_lit(4));
	    }
	    else  /* string */  {
		/*
		--  Push the string length onto the stack.
		*/
		emit_2(MOVE, reg(AX),
		       integer_lit(actual_parm_tp->info.array
						       .elmt_count));

		emit_1(PUSH, reg(AX));
		emit_1(CALL, name_lit(WRITE_STRING));
		emit_2(ADD, reg(SP), integer_lit(6));
	    }

	    /*
	    --  Error synchronization:  Should be , or )
	    */
	    synchronize(follow_parm_list, statement_end_list, NULL);

	} while (token == COMMA);

	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else if (rtn_idp->defn.info.routine.key == WRITE)
	error(WRONG_NUMBER_OF_PARMS);

    if (rtn_idp->defn.info.routine.key == WRITELN)
	emit_1(CALL, name_lit(WRITE_LINE));
}

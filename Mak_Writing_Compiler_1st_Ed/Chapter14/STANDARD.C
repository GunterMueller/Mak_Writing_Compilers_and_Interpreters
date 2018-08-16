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

	case READ:
	case READLN:    read_readln(rtn_idp);       return(NULL);

	case WRITE:
	case WRITELN:   write_writeln(rtn_idp);     return(NULL);

	case EOFF:
	case EOLN:      return(eof_eoln(rtn_idp));

	case ABS:
	case SQR:       return(abs_sqr(rtn_idp));

	case ARCTAN:
	case COS:
	case EXP:
	case LN:
	case SIN:
	case SQRT:      return(arctan_cos_exp_ln_sin_sqrt(rtn_idp));

	case PRED:
	case SUCC:      return(pred_succ(rtn_idp));

	case CHR:       return(chr());
	case ODD:       return(odd());
	case ORD:       return(ord());

	case ROUND:
	case TRUNC:     return(round_trunc(rtn_idp));
    }
}

/*--------------------------------------------------------------*/
/*  read_readln             Process a call to read or readln.   */
/*--------------------------------------------------------------*/

read_readln(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR actual_parm_tp;     /* actual parm type */

    /*
    --  Parameters are optional for readln.
    */
    if (token == LPAREN) {
	/*
	--  <id-list>
	*/
	do {
	    get_token();

	    /*
	    --  Actual parms must be variables (but parse
	    --  an expression anyway for error recovery).
	    */
	    if (token == IDENTIFIER) {
		SYMTAB_NODE_PTR idp;

		search_and_find_all_symtab(idp);
		actual_parm_tp = base_type(variable(idp,
						    VARPARM_USE));

		if (actual_parm_tp->form != SCALAR_FORM)
		    error(INCOMPATIBLE_TYPES);
		else if (actual_parm_tp == integer_typep) {
		    emit_1(CALL, name_lit(READ_INTEGER));
		    emit_1(POP,  reg(BX));
		    emit_2(MOVE, word_indirect(BX), reg(AX));
		}
		else if (actual_parm_tp == real_typep) {
		    emit_1(CALL, name_lit(READ_REAL));
		    emit_1(POP,  reg(BX));
		    emit_2(MOVE, word_indirect(BX), reg(AX));
		    emit_2(MOVE, high_dword_indirect(BX), reg(DX));
		}
		else if (actual_parm_tp == char_typep) {
		    emit_1(CALL, name_lit(READ_CHAR));
		    emit_1(POP,  reg(BX));
		    emit_2(MOVE, byte_indirect(BX), reg(AL));
		}
	    }
	    else {
		actual_parm_tp = expression();
		error(INVALID_VAR_PARM);
	    }

	    /*
	    --  Error synchronization:  Should be , or )
	    */
	    synchronize(follow_parm_list, statement_end_list, NULL);

	} while (token == COMMA);

	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else if (rtn_idp->defn.info.routine.key == READ)
	error(WRONG_NUMBER_OF_PARMS);

    if (rtn_idp->defn.info.routine.key == READLN)
	emit_1(CALL, name_lit(READ_LINE));
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

/*--------------------------------------------------------------*/
/*  eof_eoln                Process a call to eof or to eoln.   */
/*                          No parameters => boolean result.    */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
eof_eoln(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    if (token == LPAREN) {
	error(WRONG_NUMBER_OF_PARMS);
	actual_parm_list(rtn_idp, FALSE);
    }

    emit_1(CALL, name_lit(rtn_idp->defn.info.routine.key == EOFF
			      ? STD_END_OF_FILE
			      : STD_END_OF_LINE));

    return(boolean_typep);
}

/*--------------------------------------------------------------*/
/*  abs_sqr                 Process a call to abs or to sqr.    */
/*                          integer parm => integer result      */
/*                          real parm    => real result         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
abs_sqr(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */
    TYPE_STRUCT_PTR result_tp;          /* result type */

    if (token == LPAREN) {
	get_token();
	parm_tp = base_type(expression());

	if ((parm_tp != integer_typep) && (parm_tp != real_typep)) {
	    error(INCOMPATIBLE_TYPES);
	    result_tp = real_typep;
	}
	else result_tp = parm_tp;

	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else error(WRONG_NUMBER_OF_PARMS);

    switch (rtn_idp->defn.info.routine.key) {

	case ABS:
	    if (parm_tp == integer_typep) {
		int nonnegative_labelx = new_label_index();

		emit_2(COMPARE, reg(AX), integer_lit(0));
		emit_1(JUMP_GE, label(STMT_LABEL_PREFIX,
				      nonnegative_labelx));
		emit_1(NEGATE, reg(AX));
		emit_label(STMT_LABEL_PREFIX, nonnegative_labelx);
	    }
	    else {
		emit_push_operand(parm_tp);
		emit_1(CALL, name_lit(STD_ABS));
		emit_2(ADD, reg(SP), integer_lit(4));
	    }
	    break;

	case SQR:
	    if (parm_tp == integer_typep) {
		emit_2(MOVE, reg(DX), reg(AX));
		emit_1(MULTIPLY, reg(DX));
	    }
	    else {
		emit_push_operand(parm_tp);
		emit_push_operand(parm_tp);
		emit_1(CALL, name_lit(FLOAT_MULTIPLY));
		emit_2(ADD, reg(SP), integer_lit(8));
	    }
	    break;
	}

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  arctan_cos_exp_ln_sin_sqrt  Process a call to arctan, cos,  */
/*                              exp, ln, sin, or sqrt.          */
/*                              integer parm => real result     */
/*                              real_parm    => real result     */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
arctan_cos_exp_ln_sin_sqrt(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */
    char            *std_func_name;     /* name of standard func */

    if (token == LPAREN) {
	get_token();
	parm_tp = base_type(expression());

	if ((parm_tp != integer_typep) && (parm_tp != real_typep))
	    error(INCOMPATIBLE_TYPES);

	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else error(WRONG_NUMBER_OF_PARMS);

    if (parm_tp == integer_typep) {
	emit_1(PUSH, reg(AX));
	emit_1(CALL, name_lit(FLOAT_CONVERT));
	emit_2(ADD,  reg(SP), integer_lit(2));
    }

    emit_push_operand(real_typep);

    switch (rtn_idp->defn.info.routine.key) {
	case ARCTAN:    std_func_name = STD_ARCTAN;     break;
	case COS:       std_func_name = STD_COS;        break;
	case EXP:       std_func_name = STD_EXP;        break;
	case LN:        std_func_name = STD_LN;         break;
	case SIN:       std_func_name = STD_SIN;        break;
	case SQRT:      std_func_name = STD_SQRT;       break;
    }

    emit_1(CALL, name_lit(std_func_name));
    emit_2(ADD,  reg(SP), integer_lit(4));

    return(real_typep);
}

/*--------------------------------------------------------------*/
/*  pred_succ               Process a call to pred or succ.     */
/*                          integer parm => integer result      */
/*                          enum parm    => enum result         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
pred_succ(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */
    TYPE_STRUCT_PTR result_tp;          /* result type */

    if (token == LPAREN) {
	get_token();
	parm_tp = base_type(expression());

	if ((parm_tp != integer_typep) &&
	    (parm_tp->form != ENUM_FORM)) {
	    error(INCOMPATIBLE_TYPES);
	    result_tp = integer_typep;
	}
	else result_tp = parm_tp;

	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else error(WRONG_NUMBER_OF_PARMS);

    emit_1(rtn_idp->defn.info.routine.key == PRED
	       ? DECREMENT : INCREMENT,
	   reg(AX));

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  chr                     Process a call to chr.              */
/*                          integer parm => character result    */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
chr()

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */

    if (token == LPAREN) {
	get_token();
	parm_tp = base_type(expression());

	if (parm_tp != integer_typep) error(INCOMPATIBLE_TYPES);
	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else error(WRONG_NUMBER_OF_PARMS);

    return(char_typep);
}

/*--------------------------------------------------------------*/
/*  odd                     Process a call to odd.              */
/*                          integer parm => boolean result      */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
odd()

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */

    if (token == LPAREN) {
	get_token();
	parm_tp = base_type(expression());

	if (parm_tp != integer_typep) error(INCOMPATIBLE_TYPES);
	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else error(WRONG_NUMBER_OF_PARMS);

    emit_2(AND_BITS, reg(AX), integer_lit(1));
    return(boolean_typep);
}

/*--------------------------------------------------------------*/
/*  ord                     Process a call to ord.              */
/*                          enumeration parm => integer result  */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
ord()

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */

    if (token == LPAREN) {
	get_token();
	parm_tp = base_type(expression());

	if ((parm_tp->form != ENUM_FORM) &&             /* 2/9/91 */
	    (parm_tp != char_typep))
	    error(INCOMPATIBLE_TYPES);
	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else error(WRONG_NUMBER_OF_PARMS);

    return(integer_typep);
}

/*--------------------------------------------------------------*/
/*  round_trunc             Process a call to round or trunc.   */
/*                          real parm => integer result         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
round_trunc(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */

    if (token == LPAREN) {
	get_token();
	parm_tp = base_type(expression());

	if (parm_tp != real_typep) error(INCOMPATIBLE_TYPES);
	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else error(WRONG_NUMBER_OF_PARMS);

    emit_push_operand(parm_tp);
    emit_1(CALL, name_lit(rtn_idp->defn.info.routine.key == ROUND
			      ? STD_ROUND : STD_TRUNC));
    emit_2(ADD, reg(SP), integer_lit(4));

    return(integer_typep);
}


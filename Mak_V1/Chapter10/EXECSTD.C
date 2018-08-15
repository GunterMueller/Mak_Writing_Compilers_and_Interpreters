/****************************************************************/
/*                                                              */
/*      S T A N D A R D   R O U T I N E   E X E C U T O R       */
/*                                                              */
/*      Execution routines for statements.                      */
/*                                                              */
/*      FILE:       execstd.c                                   */
/*                                                              */
/*      MODULE:     executor                                    */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include <math.h>
#include "common.h"
#include "error.h"
#include "symtab.h"
#include "scanner.h"
#include "parser.h"
#include "exec.h"

#define EOF_CHAR                        '\x7f'

#define DEFAULT_NUMERIC_FIELD_WIDTH     10
#define DEFAULT_PRECISION               2

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern int            level;
extern int            exec_line_number;

extern char           *code_segmentp;
extern TOKEN_CODE     ctoken;

extern STACK_ITEM     *stack;
extern STACK_ITEM_PTR tos;
extern STACK_ITEM_PTR stack_frame_basep;
extern STACK_ITEM_PTR stack_display[];

extern TYPE_STRUCT_PTR integer_typep, real_typep,
		       boolean_typep, char_typep;

/*--------------------------------------------------------------*/
/*  Forwards                                                    */
/*--------------------------------------------------------------*/

TYPE_STRUCT_PTR exec_eof_eoln(), exec_abs_sqr(),
		exec_arctan_cos_exp_ln_sin_sqrt(),
		exec_pred_succ(), exec_chr(),
		exec_odd(), exec_ord(), exec_round_trunc();

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

BOOLEAN eof_flag = FALSE;

/*--------------------------------------------------------------*/
/*  exec_standard_routine_call  Execute a call to a standard    */
/*                              procedure or function.  Return  */
/*                              a pointer to the type structure */
/*                              of the call.                    */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_standard_routine_call(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    switch (rtn_idp->defn.info.routine.key) {

	case READ:
	case READLN:    exec_read_readln(rtn_idp);      return(NULL);

	case WRITE:
	case WRITELN:   exec_write_writeln(rtn_idp);    return(NULL);

	case EOFF:
	case EOLN:      return(exec_eof_eoln(rtn_idp));

	case ABS:
	case SQR:       return(exec_abs_sqr(rtn_idp));

	case ARCTAN:
	case COS:
	case EXP:
	case LN:
	case SIN:
	case SQRT:      return(exec_arctan_cos_exp_ln_sin_sqrt
				   (rtn_idp));

	case PRED:
	case SUCC:      return(exec_pred_succ(rtn_idp));

	case CHR:       return(exec_chr());
	case ODD:       return(exec_odd());
	case ORD:       return(exec_ord());

	case ROUND:
	case TRUNC:     return(exec_round_trunc(rtn_idp));
    }
}

/*--------------------------------------------------------------*/
/*  exec_read_readln        Execute a call to read or readln.   */
/*--------------------------------------------------------------*/

exec_read_readln(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    SYMTAB_NODE_PTR parm_idp;           /* parm id */
    TYPE_STRUCT_PTR parm_tp;            /* parm type */
    STACK_ITEM_PTR  targetp;            /* ptr to read target */

    /*
    --  Parameters are optional for readln.
    */
    get_ctoken();
    if (ctoken == LPAREN) {
	/*
	--  <id-list>
	*/
	do {
	    get_ctoken();
	    parm_idp = get_symtab_cptr();
	    parm_tp  = base_type(exec_variable(parm_idp,
					       VARPARM_USE));
	    targetp  = (STACK_ITEM_PTR) tos->address;

	    pop();      /* pop off address */

	    if (parm_tp == integer_typep)
		scanf("%d", &targetp->integer);
	    else if (parm_tp == real_typep)
		scanf("%g", &targetp->real);

	    else if (parm_tp == char_typep) {
		scanf("%c", &targetp->byte);
		if (eof_flag ||
		    (targetp->byte == '\n')) targetp->byte = ' ';
	    }

	    trace_data_store(parm_idp, parm_idp->typep,
			     targetp, parm_tp);
	} while (ctoken == COMMA);

	get_ctoken();   /* token after ) */
    }

    if (rtn_idp->defn.info.routine.key == READLN) {
	char ch;

	do {
	    ch = getchar();
	} while(!eof_flag && (ch != '\n'));
    }
}

/*--------------------------------------------------------------*/
/*  exec_write_writeln      Execute a call to write or writeln. */
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

exec_write_writeln(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* parm type */
    int             field_width;
    int             precision;

    /*
    --  Parameters are optional for writeln.
    */
    get_ctoken();
    if (ctoken == LPAREN) {
	do {
	    /*
	    --  Push value
	    */
	    get_ctoken();
	    parm_tp = base_type(exec_expression());

	    if (parm_tp == integer_typep)
		field_width = DEFAULT_NUMERIC_FIELD_WIDTH;
	    else if (parm_tp == real_typep) {
		field_width = DEFAULT_NUMERIC_FIELD_WIDTH;
		precision   = DEFAULT_PRECISION;
	    }
	    else field_width = 0;

	    /*
	    --  Optional field width <expr>
	    */
	    if (ctoken == COLON) {
		get_ctoken();
		exec_expression();
		field_width = tos->integer;
		pop();          /* pop off field width */

		/*
		--  Optional decimal places <expr>
		*/
		if (ctoken == COLON) {
		    get_ctoken();
		    exec_expression();
		    precision = tos->integer;
		    pop();      /* pop off precision */
		}
	    }

	    /*
	    --  Write value
	    */
	    if (parm_tp == integer_typep)
		printf("%*d", field_width, tos->integer);
	    else if (parm_tp == real_typep)
		printf("%*.*f", field_width, precision, tos->real);
	    else if (parm_tp == boolean_typep)
		printf("%*s", field_width, tos->integer == 1
					       ? "TRUE" : "FALSE");
	    else if (parm_tp == char_typep)
		printf("%*c", field_width, tos->byte);

	    else if (parm_tp->form == ARRAY_FORM) {
		char buffer[MAX_SOURCE_LINE_LENGTH];

		strncpy(buffer, tos->address,
				parm_tp->info.array.elmt_count);
		buffer[parm_tp->info.array.elmt_count] = '\0';
		printf("%*s", -field_width, buffer);
	    }

	    pop();      /* pop off value */
	} while (ctoken == COMMA);

	get_ctoken();   /* token after ) */
    }

    if (rtn_idp->defn.info.routine.key == WRITELN) putchar('\n');
}

/*--------------------------------------------------------------*/
/*  exec_eof_eoln           Execute a call to eof or to eoln.   */
/*                          No parameters => boolean result.    */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_eof_eoln(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    char ch = getchar();

    switch (rtn_idp->defn.info.routine.key) {

	case EOFF:
	    if (eof_flag || feof(stdin)) {
		eof_flag = TRUE;
		push_integer(1);
	    }
	    else {
		push_integer(0);
		ungetc(ch, stdin);
	    }
	    break;

	case EOLN:
	    if (eof_flag || feof(stdin)) {
		eof_flag = TRUE;
		push_integer(1);
	    }
	    else {
		push_integer(ch == '\n' ? 1 : 0);
		ungetc(ch, stdin);
	    }
	    break;
    }

    get_ctoken();       /* token after function name */
    return(boolean_typep);
}

/*--------------------------------------------------------------*/
/*  exec_abs_sqr            Execute a call to abs or to sqr.    */
/*                          integer parm => integer result      */
/*                          real parm    => real result         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_abs_sqr(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */
    TYPE_STRUCT_PTR result_tp;          /* result type */

    get_ctoken();       /* ( */
    get_ctoken();
    parm_tp = base_type(exec_expression());

    if (parm_tp == integer_typep) {
	tos->integer = rtn_idp->defn.info.routine.key == ABS
			   ? abs(tos->integer)
			   : tos->integer * tos->integer;
	result_tp = integer_typep;
    }
    else {
	tos->real = rtn_idp->defn.info.routine.key == ABS
			   ? fabs(tos->real)
			   : tos->real * tos->real;
	result_tp = real_typep;
    }

    get_ctoken();       /* token after ) */
    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  exec_arctan_cos_exp_ln_sin_sqrt Execute a call to arctan,   */
/*                                  cos, exp, ln, sin, or sqrt. */
/*                                  integer parm => real result */
/*                                  real_parm    => real result */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_arctan_cos_exp_ln_sin_sqrt(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */
    int             code = rtn_idp->defn.info.routine.key;

    get_ctoken();       /* ( */
    get_ctoken();
    parm_tp = base_type(exec_expression());
    if (parm_tp == integer_typep) tos->real = tos->integer;

    if (   ((code == LN)   && (tos->real <= 0.0))
	|| ((code == SQRT) && (tos->real <  0.0)))
	runtime_error(INVALID_FUNCTION_ARGUMENT);
    else {
	switch (rtn_idp->defn.info.routine.key) {
	    case ARCTAN:    tos->real = atan(tos->real);    break;
	    case COS:       tos->real = cos(tos->real);     break;
	    case EXP:       tos->real = exp(tos->real);     break;
	    case LN:        tos->real = log(tos->real);     break;
	    case SIN:       tos->real = sin(tos->real);     break;
	    case SQRT:      tos->real = sqrt(tos->real);    break;
	}
    }

    get_ctoken();       /* token after ) */
    return(real_typep);
}

/*--------------------------------------------------------------*/
/*  exec_pred_succ          Execute a call to pred or succ.     */
/*                          integer parm => integer result      */
/*                          enum parm    => enum result         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_pred_succ(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    TYPE_STRUCT_PTR parm_tp;            /* actual parameter type */

    get_ctoken();       /* ( */
    get_ctoken();
    parm_tp = base_type(exec_expression());

    tos->integer = rtn_idp->defn.info.routine.key == PRED
		       ? --tos->integer
		       : ++tos->integer;

    get_ctoken();       /* token after ) */
    return(parm_tp);
}

/*--------------------------------------------------------------*/
/*  exec_chr                Execute a call to chr.              */
/*                          integer parm => character result    */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_chr()

{
    get_ctoken();       /* ( */
    get_ctoken();
    exec_expression();

    tos->byte = tos->integer;

    get_ctoken();       /* token after ) */
    return(char_typep);
}

/*--------------------------------------------------------------*/
/*  exec_odd                Execute a call to odd.              */
/*                          integer parm => boolean result      */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_odd()

{
    get_ctoken();       /* ( */
    get_ctoken();
    exec_expression();

    tos->integer &= 1;

    get_ctoken();       /* token after ) */
    return(boolean_typep);
}

/*--------------------------------------------------------------*/
/*  exec_ord                Execute a call to ord.              */
/*                          enumeration parm => integer result  */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_ord()

{
    get_ctoken();       /* ( */
    get_ctoken();
    exec_expression();

    get_ctoken();       /* token after ) */
    return(integer_typep);
}

/*--------------------------------------------------------------*/
/*  exec_round_trunc        Execute a call to round or trunc.   */
/*                          real parm => integer result         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_round_trunc(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */

{
    get_ctoken();       /* ( */
    get_ctoken();
    exec_expression();

    if (rtn_idp->defn.info.routine.key == ROUND) {
	tos->integer = tos->real > 0.0
			   ? (int) (tos->real + 0.5)
			   : (int) (tos->real - 0.5);
    }
    else tos->integer = (int) tos->real;

    get_ctoken();       /* token after ) */
    return(integer_typep);
}




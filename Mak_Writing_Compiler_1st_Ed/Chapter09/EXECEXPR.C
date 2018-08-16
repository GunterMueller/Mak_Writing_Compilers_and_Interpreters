/****************************************************************/
/*                                                              */
/*      E X P R E S S I O N   E X E C U T O R                   */
/*                                                              */
/*      Execution routines for expressions.                     */
/*                                                              */
/*      FILE:       execexpr.c                                  */
/*                                                              */
/*      MODULE:     executor                                    */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"
#include "symtab.h"
#include "scanner.h"
#include "parser.h"
#include "exec.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern int            level;

extern char           *code_segmentp;
extern TOKEN_CODE     ctoken;

extern STACK_ITEM     *stack;
extern STACK_ITEM_PTR tos;
extern STACK_ITEM_PTR stack_frame_basep;

extern TYPE_STRUCT_PTR integer_typep, real_typep,
		       boolean_typep, char_typep;

/*--------------------------------------------------------------*/
/*  Forwards                                                    */
/*--------------------------------------------------------------*/

TYPE_STRUCT_PTR exec_expression(), exec_simple_expression(),
		exec_term(), exec_factor(),
		exec_constant(), exec_variable(),
		exec_subscripts(), exec_field();

/*--------------------------------------------------------------*/
/*  exec_expression     Execute an expression consisting of a   */
/*                      simple expression optionally followed   */
/*                      by a relational operator and a second   */
/*                      simple expression.  Return a pointer to */
/*                      the type structure.                     */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_expression()

{
    STACK_ITEM_PTR  operandp1, operandp2;   /* ptrs to operands */
    TYPE_STRUCT_PTR result_tp, tp2;         /* ptrs to types */
    TOKEN_CODE      op;                     /* an operator token */
    BOOLEAN         result;

    result_tp = exec_simple_expression();   /* first simple expr */

    /*
    --  If there is a relational operator, remember it and
    --  process the second simple expression.
    */
    if ((ctoken == EQUAL) || (ctoken == LT) || (ctoken == GT) ||
	(ctoken == NE)    || (ctoken == LE) || (ctoken == GE)) {
	op = ctoken;                           /* remember operator */
	result_tp = base_type(result_tp);

	get_ctoken();
	tp2 = base_type(exec_simple_expression()); /* 2nd simp expr */

	operandp1 = tos - 1;
	operandp2 = tos;

	/*
	--  Both operands are integer, boolean, or enumeration.
	*/
	if (   ((result_tp == integer_typep) &&
		(tp2       == integer_typep))
	    || (result_tp->form == ENUM_FORM)) {
	    switch (op) {
		case EQUAL:
		    result = operandp1->integer == operandp2->integer;
		    break;

		case LT:
		    result = operandp1->integer <  operandp2->integer;
		    break;

		case GT:
		    result = operandp1->integer >  operandp2->integer;
		    break;

		case NE:
		    result = operandp1->integer != operandp2->integer;
		    break;

		case LE:
		    result = operandp1->integer <= operandp2->integer;
		    break;

		case GE:
		    result = operandp1->integer >= operandp2->integer;
		    break;
	    }
	}

	/*
	--  Both operands are character.
	*/
	else if (result_tp == char_typep) {
	    switch (op) {
		case EQUAL:
		    result = operandp1->byte == operandp2->byte;
		    break;

		case LT:
		    result = operandp1->byte <  operandp2->byte;
		    break;

		case GT:
		    result = operandp1->byte >  operandp2->byte;
		    break;

		case NE:
		    result = operandp1->byte != operandp2->byte;
		    break;

		case LE:
		    result = operandp1->byte <= operandp2->byte;
		    break;

		case GE:
		    result = operandp1->byte >= operandp2->byte;
		    break;
	    }
	}

	/*
	--  Both operands are real, or one is real and the other
	--  is integer.  Convert the integer operand to real.
	*/
	else if ((result_tp == real_typep) ||
		 (tp2       == real_typep)) {
	    promote_operands_to_real(operandp1, result_tp,
				     operandp2, tp2);

	    switch (op) {
		case EQUAL:
		    result = operandp1->real == operandp2->real;
		    break;

		case LT:
		    result = operandp1->real <  operandp2->real;
		    break;

		case GT:
		    result = operandp1->real >  operandp2->real;
		    break;

		case NE:
		    result = operandp1->real != operandp2->real;
		    break;

		case LE:
		    result = operandp1->real <= operandp2->real;
		    break;

		case GE:
		    result = operandp1->real >= operandp2->real;
		    break;
	    }
	}

	/*
	--  Both operands are strings.
	*/
	else if ((result_tp->form == ARRAY_FORM) &&
		 (result_tp->info.array.elmt_typep == char_typep)) {
	    int cmp = strncmp(operandp1->address, operandp2->address,
			      result_tp->info.array.elmt_count);

	    result = (   (   (cmp < 0)
			  && (   (op == NE)
			      || (op == LE)
			      || (op == LT)))
		      || (   (cmp == 0)
			  && (   (op == EQUAL)
			      || (op == LE)
			      || (op == GE)))
		      || (   (cmp > 0)
			  && (   (op == NE)
			      || (op == GE)
			      || (op == GT))));
	}

	/*
	--  Replace the two operands on the stack with the result.
	*/
	operandp1->integer = result ? 1 : 0;
	pop();

	result_tp = boolean_typep;
    }

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  exec_simple_expression  Execute a simple expression         */
/*                          consisting of terms separated by +, */
/*                          -, or OR operators.  There may be   */
/*                          a unary + or - before the first     */
/*                          term.  Return a pointer to the      */
/*                          type structure.                     */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_simple_expression()

{
    STACK_ITEM_PTR operandp1, operandp2;    /* ptrs to operands */
    TYPE_STRUCT_PTR result_tp, tp2;         /* ptrs to types */
    TOKEN_CODE op;                          /* an operator token */
    TOKEN_CODE unary_op = PLUS;             /* unary operator token */

    /*
    --  If there is a unary + or -, remember it.
    */
    if ((ctoken == PLUS) || (ctoken == MINUS)) {
	unary_op = ctoken;
	get_ctoken();
    }

    result_tp = exec_term();    /* first term */

    /*
    --  If there was a unary -, negate the top of stack
    */
    if (unary_op == MINUS) {
	if (result_tp == integer_typep) tos->integer = -tos->integer;
	else                            tos->real    = -tos->real;
    }

    /*
    --  Loop to process subsequent terms
    --  separated by operators.
    */
    while ((ctoken == PLUS) || (ctoken == MINUS) || (ctoken == OR)) {
	op = ctoken;                    /* remember operator */
	result_tp = base_type(result_tp);

	get_ctoken();
	tp2 = base_type(exec_term());   /* subsequent term */

	operandp1 = tos - 1;
	operandp2 = tos;

	/*
	--  OR
	*/
	if (op == OR) {
	    operandp1->integer = operandp1->integer ||
				 operandp2->integer;
	    result_tp = boolean_typep;
	}

	/*
	--  + or -
	--
	--  Both operands are integer.
	*/
	else if ((result_tp == integer_typep) &&
		 (tp2       == integer_typep)) {
	    operandp1->integer = (op == PLUS)
		? operandp1->integer + operandp2->integer
		: operandp1->integer - operandp2->integer;
	    result_tp = integer_typep;
	}

	/*
	--  Both operands are real, or one is real and the other
	--  is integer.  Convert the integer operand to real.
	*/
	else {
	    promote_operands_to_real(operandp1, result_tp,
				     operandp2, tp2);

	    operandp1->real = (op == PLUS)
		? operandp1->real + operandp2->real
		: operandp1->real - operandp2->real;
	    result_tp = real_typep;
	}

	pop();  /* pop off the second operand */
    }

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  exec_term           Execute a term consisting of factors    */
/*                      separated by *, /, DIV, MOD, or AND     */
/*                      operators.  Return a pointer to the     */
/*                      type structure.                         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_term()

{
    STACK_ITEM_PTR operandp1, operandp2;    /* ptrs to operands */
    TYPE_STRUCT_PTR result_tp, tp2;         /* ptrs to types */
    TOKEN_CODE op;                          /* an operator token */

    result_tp = exec_factor();  /* first factor */

    /*
    --  Loop to process subsequent factors
    --  separated by operators.
    */
    while ((ctoken == STAR) || (ctoken == SLASH) || (ctoken == DIV) ||
	   (ctoken == MOD)  || (ctoken == AND)) {
	op = ctoken;                        /* remember operator */
	result_tp = base_type(result_tp);

	get_ctoken();
	tp2 = base_type(exec_factor());     /* subsequent factor */

	operandp1 = tos - 1;
	operandp2 = tos;

	/*
	--  AND
	*/
	if (op == AND) {
	    operandp1->integer = operandp1->integer &&
				 operandp2->integer;
	    result_tp = boolean_typep;
	}

	/*
	--  *, /, DIV, or MOD
	*/
	else switch (op) {

	    case STAR:
		/*
		--  Both operands are integer.
		*/
		if (   (result_tp == integer_typep)
		    && (tp2       == integer_typep)) {
		    operandp1->integer =
			operandp1->integer * operandp2->integer;
		    result_tp = integer_typep;
		}

		/*
		--  Both operands are real, or one is real and the
		--  other is integer.  Convert the integer operand
		--  to real.
		*/
		else {
		    promote_operands_to_real(operandp1, result_tp,
					     operandp2, tp2);

		    operandp1->real =
			operandp1->real * operandp2->real;
		    result_tp = real_typep;
		}
		break;

	    case SLASH:
		/*
		--  Both operands are real, or one is real and the
		--  other is integer.  Convert the integer operand
		--  to real.
		*/
		promote_operands_to_real(operandp1, result_tp,
					 operandp2, tp2);

		if (operandp2->real == 0.0)
		    runtime_error(DIVISION_BY_ZERO);
		else
		    operandp1->real = operandp1->real/operandp2->real;

		result_tp = real_typep;
		break;

	    case DIV:
	    case MOD:
		/*
		--  Both operands are integer.
		*/
		if (operandp2->integer == 0)
		    runtime_error(DIVISION_BY_ZERO);
		else
		    operandp1->integer = (op == DIV)
			? operandp1->integer / operandp2->integer
			: operandp1->integer % operandp2->integer;

		result_tp = integer_typep;
		break;
	}

	pop();  /* pop off the second operand */
    }

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  exec_factor         Execute a factor, which is an variable, */
/*                      a number, NOT followed by a factor, or  */
/*                      a parenthesized subexpression.  Return  */
/*                      a pointer to the type structure.        */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_factor()

{
    TYPE_STRUCT_PTR result_tp;          /* type pointer */

    switch (ctoken) {

	case IDENTIFIER: {
	    SYMTAB_NODE_PTR idp = get_symtab_cptr();

	    /*
	    --  Function call or constant or variable.
	    */
	    if (idp->defn.key == FUNC_DEFN)
		result_tp = exec_routine_call(idp);
	    else if (idp->defn.key == CONST_DEFN)
		result_tp = exec_constant(idp);
	    else
		result_tp = exec_variable(idp, EXPR_USE);

	    break;
	}

	case NUMBER: {
	    SYMTAB_NODE_PTR np = get_symtab_cptr();

	    /*
	    --  Obtain the integer or real value from the
	    --  symbol table entry and push it onto the stack.
	    */
	    if (np->typep == integer_typep) {
		push_integer(np->defn.info.constant.value.integer);
		result_tp = integer_typep;
	    }
	    else {
		push_real(np->defn.info.constant.value.real);
		result_tp = real_typep;
	    }

	    get_ctoken();
	    break;
	}

	case STRING: {
	    SYMTAB_NODE_PTR np     = get_symtab_cptr();
	    int             length = strlen(np->name);

	    /*
	    --  Obtain the character or string from the symbol
	    --  table entry.  Note that the quotes were included,
	    --  so the string lengths need to be decreased by 2.
	    */
	    if (length > 3) {
		/*
		--  String:  Push its address onto the stack.
		*/
		push_address(np->info);
		result_tp = np->typep;
	    }
	    else {
		/*
		--  Character:  Push its value onto the stack.
		*/
		push_byte(np->name[1]);
		result_tp = char_typep;
	    }

	    get_ctoken();
	    break;
	}

	case NOT:
	    get_ctoken();
	    result_tp = exec_factor();
	    tos->integer = 1 - tos->integer;    /* 0 => 1, 1 => 0 */
	    break;

	case LPAREN:
	    get_ctoken();
	    result_tp = exec_expression();
	    get_ctoken();       /* token after ) */
	    break;
    }

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  exec_constant       Push the value of a non-string constant */
/*                      identifier, or the address of the value */
/*                      a string constant identifier onto the   */
/*                      stack.  Return a pointer to the type    */
/*                      structure.                              */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_constant(idp)

    SYMTAB_NODE_PTR idp;         /* constant id */

{
    TYPE_STRUCT_PTR tp = idp->typep;

    if ((base_type(tp) == integer_typep) || (tp->form == ENUM_FORM))
	push_integer(idp->defn.info.constant.value.integer);
    else if (tp == real_typep)
	push_real(idp->defn.info.constant.value.real);
    else if (tp == char_typep)
	push_integer(idp->defn.info.constant.value.integer);
    else if (tp->form == ARRAY_FORM)
	push_address(idp->defn.info.constant.value.stringp);

    trace_data_fetch(idp, tp, tos);
    get_ctoken();

    return(tp);
}

/*--------------------------------------------------------------*/
/*  exec_variable       Push either the variable's address or   */
/*                      its value onto the stack.  Return a     */
/*                      pointer to the type structure.          */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_variable(idp, use)

    SYMTAB_NODE_PTR idp;    /* variable id */
    USE             use;    /* how variable is used */

{
    int             delta;          /* difference in levels */
    TYPE_STRUCT_PTR tp = idp->typep;
    TYPE_STRUCT_PTR base_tp;
    STACK_ITEM_PTR  datap;          /* ptr to data area */
    STACK_FRAME_HEADER_PTR hp;

    /*
    --  Point to the variable's stack item.  If the variable's level
    --  is less than the current level, follow the static links to
    --  the appropriate stack frame base.
    */
    hp = (STACK_FRAME_HEADER_PTR) stack_frame_basep;
    delta = level - idp->level;
    while (delta-- > 0)
	hp = (STACK_FRAME_HEADER_PTR) hp->static_link.address;
    datap = (STACK_ITEM_PTR) hp + idp->defn.info.data.offset;

    /*
    --  If a scalar or enumeration VAR parm, that item
    --  points to the actual item.
    */
    if ((idp->defn.key == VARPARM_DEFN) &&
	(tp->form != ARRAY_FORM) &&
	(tp->form != RECORD_FORM))
	datap = (STACK_ITEM_PTR) datap->address;

    /*
    --  Push the address of the variable's data area.
    */
    if ((tp->form == ARRAY_FORM) ||
	(tp->form == RECORD_FORM))
	push_address((ADDRESS) datap->address);
    else
	push_address((ADDRESS) datap);

    /*
    --  If there are subscripts or field designators,
    --  modify the address to point to the array element
    --  record field.
    */
    get_ctoken();
    while ((ctoken == LBRACKET) || (ctoken == PERIOD)) {
	if      (ctoken == LBRACKET) tp = exec_subscripts(tp);
	else if (ctoken == PERIOD)   tp = exec_field();
    }

    base_tp = base_type(tp);

    /*
    --  Leave the modified address on top of the stack if:
    --      it is an assignment target, or
    --      it represents a parameter passed by reference, or
    --      it is the address of an array or record.
    --  Otherwise, replace the address with the value that it
    --  points to.
    */
    if ((use != TARGET_USE) && (use != VARPARM_USE) &&
	(tp->form != ARRAY_FORM) && (tp->form != RECORD_FORM)) {

	if ((base_tp == integer_typep) || (tp->form == ENUM_FORM))
	    tos->integer = *((int *) tos->address);
	else if (base_tp == char_typep)
	    tos->byte = *((char *) tos->address);
	else
	    tos->real = *((float *) tos->address);
    }

    if ((use != TARGET_USE) && (use != VARPARM_USE))
	trace_data_fetch(idp, tp,
			 (tp->form == ARRAY_FORM) ||
			 (tp->form == RECORD_FORM)
			     ? tos->address
			     : tos);

    return(tp);
}

/*--------------------------------------------------------------*/
/*  exec_subscripts     Execute subscripts to modify the array  */
/*                      data area address on the top of stack.  */
/*                      Return a pointer to the type of the     */
/*                      array element.                          */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_subscripts(tp)

    TYPE_STRUCT_PTR tp;         /* ptr to type structure */

{
    int subscript_value;

    /*
    --  Loop to execute bracketed subscripts.
    */
    while (ctoken == LBRACKET) {
	/*
	--  Loop to execute a subscript list.
	*/
	do {
	    get_ctoken();
	    exec_expression();

	    subscript_value = tos->integer;
	    pop();

	    /*
	    --  Range check.
	    */
	    if ((subscript_value < tp->info.array.min_index) ||
		(subscript_value > tp->info.array.max_index))
		runtime_error(VALUE_OUT_OF_RANGE);

	    /*
	    --  Modify the data area address.
	    */
	    tos->address +=
		(subscript_value - tp->info.array.min_index) *
				      tp->info.array.elmt_typep->size;

	    if (ctoken == COMMA) tp = tp->info.array.elmt_typep;
	} while (ctoken == COMMA);

	get_ctoken();
	if (ctoken == LBRACKET) tp = tp->info.array.elmt_typep;
    }

    return(tp->info.array.elmt_typep);
}

/*--------------------------------------------------------------*/
/*  exec_field          Execute a field designator to modify    */
/*                      the record data area address on the     */
/*                      top of stack.  Return a pointer to the  */
/*                      type of the record field.               */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_field()

{
    SYMTAB_NODE_PTR field_idp;

    get_ctoken();
    field_idp = get_symtab_cptr();

    tos->address += field_idp->defn.info.data.offset;

    get_ctoken();
    return(field_idp->typep);
}

/*--------------------------------------------------------------*/
/*  promote_operands_to_real    If either operand is integer,   */
/*                              convert it to real.             */
/*--------------------------------------------------------------*/

promote_operands_to_real(operandp1, tp1, operandp2, tp2)

    STACK_ITEM_PTR  operandp1, operandp2;     /* ptrs to operands */
    TYPE_STRUCT_PTR tp1, tp2;                 /* ptrs to types */

{
    if (tp1 == integer_typep) operandp1->real = operandp1->integer;
    if (tp2 == integer_typep) operandp2->real = operandp2->integer;
}

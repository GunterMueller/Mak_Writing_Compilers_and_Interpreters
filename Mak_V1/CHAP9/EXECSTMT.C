/****************************************************************/
/*                                                              */
/*      S T A T E M E N T   E X E C U T O R                     */
/*                                                              */
/*      Execution routines for statements.                      */
/*                                                              */
/*      FILE:       execstmt.c                                  */
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
extern int            exec_line_number;
extern long           exec_stmt_count;

extern char           *code_segmentp;
extern char           *statement_startp;
extern TOKEN_CODE     ctoken;

extern STACK_ITEM     *stack;
extern STACK_ITEM_PTR tos;
extern STACK_ITEM_PTR stack_frame_basep;

extern TYPE_STRUCT_PTR integer_typep, real_typep,
		       boolean_typep, char_typep;

/*--------------------------------------------------------------*/
/*  exec_statement      Execute a statement by calling the      */
/*                      appropriate execution routine.          */
/*--------------------------------------------------------------*/

exec_statement()

{
   if (ctoken == STATEMENT_MARKER) {
	exec_line_number = get_statement_cmarker();
	++exec_stmt_count;

	statement_startp = code_segmentp;
	trace_statement_execution();
	get_ctoken();
   }

    switch (ctoken) {

	case IDENTIFIER: {
	    SYMTAB_NODE_PTR idp = get_symtab_cptr();

	    if (idp->defn.key == PROC_DEFN)
		exec_routine_call(idp);
	    else
		exec_assignment_statement(idp);

	    break;
	}

	case BEGIN:     exec_compound_statement();      break;
	case END:       break;

	default:  runtime_error(UNIMPLEMENTED_RUNTIME_FEATURE);
    }

    while (ctoken == SEMICOLON) get_ctoken();
}

/*--------------------------------------------------------------*/
/*  exec_assignment_statement       Execute an assignment       */
/*                                  statement.                  */
/*--------------------------------------------------------------*/

exec_assignment_statement(idp)

    SYMTAB_NODE_PTR idp;        /* target variable id */

{
    STACK_ITEM_PTR  targetp;    /* ptr to assignment target */
    TYPE_STRUCT_PTR target_tp, base_target_tp, expr_tp;

    /*
    --  Assignment to function id:  Target is the first item of
    --                              the appropriate stack frame.
    */

    if (idp->defn.key == FUNC_DEFN) {
	STACK_FRAME_HEADER_PTR hp;
	int                    delta;   /* difference in levels */

	hp    = (STACK_FRAME_HEADER_PTR) stack_frame_basep;
	delta = level - idp->level - 1;
	while (delta-- > 0)
	    hp = (STACK_FRAME_HEADER_PTR) hp->static_link.address;

	targetp = (STACK_ITEM_PTR) hp;
	target_tp = idp->typep;
	get_ctoken();
    }

    /*
    --  Assignment to variable:  Routine exec_variable leaves the
    --                           target address on top of stack.
    */
    else {
	target_tp = exec_variable(idp, TARGET_USE);
	targetp   = (STACK_ITEM_PTR) tos->address;

	pop();          /* pop off target address */
    }

    base_target_tp = base_type(target_tp);

    /*
    --  Routine exec_expression leaves the expression value
    --  on top of stack.
    */
    get_ctoken();
    expr_tp = exec_expression();

    /*
    --  Do the assignment.
    */
    if ((target_tp == real_typep) &&
	(base_type(expr_tp) == integer_typep)) {
	/*
	--  real := integer
	*/
	targetp->real = tos->integer;
    }
    else if ((target_tp->form == ARRAY_FORM) ||
	     (target_tp->form == RECORD_FORM)) {
	/*
	--  array  := array
	--  record := record
	*/
	char *ptr1 = (char *) targetp;
	char *ptr2 = tos->address;
	int  size  = target_tp->size;

	while (size--) *ptr1++ = *ptr2++;
    }
    else if ((base_target_tp == integer_typep) ||
	     (target_tp->form == ENUM_FORM)) {
	/*
	--  Range check assignment to integer
	--  or enumeration subrange.
	*/
	if (   (target_tp->form == SUBRANGE_FORM)
	    && ((tos->integer < target_tp->info.subrange.min) ||
		(tos->integer > target_tp->info.subrange.max)))
	    runtime_error(VALUE_OUT_OF_RANGE);
	/*
	--  integer     := integer
	--  enumeration := enumeration
	*/
	targetp->integer = tos->integer;
    }
    else if (base_target_tp == char_typep) {
	/*
	--  Range check assigment to character subrange.
	*/
	if (   (target_tp->form == SUBRANGE_FORM)
	    && ((tos->byte < target_tp->info.subrange.min) ||
		(tos->byte > target_tp->info.subrange.max)))
	    runtime_error(VALUE_OUT_OF_RANGE);
	/*
	--  character := character
	*/
	targetp->byte = tos->byte;
    }
    else {
	/*
	--  real := real
	*/
	targetp->real = tos->real;
    }

    pop();      /* pop off expression value */

    trace_data_store(idp, idp->typep, targetp, target_tp);
}

/*--------------------------------------------------------------*/
/*  exec_routine_call           Execute a procedure or function */
/*                              call.  Return a pointer to the  */
/*                              type structure.                 */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_routine_call(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;    /* routine id */

{
    TYPE_STRUCT_PTR exec_declared_routine_call();
    TYPE_STRUCT_PTR exec_standard_routine_call();

    if (rtn_idp->defn.info.routine.key == DECLARED)
	return(exec_declared_routine_call(rtn_idp));
    else
	return(exec_standard_routine_call(rtn_idp));
}

/*--------------------------------------------------------------*/
/*  exec_declared_routine_call      Execute a call to a         */
/*                                  declared procedure or       */
/*                                  function.  Return a pointer */
/*                                  to the type structure.      */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_declared_routine_call(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;                /* routine id */

{
    int old_level = level;                  /* level of caller */
    int new_level = rtn_idp->level + 1;     /* level of callee */
    STACK_ITEM_PTR new_stack_frame_basep;
    STACK_FRAME_HEADER_PTR hp;              /* ptr to frame header */

    /*
    --  Set up stack frame of callee.
    */
    new_stack_frame_basep = tos + 1;
    push_stack_frame_header(old_level, new_level);

    /*
    --  Push parameter values onto the stack.
    */
    get_ctoken();
    if (ctoken == LPAREN) {
	exec_actual_parms(rtn_idp);
	get_ctoken();   /* token after ) */
    }

    /*
    --  Set the return address in the new stack frame,
    --  and execute the callee.
    */
    level = new_level;
    stack_frame_basep = new_stack_frame_basep;
    hp = (STACK_FRAME_HEADER_PTR) stack_frame_basep;
    hp->return_address.address = code_segmentp - 1;
    execute(rtn_idp);

    /*
    --  Return from callee.
    */
    level = old_level;
    get_ctoken();       /* first token after return */

    return(rtn_idp->defn.key == PROC_DEFN ? NULL : rtn_idp->typep);
}

/*--------------------------------------------------------------*/
/*  exec_standard_routine_call      Execute a call to a         */
/*                                  standard procedure or       */
/*                                  function.                   */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
exec_standard_routine_call(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;    /* routine id */

{
    runtime_error(UNIMPLEMENTED_RUNTIME_FEATURE);
}

/*--------------------------------------------------------------*/
/*  exec_actual_parms           Push the values of the actual   */
/*                              parameters onto the stack.      */
/*--------------------------------------------------------------*/

exec_actual_parms(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;        /* id of callee routine */

{
    SYMTAB_NODE_PTR formal_idp;     /* formal parm id */
    TYPE_STRUCT_PTR formal_tp, actual_tp;

    /*
    --  Loop to execute actual parameters.
    */
    for (formal_idp = rtn_idp->defn.info.routine.parms;
	 formal_idp != NULL;
	 formal_idp = formal_idp->next) {

	formal_tp = formal_idp->typep;
	get_ctoken();

	/*
	--  Value parameter.
	*/
	if (formal_idp->defn.key == VALPARM_DEFN) {
	    actual_tp = exec_expression();

	    /*
	    --  Range check for a subrange formal parameter.
	    */
	    if (formal_tp->form == SUBRANGE_FORM) {
		TYPE_STRUCT_PTR base_formal_tp = base_type(formal_tp);
		int             value;

		value = ((base_formal_tp == integer_typep) ||
			 (base_formal_tp->form == ENUM_FORM))
			    ? tos->integer
			    : tos->byte;

		if ((value < formal_tp->info.subrange.min) ||
		    (value > formal_tp->info.subrange.max)) {
		    runtime_error(VALUE_OUT_OF_RANGE);
		}
	    }

	    /*
	    --  real formal := integer actual
	    */
	    else if ((formal_tp == real_typep) &&
		     (base_type(actual_tp) == integer_typep)) {
		tos->real = tos->integer;
	    }

	    /*
	    --  Formal parm is array or record:  Make a copy.
	    */
	    if ((formal_tp->form == ARRAY_FORM) ||
		(formal_tp->form == RECORD_FORM)) {
		int  size      = formal_tp->size;
		char *ptr1     = alloc_bytes(size);
		char *ptr2     = tos->address;
		char *save_ptr = ptr1;

		while (size--) *ptr1++ = *ptr2++;
		tos->address = save_ptr;
	    }
	}

	/*
	--  VAR parameter.
	*/
	else {
	    SYMTAB_NODE_PTR idp = get_symtab_cptr();

	    exec_variable(idp, VARPARM_USE);
	}
    }
}

/*--------------------------------------------------------------*/
/*  exec_compound_statement     Execute a compound statement.   */
/*--------------------------------------------------------------*/

exec_compound_statement()

{
    get_ctoken();
    while (ctoken != END) exec_statement();
    get_ctoken();
}

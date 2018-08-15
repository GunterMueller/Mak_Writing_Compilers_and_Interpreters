/****************************************************************/
/*                                                              */
/*      S T A T E M E N T   P A R S E R                         */
/*								*/
/*	Parsing routines for statements.			*/
/*								*/
/*      FILE:       stmt.c                                      */
/*								*/
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

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE       token;
extern char             word_string[];
extern LITERAL          literal;
extern TOKEN_CODE       statement_start_list[], statement_end_list[];

extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

extern int              label_index;
extern char             asm_buffer[];
extern char             *asm_bufferp;
extern FILE             *code_file;

/*--------------------------------------------------------------*/
/*  statement		Process a statement by calling the	*/
/*                      appropriate parsing routine based on    */
/*			the statement's first token.		*/
/*--------------------------------------------------------------*/

statement()

{
    /*
    --	Call the appropriate routine based on the first
    --	token of the statement.
    */
    switch (token) {

	case IDENTIFIER: {
	    SYMTAB_NODE_PTR idp;

	    /*
	    --  Assignment statement or procedure call?
	    */
	    search_and_find_all_symtab(idp);

	    if (idp->defn.key == PROC_DEFN) {
		get_token();
		routine_call(idp, TRUE);
	    }
	    else assignment_statement(idp);

	    break;
	}

	case BEGIN:     compound_statement();   break;

	case WHILE:
	case REPEAT:
	case IF:
	case FOR:
	case CASE: {
	    error(UNIMPLEMENTED_FEATURE);
	    exit(-UNIMPLEMENTED_FEATURE);
	}
    }

    /*
    --  Error synchronization:  Only a semicolon, END, ELSE, or
    --                          UNTIL may follow a statement.
    --                          Check for a missing semicolon.
    */
    synchronize(statement_end_list, statement_start_list, NULL);
    if (token_in(statement_start_list)) error(MISSING_SEMICOLON);
}

/*--------------------------------------------------------------*/
/*  assignment_statement    Process an assignment statement:	*/
/*								*/
/*				<id> := <expr>			*/
/*--------------------------------------------------------------*/

assignment_statement(var_idp)

    SYMTAB_NODE_PTR var_idp;            /* target variable id */

{
    TYPE_STRUCT_PTR var_tp, expr_tp;    /* types of var and expr */
    BOOLEAN         stacked_flag;       /* TRUE iff target address
					   was pushed on stack */

    var_tp = variable(var_idp, TARGET_USE);
    stacked_flag = (var_idp->defn.key == VARPARM_DEFN) ||
		   (var_idp->defn.key == FUNC_DEFN) ||
		   (var_idp->typep->form == ARRAY_FORM) ||
		   (var_idp->typep->form == RECORD_FORM) ||
		   ((var_idp->level > 1) && (var_idp->level < level));

    if_token_get_else_error(COLONEQUAL, MISSING_COLONEQUAL);
    expr_tp = expression();

    if (! is_assign_type_compatible(var_tp, expr_tp))
	error(INCOMPATIBLE_ASSIGNMENT);

    var_tp  = base_type(var_tp);
    expr_tp = base_type(expr_tp);

    /*
    --  Emit code to do the assignment.
    */
    if (var_tp == char_typep) {
	/*
	--  char := char
	*/
	if (stacked_flag) {
	    emit_1(POP, reg(BX));
	    emit_2(MOVE, byte_indirect(BX), reg(AL));
	}
	else emit_2(MOVE, byte(var_idp), reg(AL));
    }
    else if (var_tp == real_typep) {
	/*
	--  real := ...
	*/
	if (expr_tp == integer_typep) {
	    /*
	    --  ... integer
	    */
	    emit_1(PUSH, reg(AX));
	    emit_1(CALL, name_lit(FLOAT_CONVERT));
	    emit_2(ADD, reg(SP), integer_lit(2));
	}
	/*
	--  ... real
	*/
	if (stacked_flag) {
	    emit_1(POP, reg(BX));
	    emit_2(MOVE, word_indirect(BX), reg(AX));
	    emit_2(MOVE, high_dword_indirect(BX), reg(DX));
	}
	else {
	    emit_2(MOVE, word(var_idp), reg(AX));
	    emit_2(MOVE, high_dword(var_idp), reg(DX));
	}
    }
    else if ((var_tp->form == ARRAY_FORM) ||
	     (var_tp->form == RECORD_FORM)) {
	/*
	--  array  := array
	--  record := record
	*/
	emit_2(MOVE, reg(CX), integer_lit(var_tp->size));
	emit_1(POP,  reg(SI));
	emit_1(POP,  reg(DI));
	emit_2(MOVE, reg(AX), reg(DS));
	emit_2(MOVE, reg(ES), reg(AX));
	emit(CLEAR_DIRECTION);
	emit(MOVE_BLOCK);
    }
    else {
	/*
	--  integer := integer
	--  enum    := enum
	*/
	if (stacked_flag) {
	    emit_1(POP, reg(BX));
	    emit_2(MOVE, word_indirect(BX), reg(AX));
	}
	else emit_2(MOVE, word(var_idp), reg(AX));
    }
}

/*--------------------------------------------------------------*/
/*  compound_statement	    Process a compound statement:	*/
/*								*/
/*				BEGIN <stmt-list> END		*/
/*--------------------------------------------------------------*/

compound_statement()

{
    /*
    --  <stmt-list>
    */
    get_token();
    do {
	statement();
	while (token == SEMICOLON) get_token();
	if (token == END) break;

	/*
	--  Error synchronization:  Should be at the start of the
	--                          next statement.
	*/
	synchronize(statement_start_list, NULL, NULL);
    } while (token_in(statement_start_list));

    if_token_get_else_error(END, MISSING_END);
}

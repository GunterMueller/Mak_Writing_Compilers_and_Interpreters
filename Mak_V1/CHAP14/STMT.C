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

	case REPEAT:    repeat_statement();     break;
	case WHILE:     while_statement();      break;
	case IF:        if_statement();         break;
	case FOR:       for_statement();        break;
	case CASE:      case_statement();       break;
	case BEGIN:     compound_statement();   break;
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
/*  repeat_statement    Process a REPEAT statement:             */
/*                                                              */
/*                          REPEAT <stmt-list> UNTIL <expr>     */
/*--------------------------------------------------------------*/

repeat_statement()

{
    TYPE_STRUCT_PTR expr_tp;
    int             loop_begin_labelx = new_label_index();
    int             loop_exit_labelx  = new_label_index();

    emit_label(STMT_LABEL_PREFIX, loop_begin_labelx);

    /*
    --  <stmt-list>
    */
    get_token();
    do {
	statement();
	while (token == SEMICOLON) get_token();
    } while (token_in(statement_start_list));

    if_token_get_else_error(UNTIL, MISSING_UNTIL);

    expr_tp = expression();
    if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

    emit_2(COMPARE, reg(AX), integer_lit(1));
    emit_1(JUMP_EQ, label(STMT_LABEL_PREFIX, loop_exit_labelx));
    emit_1(JUMP, label(STMT_LABEL_PREFIX, loop_begin_labelx));
    emit_label(STMT_LABEL_PREFIX, loop_exit_labelx);
}

/*--------------------------------------------------------------*/
/*  while_statement     Process a WHILE statement:              */
/*                                                              */
/*                          WHILE <expr> DO <stmt>              */
/*--------------------------------------------------------------*/

while_statement()

{
    TYPE_STRUCT_PTR expr_tp;
    int             loop_test_labelx = new_label_index();
    int             loop_stmt_labelx = new_label_index();
    int             loop_exit_labelx = new_label_index();

    emit_label(STMT_LABEL_PREFIX, loop_test_labelx);

    get_token();
    expr_tp = expression();
    if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

    emit_2(COMPARE, reg(AX), integer_lit(1));
    emit_1(JUMP_EQ, label(STMT_LABEL_PREFIX, loop_stmt_labelx));
    emit_1(JUMP,    label(STMT_LABEL_PREFIX, loop_exit_labelx));
    emit_label(STMT_LABEL_PREFIX, loop_stmt_labelx);

    if_token_get_else_error(DO, MISSING_DO);
    statement();

    emit_1(JUMP, label(STMT_LABEL_PREFIX, loop_test_labelx));
    emit_label(STMT_LABEL_PREFIX, loop_exit_labelx);
}

/*--------------------------------------------------------------*/
/*  if_statement        Process an IF statement:                */
/*                                                              */
/*                          IF <expr> THEN <stmt>               */
/*                                                              */
/*                      or:                                     */
/*                                                              */
/*                          IF <expr> THEN <stmt> ELSE <stmt>   */
/*--------------------------------------------------------------*/

if_statement()

{
    TYPE_STRUCT_PTR expr_tp;
    int             true_labelx  = new_label_index();
    int             false_labelx = new_label_index();
    int             if_end_labelx;

    get_token();
    expr_tp = expression();
    if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

    emit_2(COMPARE, reg(AX), integer_lit(1));
    emit_1(JUMP_EQ, label(STMT_LABEL_PREFIX, true_labelx));
    emit_1(JUMP,    label(STMT_LABEL_PREFIX, false_labelx));
    emit_label(STMT_LABEL_PREFIX, true_labelx);

    if_token_get_else_error(THEN, MISSING_THEN);
    statement();

    /*
    --  ELSE branch?
    */
    if (token == ELSE) {
	if_end_labelx = new_label_index();
	emit_1(JUMP, label(STMT_LABEL_PREFIX, if_end_labelx));
	emit_label(STMT_LABEL_PREFIX, false_labelx);

	get_token();
	statement();

	emit_label(STMT_LABEL_PREFIX, if_end_labelx);
    }
    else emit_label(STMT_LABEL_PREFIX, false_labelx);
}

/*--------------------------------------------------------------*/
/*  for_statement       Process a FOR statement:                */
/*                                                              */
/*                          FOR <id> := <expr> TO|DOWNTO <expr> */
/*                              DO <stmt>                       */
/*--------------------------------------------------------------*/

for_statement()

{
    SYMTAB_NODE_PTR for_idp;
    TYPE_STRUCT_PTR for_tp, expr_tp;
    BOOLEAN         to_flag;
    int             loop_test_labelx = new_label_index();
    int             loop_stmt_labelx = new_label_index();
    int             loop_exit_labelx = new_label_index();

    get_token();
    if (token == IDENTIFIER) {
	search_and_find_all_symtab(for_idp);
	if ((for_idp->level != level) ||
	    (for_idp->defn.key != VAR_DEFN))
	    error(INVALID_FOR_CONTROL);

	for_tp = base_type(for_idp->typep);
	get_token();

	if ((for_tp != integer_typep) &&
	    (for_tp != char_typep) &&
	    (for_tp->form != ENUM_FORM)) error(INCOMPATIBLE_TYPES);
    }
    else {
	error(IDENTIFIER, MISSING_IDENTIFIER);
	for_tp = &dummy_type;
    }

    if_token_get_else_error(COLONEQUAL, MISSING_COLONEQUAL);

    expr_tp = expression();
    if (! is_assign_type_compatible(for_tp, expr_tp))
	error(INCOMPATIBLE_TYPES);

    if (for_tp == char_typep) emit_2(MOVE, byte(for_idp), reg(AL))
    else                      emit_2(MOVE, word(for_idp), reg(AX))

    if ((token == TO) || (token == DOWNTO)) {
	to_flag = (token == TO);
	get_token();
    }
    else error(MISSING_TO_OR_DOWNTO);

    emit_label(STMT_LABEL_PREFIX, loop_test_labelx);

    expr_tp = expression();
    if (! is_assign_type_compatible(for_tp, expr_tp))
	error(INCOMPATIBLE_TYPES);

    if (for_tp == char_typep) emit_2(COMPARE, byte(for_idp), reg(AL))
    else                      emit_2(COMPARE, word(for_idp), reg(AX))
    emit_1(to_flag ? JUMP_LE : JUMP_GE,
	   label(STMT_LABEL_PREFIX, loop_stmt_labelx));
    emit_1(JUMP, label(STMT_LABEL_PREFIX, loop_exit_labelx));
    emit_label(STMT_LABEL_PREFIX, loop_stmt_labelx);

    if_token_get_else_error(DO, MISSING_DO);
    statement();

    emit_1(to_flag ? INCREMENT : DECREMENT,
	   for_tp == char_typep ? byte(for_idp) : word(for_idp));
    emit_1(JUMP, label(STMT_LABEL_PREFIX, loop_test_labelx));

    emit_label(STMT_LABEL_PREFIX, loop_exit_labelx);
    emit_1(to_flag ?  DECREMENT : INCREMENT,
	   for_tp == char_typep ? byte(for_idp) : word(for_idp));
}

/*--------------------------------------------------------------*/
/*  case_statement      Process a CASE statement:               */
/*                                                              */
/*                          CASE <expr> OF                      */
/*                              <case-branch> ;                 */
/*                              ...                             */
/*                          END                                 */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_expr_list[]      = {OF, SEMICOLON, 0};

TOKEN_CODE case_label_start_list[] = {IDENTIFIER, NUMBER, PLUS,
				      MINUS, STRING, 0};

case_statement()

{
    BOOLEAN         another_branch;
    int             case_end_labelx = new_label_index();
    TYPE_STRUCT_PTR expr_tp;

    get_token();
    expr_tp = expression();

    if (   ((expr_tp->form != SCALAR_FORM) &&
	    (expr_tp->form != ENUM_FORM) &&
	    (expr_tp->form != SUBRANGE_FORM))
	|| (expr_tp == real_typep)) error(INCOMPATIBLE_TYPES);

    /*
    --  Error synchronization:  Should be OF
    */
    synchronize(follow_expr_list, case_label_start_list, NULL);
    if_token_get_else_error(OF, MISSING_OF);

    /*
    --  Loop to process CASE branches.
    */
    another_branch = token_in(case_label_start_list);
    while (another_branch) {
	if (token_in(case_label_start_list))
	    case_branch(expr_tp, case_end_labelx);

	if (token == SEMICOLON) {
	    get_token();
	    another_branch = TRUE;
	}
	else if (token_in(case_label_start_list)) {
	    error(MISSING_SEMICOLON);
	    another_branch = TRUE;
	}
	else another_branch = FALSE;
    }

    if_token_get_else_error(END, MISSING_END);
    emit_label(STMT_LABEL_PREFIX, case_end_labelx);
}

/*--------------------------------------------------------------*/
/*  case_branch             Process a CASE branch:              */
/*                                                              */
/*                              <case-label-list> : <stmt>      */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_case_label_list[] = {COLON, SEMICOLON, 0};

case_branch(expr_tp, case_end_labelx)

    TYPE_STRUCT_PTR expr_tp;            /* type of CASE expression */
    int             case_end_labelx;    /* CASE end label index */

{
    BOOLEAN         another_label;
    int             next_test_labelx;
    int             branch_stmt_labelx = new_label_index();
    TYPE_STRUCT_PTR label_tp;
    TYPE_STRUCT_PTR case_label();

    /*
    --  <case-label-list>
    */
    do {
	next_test_labelx = new_label_index();

	label_tp = case_label();
	if (expr_tp != label_tp) error(INCOMPATIBLE_TYPES);

	emit_1(JUMP_NE, label(STMT_LABEL_PREFIX, next_test_labelx));

	get_token();
	if (token == COMMA) {
	    get_token();
	    emit_1(JUMP, label(STMT_LABEL_PREFIX, branch_stmt_labelx));

	    if (token_in(case_label_start_list)) {
		emit_label(STMT_LABEL_PREFIX, next_test_labelx);
		another_label = TRUE;
	    }
	    else {
		error(MISSING_CONSTANT);
		another_label = FALSE;
	    }
	}
	else another_label = FALSE;
    } while (another_label);

    /*
    --  Error synchronization:  Should be :
    */
    synchronize(follow_case_label_list, statement_start_list, NULL);
    if_token_get_else_error(COLON, MISSING_COLON);

    emit_label(STMT_LABEL_PREFIX, branch_stmt_labelx);
    statement();

    emit_1(JUMP, label(STMT_LABEL_PREFIX, case_end_labelx));
    emit_label(STMT_LABEL_PREFIX, next_test_labelx);
}

/*--------------------------------------------------------------*/
/*  case_label              Process a CASE label and return a   */
/*                          pointer to its type structure.      */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
case_label()

{
    TOKEN_CODE sign     = PLUS;    	/* unary + or - sign */
    BOOLEAN    saw_sign = FALSE;   	/* TRUE iff unary sign */

    /*
    --  Unary + or - sign.
    */
    if ((token == PLUS) || (token == MINUS)) {
	sign     = token;
	saw_sign = TRUE;
	get_token();
    }

    /*
    --  Numeric constant:  Integer type only.
    */
    if (token == NUMBER) {
	if (literal.type == INTEGER_LIT)
	    emit_2(COMPARE, reg(AX),
		   integer_lit(sign == PLUS
				   ?  literal.value.integer
				   : -literal.value.integer))
	else error(INVALID_CONSTANT);

	return(integer_typep);
    }

    /*
    --  Identifier constant:  Integer, character, or enumeration
    --                        types only.
    */
    else if (token == IDENTIFIER) {
	SYMTAB_NODE_PTR idp;

	search_all_symtab(idp);

	if (idp == NULL) {
	    error(UNDEFINED_IDENTIFIER);
	    return(&dummy_type);
	}

	else if (idp->defn.key != CONST_DEFN) {
	    error(NOT_A_CONSTANT_IDENTIFIER);
	    return(&dummy_type);
	}

	else if (idp->typep == integer_typep) {
	    emit_2(COMPARE, reg(AX),
		   integer_lit(sign == PLUS
				   ?  idp->defn.info.constant
					       .value.integer
				   : -idp->defn.info.constant
					       .value.integer));
	    return(integer_typep);
	}

	else if (idp->typep == char_typep) {
	    if (saw_sign) error(INVALID_CONSTANT);
	    emit_2(COMPARE, reg(AL),
			    char_lit(idp->defn.info.constant
					      .value.character));
	    return(char_typep);
	}

	else if (idp->typep->form == ENUM_FORM) {
	    if (saw_sign) error(INVALID_CONSTANT);
	    emit_2(COMPARE, reg(AX),
			    integer_lit(idp->defn.info.constant
						 .value.integer));
	    return(idp->typep);
	}

	else return(&dummy_type);
    }

    /*
    --  String constant:  Character type only.
    */
    else if (token == STRING) {
	if (saw_sign) error(INVALID_CONSTANT);

	if (strlen(literal.value.string) == 1) {
	    emit_2(COMPARE, reg(AL), char_lit(literal.value.string[0]));
	    return(char_typep);
	}
	else {
	    error(INVALID_CONSTANT);
	    return(&dummy_type);
	}
    }

    else {
	error(INVALID_CONSTANT);
	return(&dummy_type);
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

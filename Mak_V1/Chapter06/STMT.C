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

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE       token;
extern char             word_string[];
extern LITERAL          literal;
extern TOKEN_CODE       statement_start_list[], statement_end_list[];

extern SYMTAB_NODE_PTR  symtab_root;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

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
	    --  Assignment statement.
	    */
	    search_and_find_all_symtab(idp);
	    assignment_statement(idp);

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

    var_tp = variable(var_idp, TARGET_USE);
    if_token_get_else_error(COLONEQUAL, MISSING_COLONEQUAL);

    expr_tp = expression();

    if (! is_assign_type_compatible(var_tp, expr_tp))
	error(INCOMPATIBLE_ASSIGNMENT);
}

/*--------------------------------------------------------------*/
/*  repeat_statement    Process a REPEAT statement:             */
/*                                                              */
/*                          REPEAT <stmt-list> UNTIL <expr>     */
/*--------------------------------------------------------------*/

repeat_statement()

{
    TYPE_STRUCT_PTR expr_tp;

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
}

/*--------------------------------------------------------------*/
/*  while_statement     Process a WHILE statement:              */
/*                                                              */
/*                          WHILE <expr> DO <stmt>              */
/*--------------------------------------------------------------*/

while_statement()

{
    TYPE_STRUCT_PTR expr_tp;

    get_token();

    expr_tp = expression();
    if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

    if_token_get_else_error(DO, MISSING_DO);
    statement();
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

    get_token();

    expr_tp = expression();
    if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

    if_token_get_else_error(THEN, MISSING_THEN);
    statement();

    /*
    --  ELSE branch?
    */
    if (token == ELSE) {
	get_token();
	statement();
    }
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

    get_token();

    if (token == IDENTIFIER) {
	search_and_find_all_symtab(for_idp);

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

    if ((token == TO) || (token == DOWNTO)) get_token();
    else error(MISSING_TO_OR_DOWNTO);

    expr_tp = expression();
    if (! is_assign_type_compatible(for_tp, expr_tp))
	error(INCOMPATIBLE_TYPES);

    if_token_get_else_error(DO, MISSING_DO);
    statement();
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
    TYPE_STRUCT_PTR expr_tp;
    TYPE_STRUCT_PTR case_label();

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
	if (token_in(case_label_start_list)) case_branch(expr_tp);

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
}

/*--------------------------------------------------------------*/
/*  case_branch             Process a CASE branch:              */
/*                                                              */
/*                              <case-label-list> : <stmt>      */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_case_label_list[] = {COLON, SEMICOLON, 0};

case_branch(expr_tp)

    TYPE_STRUCT_PTR expr_tp;            /* type of CASE expression */

{
    BOOLEAN         another_label;
    TYPE_STRUCT_PTR label_tp;
    TYPE_STRUCT_PTR case_label();

    /*
    --  <case-label-list>
    */
    do {
	label_tp = case_label();
	if (expr_tp != label_tp) error(INCOMPATIBLE_TYPES);

	get_token();
	if (token == COMMA) {
	    get_token();
	    if (token_in(case_label_start_list)) another_label = TRUE;
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

    statement();
}

/*--------------------------------------------------------------*/
/*  case_label              Process a CASE label and return a   */
/*                          pointer to its type structure.      */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
case_label()

{
    TOKEN_CODE      sign     = PLUS;    /* unary + or - sign */
    BOOLEAN         saw_sign = FALSE;   /* TRUE iff unary sign */
    TYPE_STRUCT_PTR label_tp;

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
	if (literal.type == REAL_LIT) error(INVALID_CONSTANT);
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

	else if (idp->typep == integer_typep)
	    return(integer_typep);

	else if (idp->typep == char_typep) {
	    if (saw_sign) error(INVALID_CONSTANT);
	    return(char_typep);
	}

	else if (idp->typep->form == ENUM_FORM) {
	    if (saw_sign) error(INVALID_CONSTANT);
	    return(idp->typep);
	}

	else return(&dummy_type);
    }

    /*
    --  String constant:  Character type only.
    */
    else if (token == STRING) {
	if (saw_sign) error(INVALID_CONSTANT);

	if (strlen(literal.value.string) == 1) return(char_typep);
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

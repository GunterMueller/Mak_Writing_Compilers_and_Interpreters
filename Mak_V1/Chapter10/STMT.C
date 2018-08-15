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
#include "exec.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE       token;
extern char             token_string[];
extern char             word_string[];
extern LITERAL          literal;
extern TOKEN_CODE       statement_start_list[], statement_end_list[];

extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;
extern char             *code_bufferp;

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
    if (token != BEGIN) crunch_statement_marker();

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
		crunch_symtab_node_ptr(idp);
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
    synchronize(statement_end_list, NULL, NULL);
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
    char            *loop_end_location;

    get_token();
    loop_end_location = crunch_address_marker(NULL);

    expr_tp = expression();
    if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

    if_token_get_else_error(DO, MISSING_DO);
    statement();

    fixup_address_marker(loop_end_location);
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
    char            *false_location;
    char            *if_end_location;

    get_token();
    false_location = crunch_address_marker(NULL);

    expr_tp = expression();
    if (expr_tp != boolean_typep) error(INCOMPATIBLE_TYPES);

    if_token_get_else_error(THEN, MISSING_THEN);
    statement();

    fixup_address_marker(false_location);

    /*
    --  ELSE branch?
    */
    if (token == ELSE) {
	get_token();
	if_end_location = crunch_address_marker(NULL);

	statement();

	fixup_address_marker(if_end_location);
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
    char            *loop_end_location;

    get_token();
    loop_end_location = crunch_address_marker(NULL);

    if (token == IDENTIFIER) {
	search_and_find_all_symtab(for_idp);
	crunch_symtab_node_ptr(for_idp);

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

    if ((token == TO) || (token == DOWNTO)) get_token();
    else error(MISSING_TO_OR_DOWNTO);

    expr_tp = expression();
    if (! is_assign_type_compatible(for_tp, expr_tp))
	error(INCOMPATIBLE_TYPES);

    if_token_get_else_error(DO, MISSING_DO);
    statement();

    fixup_address_marker(loop_end_location);
}

/*--------------------------------------------------------------*/
/*  CASE statement globals                                      */
/*--------------------------------------------------------------*/

typedef struct case_item {
    int              label_value;
    char             *branch_location;
    struct case_item *next;
} CASE_ITEM, *CASE_ITEM_PTR;

CASE_ITEM_PTR case_item_head, case_item_tail;
int           case_label_count;

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
    CASE_ITEM_PTR   case_itemp, next_case_itemp;
    char            *branch_table_location;
    char            *case_end_chain = NULL;

    /*
    --  Initializations for the branch table.
    */
    get_token();
    branch_table_location = crunch_address_marker(NULL);
    case_item_head = case_item_tail = NULL;
    case_label_count = 0;

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

	/*
	--  Link another address marker at the end of
	--  the CASE branch to point to the end of
	--  the CASE statement.
	*/
	case_end_chain = crunch_address_marker(case_end_chain);

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

    /*
    --  Emit the branch table.
    */
    fixup_address_marker(branch_table_location);
    crunch_integer(case_label_count);
    case_itemp = case_item_head;
    while (case_itemp != NULL) {
	crunch_integer(case_itemp->label_value);
	crunch_offset(case_itemp->branch_location);
	next_case_itemp = case_itemp->next;
	free(case_itemp);
	case_itemp = next_case_itemp;
    }

    if_token_get_else_error(END, MISSING_END);

    /*
    --  Patch the CASE branch address markers.
    */
    while (case_end_chain != NULL)
	case_end_chain = fixup_address_marker(case_end_chain);
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
    CASE_ITEM_PTR   case_itemp;
    CASE_ITEM_PTR   old_case_item_tail = case_item_tail;
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

    /*
    --  Loop to fill in the branch_location field of
    --  each CASE_ITEM item for this branch.
    */
    case_itemp = old_case_item_tail == NULL
		     ? case_item_head
		     : old_case_item_tail->next;
    while (case_itemp != NULL) {
	case_itemp->branch_location = code_bufferp;
	case_itemp = case_itemp->next;
    }

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
    CASE_ITEM_PTR   case_itemp = alloc_struct(CASE_ITEM);

    /*
    --  Link in a CASE_ITEM item for this label.
    */
    if (case_item_head != NULL) {
	case_item_tail->next = case_itemp;
	case_item_tail = case_itemp;
    }
    else {
	case_item_head = case_item_tail = case_itemp;
    }
    case_itemp->next = NULL;
    ++case_label_count;

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
	SYMTAB_NODE_PTR np = search_symtab(token_string,
					   symtab_display[1]);

	if (np == NULL) np = enter_symtab(token_string,
					  symtab_display[1]);
	crunch_symtab_node_ptr(np);

	if (literal.type == INTEGER_LIT)
	    case_itemp->label_value = sign == PLUS
					  ?  literal.value.integer
					  : -literal.value.integer;
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
	crunch_symtab_node_ptr(idp);

	if (idp == NULL) {
	    error(UNDEFINED_IDENTIFIER);
	    return(&dummy_type);
	}

	else if (idp->defn.key != CONST_DEFN) {
	    error(NOT_A_CONSTANT_IDENTIFIER);
	    return(&dummy_type);
	}

	else if (idp->typep == integer_typep) {
	    case_itemp->label_value = sign == PLUS
					  ?  idp->defn.info.constant
						      .value.integer
					  : -idp->defn.info.constant
						      .value.integer;
	    return(integer_typep);
	}

	else if (idp->typep == char_typep) {
	    if (saw_sign) error(INVALID_CONSTANT);
	    case_itemp->label_value = idp->defn.info.constant
					       .value.character;
	    return(char_typep);
	}

	else if (idp->typep->form == ENUM_FORM) {
	    if (saw_sign) error(INVALID_CONSTANT);
	    case_itemp->label_value = idp->defn.info.constant
					       .value.integer;
	    return(idp->typep);
	}

	else return(&dummy_type);
    }

    /*
    --  String constant:  Character type only.
    */
    else if (token == STRING) {
	SYMTAB_NODE_PTR np = search_symtab(token_string,
					   symtab_display[1]);

	if (np == NULL) np = enter_symtab(token_string,
					  symtab_display[1]);
	crunch_symtab_node_ptr(np);

	if (saw_sign) error(INVALID_CONSTANT);

	if (strlen(literal.value.string) == 1) {
	    case_itemp->label_value = literal.value.string[0];
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

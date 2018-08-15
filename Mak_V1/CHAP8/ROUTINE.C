/****************************************************************/
/*                                                              */
/*      R O U T I N E   P A R S E R                             */
/*                                                              */
/*      Parsing routines for programs and declared              */
/*      procedures and functions.                               */
/*                                                              */
/*      FILE:       routine.c                                   */
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

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE       token;
extern char             word_string[];
extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT      dummy_type;

extern char             *code_buffer;
extern char             *code_bufferp;

extern TOKEN_CODE       statement_start_list[],
			statement_end_list[],
			declaration_start_list[];

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

char buffer[MAX_PRINT_LINE_LENGTH];

/*--------------------------------------------------------------*/
/*  Forwards                                                    */
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR formal_parm_list();
SYMTAB_NODE_PTR program_header(), procedure_header(),
		function_header();
char            *create_code_segment();

/*--------------------------------------------------------------*/
/*  program       Process a program:                            */
/*                                                              */
/*                          <program-header> ; <block> .        */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_header_list[] = {SEMICOLON, END_OF_FILE, 0};

program()

{
    SYMTAB_NODE_PTR program_idp;        /* program id */

    /*
    --                  PARSE THE PROGRAM
    --
    --
    --  Intialize the symbol table and then allocate
    --  the code buffer.
    */
    init_symtab();
    code_buffer  = alloc_bytes(MAX_CODE_BUFFER_SIZE);
    code_bufferp = code_buffer;

    /*
    --  Begin parsing with the program header.
    */
    program_idp = program_header();

    /*
    --  Error synchronization:  Should be ;
    */
    synchronize(follow_header_list,
		declaration_start_list, statement_start_list);
    if_token_get(SEMICOLON);
    else if (token_in(declaration_start_list) ||
	     token_in(statement_start_list))
	error(MISSING_SEMICOLON);

    analyze_routine_header(program_idp);

    /*
    --  Parse the program's block.
    */
    program_idp->defn.info.routine.locals = NULL;
    block(program_idp);

    program_idp->defn.info.routine.local_symtab = exit_scope();
    program_idp->defn.info.routine.code_segment = create_code_segment();
    analyze_block(program_idp->defn.info.routine.code_segment);

    if_token_get_else_error(PERIOD, MISSING_PERIOD);

    /*
    --  Look for the end of file.
    */
    while (token != END_OF_FILE) {
	error(UNEXPECTED_TOKEN);
	get_token();
    }

    quit_scanner();
    free(code_buffer);
    exit(0);
}

/*--------------------------------------------------------------*/
/*  program_header      Process a program header:               */
/*                                                              */
/*                          PROGRAM <id> ( <id-list> )          */
/*                                                              */
/*                      Return a pointer to the program id      */
/*                      node.                                   */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_prog_id_list[] = {LPAREN, SEMICOLON,
				    END_OF_FILE, 0};

TOKEN_CODE follow_parms_list[]   = {RPAREN, SEMICOLON,
				    END_OF_FILE, 0};

    SYMTAB_NODE_PTR
program_header()

{
    SYMTAB_NODE_PTR program_idp;        /* program id */
    SYMTAB_NODE_PTR parm_idp;           /* parm id */
    SYMTAB_NODE_PTR prev_parm_idp = NULL;

    if_token_get_else_error(PROGRAM, MISSING_PROGRAM);

    if (token == IDENTIFIER) {
	search_and_enter_local_symtab(program_idp);
	program_idp->defn.key = PROG_DEFN;
	program_idp->defn.info.routine.key = DECLARED;
	program_idp->defn.info.routine.parm_count = 0;
	program_idp->defn.info.routine.total_parm_size = 0;
	program_idp->defn.info.routine.total_local_size = 0;
	program_idp->typep = &dummy_type;
	program_idp->label_index = 0;
	get_token();
    }
    else error(MISSING_IDENTIFIER);

    /*
    -- Error synchronization:  Should be ( or ;
    */
    synchronize(follow_prog_id_list,
		declaration_start_list, statement_start_list);

    enter_scope(NULL);

    /*
    --  Program parameters.
    */
    if (token == LPAREN) {
	/*
	--  <id-list>
	*/
	do {
	    get_token();
	    if (token == IDENTIFIER) {
		search_and_enter_local_symtab(parm_idp);
		parm_idp->defn.key = VARPARM_DEFN;
		parm_idp->typep = &dummy_type;
		get_token();

		/*
		--  Link program parm ids together.
		*/
		if (prev_parm_idp == NULL)
		    program_idp->defn.info.routine.parms =
				     prev_parm_idp = parm_idp;
		else {
		    prev_parm_idp->next = parm_idp;
		    prev_parm_idp = parm_idp;
		}
	    }
	    else error(MISSING_IDENTIFIER);
	} while (token == COMMA);

	/*
	--  Error synchronization:  Should be )
	*/
	synchronize(follow_parms_list,
		    declaration_start_list, statement_start_list);
	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }
    else program_idp->defn.info.routine.parms = NULL;

    return(program_idp);
}

/*--------------------------------------------------------------*/
/*  routine             Call the appropriate routine to process */
/*                      a procedure or function definition:     */
/*                                                              */
/*                          <routine-header> ; <block>          */
/*--------------------------------------------------------------*/

routine()

{
    SYMTAB_NODE_PTR rtn_idp;    /* routine id */

    rtn_idp = (token == PROCEDURE) ? procedure_header()
				   : function_header();

    /*
    --  Error synchronization:  Should be ;
    */
    synchronize(follow_header_list,
		declaration_start_list, statement_start_list);
    if_token_get(SEMICOLON);
    else if (token_in(declaration_start_list) ||
	     token_in(statement_start_list))
	error(MISSING_SEMICOLON);

    /*
    --  <block> or FORWARD.
    */
    if (strcmp(word_string, "forward") != 0) {
	rtn_idp->defn.info.routine.key = DECLARED;
	analyze_routine_header(rtn_idp);

	rtn_idp->defn.info.routine.locals = NULL;
	block(rtn_idp);

	rtn_idp->defn.info.routine.code_segment = create_code_segment();
	analyze_block(rtn_idp->defn.info.routine.code_segment);
    }
    else {
	get_token();
	rtn_idp->defn.info.routine.key = FORWARD;
	analyze_routine_header(rtn_idp);
    }

    rtn_idp->defn.info.routine.local_symtab = exit_scope();
}

/*--------------------------------------------------------------*/
/*  procedure_header    Process a procedure header:             */
/*                                                              */
/*                          PROCEDURE <id>                      */
/*                                                              */
/*                      or:                                     */
/*                                                              */
/*                          PROCEDURE <id> ( <parm-list> )      */
/*                                                              */
/*                      Return a pointer to the procedure id    */
/*                      node.                                   */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_proc_id_list[] = {LPAREN, SEMICOLON,
				    END_OF_FILE, 0};

    SYMTAB_NODE_PTR
procedure_header()

{
    SYMTAB_NODE_PTR proc_idp;           /* procedure id */
    SYMTAB_NODE_PTR parm_listp;         /* formal parm list */
    int             parm_count;
    int             total_parm_size;
    BOOLEAN         forward_flag = FALSE;   /* TRUE iff forwarded */

    get_token();

    /*
    --  If the procedure identifier has already been
    --  declared in this scope, it must be a forward.
    */
    if (token == IDENTIFIER) {
	search_local_symtab(proc_idp);
	if (proc_idp == NULL) {
	    enter_local_symtab(proc_idp);
	    proc_idp->defn.key = PROC_DEFN;
	    proc_idp->defn.info.routine.total_local_size = 0;
	    proc_idp->typep = &dummy_type;
	    proc_idp->label_index = 0;
	}
	else if ((proc_idp->defn.key == PROC_DEFN) &&
		 (proc_idp->defn.info.routine.key == FORWARD))
	    forward_flag = TRUE;
	else error(REDEFINED_IDENTIFIER);

	get_token();
    }
    else error(MISSING_IDENTIFIER);

    /*
    --  Error synchronization:  Should be ( or ;
    */
    synchronize(follow_proc_id_list,
		declaration_start_list, statement_start_list);

    enter_scope(NULL);

    /*
    --  Optional formal parameters.  If there was a forward,
    --  there must not be any parameters here (but parse them
    --  anyway for error recovery).
    */
    if (token == LPAREN) {
	parm_listp = formal_parm_list(&parm_count, &total_parm_size);

	if (forward_flag) error(ALREADY_FORWARDED);
	else {
	    proc_idp->defn.info.routine.parm_count = parm_count;
	    proc_idp->defn.info.routine.total_parm_size =
						total_parm_size;
	    proc_idp->defn.info.routine.parms = parm_listp;
	}
    }
    else if (!forward_flag) {
	proc_idp->defn.info.routine.parm_count = 0;
	proc_idp->defn.info.routine.total_parm_size = 0;
	proc_idp->defn.info.routine.parms = NULL;
    }

    proc_idp->typep = NULL;
    return(proc_idp);
}

/*--------------------------------------------------------------*/
/*  function_header    Process a function header:               */
/*                                                              */
/*                          FUNCTION <id> : <type-id>           */
/*                                                              */
/*                      or:                                     */
/*                                                              */
/*                          FUNCTION <id> ( <parm-list> )       */
/*                                        : <type-id>           */
/*                                                              */
/*                      Return a pointer to the function id     */
/*                      node.                                   */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_func_id_list[] = {LPAREN, COLON, SEMICOLON,
				    END_OF_FILE, 0};

    SYMTAB_NODE_PTR
function_header()

{
    SYMTAB_NODE_PTR func_idp, type_idp;     /* func and type ids */
    SYMTAB_NODE_PTR parm_listp;             /* formal parm list */
    int             parm_count;
    int             total_parm_size;
    BOOLEAN         forward_flag = FALSE;   /* TRUE iff forwarded */

    get_token();

    /*
    --  If the function identifier has already been
    --  declared in this scope, it must be a forward.
    */
    if (token == IDENTIFIER) {
	search_local_symtab(func_idp);
	if (func_idp == NULL) {
	    enter_local_symtab(func_idp);
	    func_idp->defn.key = FUNC_DEFN;
	    func_idp->defn.info.routine.total_local_size = 0;
	    func_idp->typep = &dummy_type;
	    func_idp->label_index = 0;
	}
	else if ((func_idp->defn.key == FUNC_DEFN) &&
		 (func_idp->defn.info.routine.key == FORWARD))
	    forward_flag = TRUE;
	else error(REDEFINED_IDENTIFIER);

	get_token();
    }
    else error(MISSING_IDENTIFIER);

    /*
    --  Error synchronization:  Should be ( or : or ;
    */
    synchronize(follow_func_id_list,
		declaration_start_list, statement_start_list);

    enter_scope(NULL);

    /*
    --  Optional formal parameters.  If there was a forward,
    --  there must not be any parameters here (but parse them
    --  anyway for error recovery).
    */
    if (token == LPAREN) {
	parm_listp = formal_parm_list(&parm_count, &total_parm_size);

	if (forward_flag) error(ALREADY_FORWARDED);
	else {
	    func_idp->defn.info.routine.parm_count = parm_count;
	    func_idp->defn.info.routine.total_parm_size =
						total_parm_size;
	    func_idp->defn.info.routine.parms = parm_listp;
	}
    }
    else if (!forward_flag) {
	func_idp->defn.info.routine.parm_count = 0;
	func_idp->defn.info.routine.total_parm_size = 0;
	func_idp->defn.info.routine.parms = NULL;
    }

    /*
    --  Function type.  If there was a forward,
    --  there must not be a type here (but parse it
    --  anyway for error recovery).
    */
    if (!forward_flag || (token == COLON)) {
	if_token_get_else_error(COLON, MISSING_COLON);

	if (token == IDENTIFIER) {
	    search_and_find_all_symtab(type_idp);
	    if (type_idp->defn.key != TYPE_DEFN) error(INVALID_TYPE);
	    if (!forward_flag) func_idp->typep = type_idp->typep;
	    get_token();
	}
	else {
	    error(MISSING_IDENTIFIER);
	    func_idp->typep = &dummy_type;
	}

	if (forward_flag) error(ALREADY_FORWARDED);
    }

    return(func_idp);
}

/*--------------------------------------------------------------*/
/*  formal_parm_list    Process a formal parameter list:        */
/*                                                              */
/*                          ( VAR <id-list> : <type> ;          */
/*                            <id-list> : <type> ;              */
/*                            ... )                             */
/*                                                              */
/*                      Return a pointer to the head of the     */
/*                      parameter id list.                      */
/*--------------------------------------------------------------*/

    SYMTAB_NODE_PTR
formal_parm_list(countp, total_sizep)

    int *countp;        /* ptr to count of parameters */
    int *total_sizep;   /* ptr to total byte size of parameters */

{
    SYMTAB_NODE_PTR parm_idp, first_idp, last_idp;    /* parm ids */
    SYMTAB_NODE_PTR prev_last_idp = NULL;       /* last id of list */
    SYMTAB_NODE_PTR parm_listp = NULL;          /* parm list */
    SYMTAB_NODE_PTR type_idp;                   /* type id */
    TYPE_STRUCT_PTR parm_tp;                    /* parm type */
    DEFN_KEY        parm_defn;                  /* parm definition */
    int             parm_count = 0;             /* count of parms */
    int             parm_offset = 0;

    get_token();

    /*
    --  Loop to process parameter declarations separated by ;
    */
    while ((token == IDENTIFIER) || (token == VAR)) {
	first_idp = NULL;

	/*
	--  VAR parms?
	*/
	if (token == VAR) {
	    parm_defn = VARPARM_DEFN;
	    get_token();
	}
	else parm_defn = VALPARM_DEFN;

	/*
	--  <id list>
	*/
	while (token == IDENTIFIER) {
	    search_and_enter_local_symtab(parm_idp);
	    parm_idp->defn.key    = parm_defn;
	    parm_idp->label_index = 0;
	    ++parm_count;

	    if (parm_listp == NULL) parm_listp = parm_idp;

	    /*
	    --  Link parm ids together.
	    */
	    if (first_idp == NULL)
		first_idp = last_idp = parm_idp;
	    else {
		last_idp->next = parm_idp;
		last_idp = parm_idp;
	    }

	    get_token();
	    if_token_get(COMMA);
	}

	if_token_get_else_error(COLON, MISSING_COLON);

	if (token == IDENTIFIER) {
	    search_and_find_all_symtab(type_idp);
	    if (type_idp->defn.key != TYPE_DEFN) error(INVALID_TYPE);
	    parm_tp = type_idp->typep;
	    get_token();
	}
	else {
	    error(MISSING_IDENTIFIER);
	    parm_tp = &dummy_type;
	}

	/*
	--  Assign the offset and the type to all parm ids
	--  in the sublist.
	*/
	for (parm_idp = first_idp;
	     parm_idp != NULL;
	     parm_idp = parm_idp->next) {
	    parm_idp->typep = parm_tp;
	    parm_idp->defn.info.data.offset = parm_offset++;
	}

	/*
	--  Link this list to the list of all parm ids.
	*/
	if (prev_last_idp != NULL) prev_last_idp->next = first_idp;
	prev_last_idp = last_idp;

	/*
	--  Error synchronization:  Should be ; or )
	*/
	synchronize(follow_parms_list, NULL, NULL);
	if_token_get(SEMICOLON);
    }

    if_token_get_else_error(RPAREN, MISSING_RPAREN);
    *countp = parm_count;
    *total_sizep = parm_offset;

    return(parm_listp);
}

/*--------------------------------------------------------------*/
/*  routine_call            Process a call to a declared or     */
/*                          a standard procedure or function.   */
/*                          Return a pointer to the type        */
/*                          structure of the call.              */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
routine_call(rtn_idp, parm_check_flag)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */
    BOOLEAN         parm_check_flag;    /* if TRUE check parms */

{
    TYPE_STRUCT_PTR declared_routine_call(), standard_routine_call();

    if ((rtn_idp->defn.info.routine.key == DECLARED) ||
	(rtn_idp->defn.info.routine.key == FORWARD) ||
	!parm_check_flag)
	return(declared_routine_call(rtn_idp, parm_check_flag));
    else
	return(standard_routine_call(rtn_idp));
}

/*--------------------------------------------------------------*/
/*  declared_routine_call   Process a call to a declared        */
/*                          procedure or function:              */
/*                                                              */
/*                              <id>                            */
/*                                                              */
/*                          or:                                 */
/*                                                              */
/*                              <id> ( <parm-list> )            */
/*                                                              */
/*                          The actual parameters are checked   */
/*                          against the formal parameters for   */
/*                          type and number.  Return a pointer  */
/*                          to the type structure of the call.  */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
declared_routine_call(rtn_idp, parm_check_flag)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */
    BOOLEAN         parm_check_flag;    /* if TRUE check parms */

{
    actual_parm_list(rtn_idp, parm_check_flag);
    return(rtn_idp->defn.key == PROC_DEFN ? NULL : rtn_idp->typep);
}

/*--------------------------------------------------------------*/
/*  actual_parm_list    Process an actual parameter list:       */
/*                                                              */
/*                          ( <expr-list> )                     */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_parm_list[] = {COMMA, RPAREN, 0};

actual_parm_list(rtn_idp, parm_check_flag)

    SYMTAB_NODE_PTR rtn_idp;            /* routine id */
    BOOLEAN         parm_check_flag;    /* if TRUE check parms */

{
    SYMTAB_NODE_PTR formal_parm_idp;
    DEFN_KEY        formal_parm_defn;
    TYPE_STRUCT_PTR formal_parm_tp, actual_parm_tp;

    if (parm_check_flag)
	formal_parm_idp = rtn_idp->defn.info.routine.parms;

    if (token == LPAREN) {
	/*
	--  Loop to process actual parameter expressions.
	*/
	do {
	    /*
	    --  Obtain info about the corresponding formal parm.
	    */
	    if (parm_check_flag && (formal_parm_idp != NULL)) {
		formal_parm_defn = formal_parm_idp->defn.key;
		formal_parm_tp   = formal_parm_idp->typep;
	    }

	    get_token();

	    /*
	    --  Formal value parm:  Actual parm's type must be
	    --                      assignment compatible with
	    --                      formal parm's type.  Actual
	    --                      parm can be an expression.
	    */
	    if ((formal_parm_idp == NULL) ||
		(formal_parm_defn == VALPARM_DEFN) ||
		!parm_check_flag) {
		actual_parm_tp = expression();
		if (parm_check_flag && (formal_parm_idp != NULL) &&
		    (! is_assign_type_compatible(formal_parm_tp,
						 actual_parm_tp)))
		    error(INCOMPATIBLE_TYPES);
	    }

	    /*
	    --  Formal VAR parm:  Actual parm's type must be the same
	    --                    as formal parm type.  Actual parm
	    --                    must be a variable.
	    */
	    else  /* formal_parm_defn == VARPARM_DEFN */  {
		if (token == IDENTIFIER) {
		    SYMTAB_NODE_PTR idp;

		    search_and_find_all_symtab(idp);
		    actual_parm_tp = variable(idp, VARPARM_USE);

		    if (formal_parm_tp != actual_parm_tp)
			error(INCOMPATIBLE_TYPES);
		}
		else {
		    /*
		    --  Not a variable:  Parse an expression anyway
		    --                   for error recovery.
		    */
		    actual_parm_tp = expression();
		    error(INVALID_VAR_PARM);
		}
	    }

	    /*
	    --  Check if there are more actual parms
	    --  than formal parms.
	    */
	    if (parm_check_flag) {
		if (formal_parm_idp == NULL)
		    error(WRONG_NUMBER_OF_PARMS);
		else formal_parm_idp = formal_parm_idp->next;
	    }

	    /*
	    --  Error synchronization:  Should be , or )
	    */
	    synchronize(follow_parm_list, statement_end_list, NULL);

	} while (token == COMMA);

	if_token_get_else_error(RPAREN, MISSING_RPAREN);
    }

    /*
    --  Check if there are fewer actual parms than formal parms.
    */
    if (parm_check_flag && (formal_parm_idp != NULL))
	error(WRONG_NUMBER_OF_PARMS);
}

/*--------------------------------------------------------------*/
/*  block               Process a block, which consists of      */
/*                      declarations followed by a compound     */
/*                      statement.                              */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_decls_list[] = {SEMICOLON, BEGIN, END_OF_FILE, 0};

block(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;    /* id of program or routine */

{
    extern BOOLEAN block_flag;

    declarations(rtn_idp);

    /*
    --  Error synchronization:  Should be ;
    */
    synchronize(follow_decls_list, NULL, NULL);
    if (token != BEGIN) error(MISSING_BEGIN);

    crunch_token();

    block_flag = TRUE;
    compound_statement();
    block_flag = FALSE;
}

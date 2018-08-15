/****************************************************************/
/*                                                              */
/*      I N T E R A C T I V E   D E B U G G E R                 */
/*                                                              */
/*      Interactive debugging routines.                         */
/*                                                              */
/*      FILE:       debug.c                                     */
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
#include "scanner.h"
#include "symtab.h"
#include "exec.h"

#define MAX_BREAKS      16
#define MAX_WATCHES     16
#define COMMAND_QUERY   "Command? "

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

extern int              level;
extern SYMTAB_NODE_PTR  symtab_display[];
extern STACK_ITEM_PTR   tos;

extern int      line_number;
extern int      buffer_offset;
extern BOOLEAN  print_flag;

extern char     *code_segmentp;
extern char     *statement_startp;
extern int      ctoken;
extern int      exec_line_number;
extern int      error_count;

extern char     *bufferp;
extern char     ch;
extern char     source_buffer[];
extern char     word_string[];
extern int      token;
extern LITERAL  literal;
extern BOOLEAN  block_flag;

extern char     *code_buffer;
extern char     *code_bufferp;
extern char     *code_segmentp;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

FILE    *console;

BOOLEAN debugger_command_flag,  /* TRUE during debug command */
	halt_flag,              /* TRUE to pause for debug command */
	trace_flag,             /* TRUE to trace statement */
	step_flag,              /* TRUE to single-step */
	entry_flag,             /* TRUE to trace routine entry */
	exit_flag;              /* TRUE to trace routine exit */

int     break_count;                    /* count of breakpoints */
int     break_list[MAX_BREAKS];         /* list of breakpoints */

int             watch_count;                /* count of watches */
SYMTAB_NODE_PTR watch_list[MAX_WATCHES];    /* list of watches */

typedef struct {                        /* watch structure */
    SYMTAB_NODE_PTR watch_idp;          /* id node watched variable */
    BOOLEAN         store_flag;         /* TRUE to trace stores */
    BOOLEAN         fetch_flag;         /* TRUE to trace fetches */
} WATCH_STRUCT, *WATCH_STRUCT_PTR;

char *symbol_strings[] = {
    "<no token>", "<IDENTIFIER>", "<NUMBER>", "<STRING>",
    "^", "*", "(", ")", "-", "+", "=", "[", "]", ":", ";",
    "<", ">", ",", ".", "/", ":=", "<=", ">=", "<>", "..",
    "<END OF FILE>", "<ERROR>",
    "AND", "ARRAY", "BEGIN", "CASE", "CONST", "DIV", "DO", "DOWNTO",
    "ELSE", "END", "FILE", "FOR", "FUNCTION", "GOTO", "IF", "IN",
    "LABEL", "MOD", "NIL", "NOT", "OF", "OR", "PACKED", "PROCEDURE",
    "PROGRAM", "RECORD", "REPEAT", "SET", "THEN", "TO", "TYPE",
    "UNTIL", "VAR", "WHILE", "WITH",
};

/*--------------------------------------------------------------*/
/*  init_debugger       Initialize the interactive debugger.    */
/*--------------------------------------------------------------*/

init_debugger()

{
    int i;

    /*
    --  Initialize the debugger's globals.
    */
    console = fopen("CON", "r");
    code_buffer = alloc_bytes(MAX_SOURCE_LINE_LENGTH + 1);

    print_flag = FALSE;
    halt_flag  = block_flag = TRUE;
    debugger_command_flag = trace_flag = step_flag
			  = entry_flag = exit_flag
			  = FALSE;

    break_count = 0;
    for (i = 0; i < MAX_BREAKS; ++i) break_list[i] = 0;

    watch_count = 0;
    for (i = 0; i < MAX_WATCHES; ++i) watch_list[i] = NULL;
}

/*--------------------------------------------------------------*/
/*  read_debugger_command       Read and process a debugging    */
/*                              command typed in by the user.   */
/*--------------------------------------------------------------*/

read_debugger_command()

{
    BOOLEAN done = FALSE;

    debugger_command_flag = TRUE;

    do {
	printf("\n%s", COMMAND_QUERY);

	/*
	--  Read in a debugging command and replace the
	--  final \n\0 with ;;\0
	*/
	bufferp = fgets(source_buffer, MAX_SOURCE_LINE_LENGTH,
			console);
	strcpy(&source_buffer[strlen(source_buffer) - 1], ";;");

	ch = *bufferp++;
	buffer_offset = sizeof(COMMAND_QUERY);
	code_bufferp  = code_buffer;
	error_count   = 0;

	get_token();

	/*
	--  Process the command.
	*/
	switch (token) {
	    case SEMICOLON:     done = TRUE;                break;
	    case IDENTIFIER:    execute_debugger_command(); break;
	}

	if (token != SEMICOLON) error(UNEXPECTED_TOKEN);
    } while (!done);

    debugger_command_flag = FALSE;
}

/*--------------------------------------------------------------*/
/*  execute_debugger_command       Execute a debugger command.  */
/*--------------------------------------------------------------*/

execute_debugger_command()

{
    WATCH_STRUCT_PTR wp;
    WATCH_STRUCT_PTR allocate_watch();

    if (strcmp(word_string, "trace") == 0) {
	trace_flag = TRUE;
	step_flag  = FALSE;
	get_token();
    }
    else if (strcmp(word_string, "untrace") == 0) {
	trace_flag = FALSE;
	get_token();
    }

    else if (strcmp(word_string, "step") == 0) {
	step_flag = TRUE;
	trace_flag = FALSE;
	get_token();
    }
    else if (strcmp(word_string, "unstep") == 0) {
	step_flag = FALSE;
	get_token();
    }

    else if (strcmp(word_string, "break") == 0)
	set_breakpoint();
    else if (strcmp(word_string, "unbreak") == 0)
	remove_breakpoint();

    else if (strcmp(word_string, "entry") == 0) {
	entry_flag = TRUE;
	get_token();
    }
    else if (strcmp(word_string, "unentry") == 0) {
	entry_flag = FALSE;
	get_token();
    }

    else if (strcmp(word_string, "exit") == 0) {
	exit_flag = TRUE;
	get_token();
    }
    else if (strcmp(word_string, "unexit") == 0) {
	exit_flag = FALSE;
	get_token();
    }

    else if (strcmp(word_string, "watch") == 0) {
	wp = allocate_watch();
	if (wp != NULL) {
	    wp->store_flag = TRUE;
	    wp->fetch_flag = TRUE;
	}
    }
    else if (strcmp(word_string, "unwatch") == 0)
	remove_watch();

    else if (strcmp(word_string, "store") == 0) {
	wp = allocate_watch();
	if (wp != NULL) wp->store_flag = TRUE;
    }
    else if (strcmp(word_string, "fetch") == 0) {
	wp = allocate_watch();
	if (wp != NULL) wp->fetch_flag = TRUE;
    }

    else if (strcmp(word_string, "show") == 0)
	show_value();
    else if (strcmp(word_string, "assign") == 0)
	assign_variable();

    else if (strcmp(word_string, "where") == 0) {
	print_statement();
	get_token();
    }
    else if (strcmp(word_string, "kill") == 0) {
	printf("Program killed.\n");
	exit(0);
    }
}

		/********************************/
		/*                              */
		/*      Tracing routines        */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  trace_statement_execution   Called just before the          */
/*                              execution of each statement.    */
/*--------------------------------------------------------------*/

trace_statement_execution()

{
    if (break_count > 0) {
	int i;

	/*
	--  Check if the statement is a breakpoint.
	*/
	for (i = 0; i < break_count; ++i) {
	    if (exec_line_number == break_list[i]) {
		printf("\nBreakpoint");
		print_statement();
		halt_flag = TRUE;
		break;
	    }
	}
    }

    /*
    --  Pause if necessary to read a debugger command.
    */
    if (halt_flag) {
	read_debugger_command();
	halt_flag = step_flag;
    }

    /*
    --  If single-stepping, print the current statement.
    --  If tracing, print the current line number.
    */
    if (step_flag)  print_statement();
    if (trace_flag) print_line_number();
}

/*--------------------------------------------------------------*/
/*  trace_routine_entry         Called upon entry into a        */
/*                              procedure or a function.        */
/*--------------------------------------------------------------*/

trace_routine_entry(idp)

    SYMTAB_NODE_PTR idp;        /* routine id */

{
    if (entry_flag) printf("\nEntering %s\n", idp->name);
}

/*--------------------------------------------------------------*/
/*  trace_routine_exit          Called upon exit from a         */
/*                              procedure or a function.        */
/*--------------------------------------------------------------*/

trace_routine_exit(idp)

    SYMTAB_NODE_PTR idp;        /* routine id */

{
    if (exit_flag) printf("\nExiting %s\n", idp->name);
}

/*--------------------------------------------------------------*/
/*  trace_data_store            Called just before a variable   */
/*                              is stored into.                 */
/*--------------------------------------------------------------*/

trace_data_store(idp, idp_tp, targetp, target_tp)

    SYMTAB_NODE_PTR idp;            /* id of target variable */
    TYPE_STRUCT_PTR idp_tp;         /* ptr to id's type */
    STACK_ITEM_PTR  targetp;        /* ptr to target location */
    TYPE_STRUCT_PTR target_tp;      /* ptr to target's type */

{
    /*
    --  Check if the variable is being watched for stores.
    */
    if ((idp->info != NULL) &&
	((WATCH_STRUCT_PTR) idp->info)->store_flag) {
	printf("\nAt %d:  Store %s", exec_line_number, idp->name);
	if      (idp_tp->form == ARRAY_FORM)  printf("[*]");
	else if (idp_tp->form == RECORD_FORM) printf(".*");
	print_data_value(targetp, target_tp, ":=");
    }
}

/*--------------------------------------------------------------*/
/*  trace_data_fetch            Called just before a variable   */
/*                              is fetched from.                */
/*--------------------------------------------------------------*/

trace_data_fetch(idp, tp, datap)

    SYMTAB_NODE_PTR idp;            /* id of target variable */
    TYPE_STRUCT_PTR tp;             /* ptr to id's type */
    STACK_ITEM_PTR  datap;          /* ptr to data */

{
    TYPE_STRUCT_PTR idp_tp = idp->typep;

    /*
    --  Check if the variable is being watched for fetches.
    */
    if (   (idp->info != NULL)
	&& ((WATCH_STRUCT_PTR) idp->info)->fetch_flag) {
	printf("\nAt %d:  Fetch %s", exec_line_number, idp->name);
	if      (idp_tp-> form == ARRAY_FORM)  printf("[*]");
	else if (idp_tp->form == RECORD_FORM) printf(".*");
	print_data_value(datap, tp, "=");
    }
}

		/********************************/
		/*                              */
		/*      Printing routines       */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  print_statement             Uncrunch and print a statement. */
/*--------------------------------------------------------------*/

print_statement()

{
    int     tk;                 /* token code */
    BOOLEAN done = FALSE;
    char    *csp = statement_startp;

    printf("\nAt %3d:", exec_line_number);

    do {
	switch (tk = *csp++) {

	    case SEMICOLON:
	    case END:
	    case ELSE:
	    case THEN:
	    case UNTIL:
	    case BEGIN:
	    case OF:
	    case STATEMENT_MARKER:      done = TRUE;
					break;

	    default:
		done = FALSE;

		switch (tk) {

		    case ADDRESS_MARKER:
			csp += sizeof(ADDRESS);
			break;

		    case IDENTIFIER:
		    case NUMBER:
		    case STRING: {
			SYMTAB_NODE_PTR np = *((SYMTAB_NODE_PTR *) csp);

			printf(" %s", np->name);
			csp += sizeof(SYMTAB_NODE_PTR);
			break;
		    }

		    default:
			printf(" %s", symbol_strings[tk]);
			break;
		}
	}
    } while (!done);

    printf("\n");
}

/*--------------------------------------------------------------*/
/*  print_line_number           Print the current line number.  */
/*--------------------------------------------------------------*/

print_line_number()

{
    printf("<%d>", exec_line_number);
}

/*--------------------------------------------------------------*/
/*  print_data_value            Print a data value.             */
/*--------------------------------------------------------------*/

print_data_value(datap, tp, str)

    STACK_ITEM_PTR  datap;      /* ptr to data value to print */
    TYPE_STRUCT_PTR tp;         /* ptr to type of stack item */
    char            *str;       /* " = " or " := " */

{
    /*
    --  Reduce a subrange type to its range type.
    --  Convert a non-boolean enumeration type to integer.
    */
    if (tp->form == SUBRANGE_FORM)
	tp = tp->info.subrange.range_typep;
    if ((tp->form == ENUM_FORM) && (tp != boolean_typep))
	tp = integer_typep;

    if (tp == integer_typep)
	printf(" %s %d\n", str, datap->integer);
    else if (tp == real_typep)
	printf(" %s %0.6g\n", str, datap->real);
    else if (tp == boolean_typep)
	printf(" %s %s\n", str, datap->integer == 1 ? "true"
						    : "false");
    else if (tp == char_typep)
	printf(" %s '%c'\n", str, datap->byte);
    else if (tp->form == ARRAY_FORM) {
	if (tp->info.array.elmt_typep == char_typep) {
	    char *chp = (char *) datap;
	    int  size = tp->info.array.elmt_count;

	    printf(" %s '", str);
	    while (size--) printf("%c", *chp++);
	    printf("'\n");
	}
	else printf(" %s <array>\n", str);
    }
    else if (tp->form == RECORD_FORM)
	printf(" %s <record>\n", str);
}

		/****************************************/
		/*                                      */
		/*      Breakpoints and watches         */
		/*                                      */
		/****************************************/

/*--------------------------------------------------------------*/
/*  set_breakpoint      Set a breakpoint, or print all          */
/*                      breakpoints in the break list.          */
/*--------------------------------------------------------------*/

set_breakpoint()

{
    get_token();

    switch (token) {

	case SEMICOLON: {
	    /*
	    --  No line number:  List all breakpoints.
	    */
	    int i;

	    printf("Statement breakpoints at:\n");

	    for (i = 0; i < break_count; ++i)
		printf("%5d\n", break_list[i]);

	    break;
	}

	case NUMBER: {
	    /*
	    --  Set a breakpoint by appending it to
	    --  the break list.
	    */
	    int number;

	    if (literal.type == INTEGER_LIT) {
		number = literal.value.integer;
		if ((number > 0) && (number <= line_number)) {
		    if (break_count < MAX_BREAKS) {
			break_list[break_count] = number;
			++break_count;
		    }
		    else printf("Break list is full.\n");
		}
		else error(VALUE_OUT_OF_RANGE);
	    }
	    else error(UNEXPECTED_TOKEN);

	    get_token();
	    break;
	}
    }
}

/*--------------------------------------------------------------*/
/*  remove_breakpoint   Remove a specific breakpoint, or remove */
/*                      all breakpoints.                        */
/*--------------------------------------------------------------*/

remove_breakpoint()

{
    int i, j, number;

    get_token();

    switch (token) {

	case SEMICOLON: {
	    /*
	    --  No line number:  Remove all breakpoints.
	    */
	    for (i = 0; i < break_count; ++i) break_list[i] = 0;
	    break_count = 0;
	    break;
	}

	case NUMBER: {
	    /*
	    --  Remove a breakpoint from the break list.
	    --  Move the following breakpoints up one in the
	    --  list to fill in the gap.
	    */
	    if (literal.type == INTEGER_LIT) {
		number = literal.value.integer;
		if (number > 0) {
		    for (i = 0; i < break_count; ++i) {
			if (break_list[i] == number) {
			    break_list[i] = 0;
			    --break_count;

			    for (j = i; j < break_count; ++j)
				break_list[j] = break_list[j+1];

			    break;
			}
		    }
		}
		else error(VALUE_OUT_OF_RANGE);
	    }

	    get_token();
	    break;
	}
    }
}

/*--------------------------------------------------------------*/
/*  allocate_watch      Return a pointer to a watch structure,  */
/*                      or print all variables being watched.   */
/*--------------------------------------------------------------*/

    WATCH_STRUCT_PTR
allocate_watch()

{
    int              i;
    SYMTAB_NODE_PTR  idp;
    WATCH_STRUCT_PTR wp;

    get_token();

    switch (token) {

	case SEMICOLON: {
	    /*
	    --  No variable:  Print all variables being watched.
	    */
	    printf("Variables being watched:\n");

	    for (i = 0; i < watch_count; ++i) {
		idp = watch_list[i];
		if (idp != NULL) {
		    wp = (WATCH_STRUCT_PTR) idp->info;
		    printf ("%16s  ", idp->name);
		    if (wp->store_flag) printf(" (store)");
		    if (wp->fetch_flag) printf(" (fetch)");
		    printf("\n");
		}
	    }

	    return(NULL);
	}

	case IDENTIFIER: {
	    search_and_find_all_symtab(idp);
	    get_token();

	    switch (idp->defn.key) {

		case UNDEFINED:
		    return(NULL);

		case CONST_DEFN:
		case VAR_DEFN:
		case FIELD_DEFN:
		case VALPARM_DEFN:
		case VARPARM_DEFN:  {
		    /*
		    --  Return a pointer to the variable's watch
		    --  structure if it is already being watched.
		    --  Otherwise, allocate and return a pointer
		    --  to a new watch structure.
		    */
		    if (idp->info != NULL)
			return((WATCH_STRUCT_PTR) idp->info);
		    else if (watch_count < MAX_WATCHES) {
			wp = alloc_struct(WATCH_STRUCT);
			wp->store_flag = FALSE;
			wp->fetch_flag = FALSE;

			idp->info = (char *) wp;

			watch_list[watch_count] = idp;
			++watch_count;

			return(wp);
		    }
		    else {
			printf("Watch list is full.\n");
			return(NULL);
		    }
		}

		default: {
		    error(INVALID_IDENTIFIER_USAGE);
		    return(NULL);
		}
	    }
	}
    }
}

/*--------------------------------------------------------------*/
/*  remove_watch        Remove a specific variable from being   */
/*                      watched, or remove all variables from   */
/*                      the watch list.                         */
/*--------------------------------------------------------------*/

remove_watch()

{
    int              i, j;
    SYMTAB_NODE_PTR  idp;
    WATCH_STRUCT_PTR wp;

    get_token();

    switch (token) {

	case SEMICOLON: {
	    /*
	    --  No variable:  Remove all variables from watch list.
	    */
	    for (i = 0; i < watch_count; ++i) {
		if ((idp = watch_list[i]) != NULL) {
		    wp = (WATCH_STRUCT_PTR) idp->info;
		    watch_list[i] = NULL;
		    idp->info = NULL;
		    free(wp);
		}
	    }
	    watch_count = 0;
	    break;
	}

	case IDENTIFIER: {
	    /*
	    --  Remove a variable from the watch list.
	    --  Move the following watches up one in the
	    --  list to fill in the gap.
	    */
	    search_and_find_all_symtab(idp);
	    get_token();

	    if ((idp != NULL) && (idp->info != NULL)) {
		wp = (WATCH_STRUCT_PTR) idp->info;
		for (i = 0; i < watch_count; ++i) {
		    if (watch_list[i] == idp) {
			watch_list[i] = NULL;
			idp->info = NULL;
			free(wp);
			--watch_count;

			for (j = i; j < watch_count; ++j)
			    watch_list[j] = watch_list[j+1];

			break;
		    }
		}
	    }
	    break;
	}
    }
}

		/********************************/
		/*                              */
		/*      Show and assign         */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  show_value          Print the value of an expression.       */
/*--------------------------------------------------------------*/

show_value()

{
    get_token();

    switch (token) {

	case SEMICOLON: {
	    error(INVALID_EXPRESSION);
	    break;
	}

	default: {
	    /*
	    --  First parse, then execute the expression
	    --  from the code buffer.
	    */
	    TYPE_STRUCT_PTR expression();
	    TYPE_STRUCT_PTR tp       = expression();    /* parse */
	    char *save_code_segmentp = code_segmentp;
	    int  save_ctoken         = ctoken;

	    if (error_count > 0) break;

	    /*
	    --  Switch to the code buffer.
	    */
	    code_segmentp = code_buffer + 1;
	    get_ctoken();
	    exec_expression();                          /* execute */

	    /*
	    --  Print, then pop off the value.
	    */
	    if ((tp->form == ARRAY_FORM) ||
		(tp->form == RECORD_FORM))
		print_data_value(tos->address, tp, " ");
	    else
		print_data_value(tos, tp, " ");

	    pop();

	    /*
	    --  Resume the code segment.
	    */
	    code_segmentp = save_code_segmentp;
	    ctoken = save_ctoken;
	    break;
	}
    }
}

/*--------------------------------------------------------------*/
/*  assign_variable     Execute an assignment statement.        */
/*--------------------------------------------------------------*/

assign_variable()

{
    get_token();

    switch (token) {

	case SEMICOLON: {
	    error(MISSING_VARIABLE);
	    break;
	}

	case IDENTIFIER: {
	    /*
	    --  First parse, then execute the assignment statement
	    --  from the code buffer.
	    */
	    SYMTAB_NODE_PTR idp;
	    char *save_code_segmentp = code_segmentp;
	    int  save_ctoken         = ctoken;

	    search_and_find_all_symtab(idp);

	    assignment_statement(idp);          /* parse */
	    if (error_count > 0) break;

	    /*
	    --  Switch to the code buffer.
	    */
	    code_segmentp = code_buffer + 1;
	    get_ctoken();
	    idp = get_symtab_cptr();
	    exec_assignment_statement(idp);     /* execute */

	    /*
	    --  Resume the code segment.
	    */
	    code_segmentp = save_code_segmentp;
	    ctoken = save_ctoken;
	    break;
	}
    }
}

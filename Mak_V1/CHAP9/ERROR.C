/****************************************************************/
/*                                                              */
/*      E R R O R   R O U T I N E S                             */
/*                                                              */
/*      Error messages and routines to print them.              */
/*                                                              */
/*      FILE:       error.c                                     */
/*                                                              */
/*      MODULE:     error                                       */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern char	*tokenp;
extern BOOLEAN	print_flag;
extern char	source_buffer[];
extern char     *bufferp;

/*--------------------------------------------------------------*/
/*  Error messages	Keyed to enumeration type ERROR_CODE	*/
/*			in file error.h.			*/
/*--------------------------------------------------------------*/

char *error_messages[] = {
    "No error",
    "Syntax error",
    "Too many syntax errors",
    "Failed to open source file",
    "Unexpected end of file",
    "Invalid number",
    "Invalid fraction",
    "Invalid exponent",
    "Too many digits",
    "Real literal out of range",
    "Integer literal out of range",
    "Missing right parenthesis",
    "Invalid expression",
    "Invalid assignment statement",
    "Missing identifier",
    "Missing := ",
    "Undefined identifier",
    "Stack overflow",
    "Invalid statement",
    "Unexpected token",
    "Missing ; ",
    "Missing DO",
    "Missing UNTIL",
    "Missing THEN",
    "Invalid FOR control variable",
    "Missing OF",
    "Invalid constant",
    "Missing constant",
    "Missing : ",
    "Missing END",
    "Missing TO or DOWNTO",
    "Redefined identifier",
    "Missing = ",
    "Invalid type",
    "Not a type identifier",
    "Invalid subrangetype",
    "Not a constant identifier",
    "Missing .. ",
    "Incompatible types",
    "Invalid assignment target",
    "Invalid identifier usage",
    "Incompatible assignment",
    "Min limit greater than max limit",
    "Missing [ ",
    "Missing ] ",
    "Invalid index type",
    "Missing BEGIN",
    "Missing period",
    "Too many subscripts",
    "Invalid field",
    "Nesting too deep",
    "Missing PROGRAM",
    "Already specified in FORWARD",
    "Wrong number of actual parameters",
    "Invalid VAR parameter",
    "Not a record variable",
    "Missing variable",
    "Code segment overflow",
    "Unimplemented feature",
};

char *runtime_error_messages[] = {
    "Runtime stack overflow",
    "Value out of range",
    "Invalid CASE expression value",
    "Division by zero",
    "Invalid standard function argument",
    "Unimplemented runtime feature",
};

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

int error_count = 0;    /* number of syntax errors */

		/********************************/
		/*				*/
		/*	Error routines		*/
		/*				*/
		/********************************/

/*--------------------------------------------------------------*/
/*  error		Print an arrow under the error and then	*/
/*			print the error message.		*/
/*--------------------------------------------------------------*/

error(code)

    ERROR_CODE code;	/* error code */

{
    extern int buffer_offset;
    char message_buffer[MAX_PRINT_LINE_LENGTH];
    char *message = error_messages[code];
    int  offset   = buffer_offset - 2;

    /*
    --	Print the arrow pointing to the token just scanned.
    */
    if (print_flag) offset += 8;
    sprintf(message_buffer, "%*s^\n", offset, " ");
    if (print_flag) print_line(message_buffer);
    else            printf(message_buffer);

    /*
    --	Print the error message.
    */
    sprintf(message_buffer, " *** ERROR: %s.\n", message);
    if (print_flag) print_line(message_buffer);
    else            printf(message_buffer);

    *tokenp = '\0';
    ++error_count;

    if (error_count > MAX_SYNTAX_ERRORS) {
	sprintf(message_buffer,
		"Too many syntax errors.  Aborted.\n");
	if (print_flag) print_line(message_buffer);
	else            printf(message_buffer);

	exit(-TOO_MANY_SYNTAX_ERRORS);
    }
}

/*--------------------------------------------------------------*/
/*  runtime_error       Print a runtime error message and then  */
/*                      abort the program execution.            */
/*--------------------------------------------------------------*/

runtime_error(code)

    ERROR_CODE code;    /* error code */

{
    char        *message = runtime_error_messages[code];
    extern int  exec_line_number;
    extern long exec_stmt_count;

    printf("\n*** RUNTIME ERROR in line %d: %s\n",
	   exec_line_number, message);
    printf("\nUnsuccessful completion.  %ld statements executed.\n\n",
	   exec_stmt_count);
    exit(-code);
}


/****************************************************************/
/*                                                              */
/*      Program 5-1:  Syntax Checker I                          */
/*                                                              */
/*      Check the syntax of Pascal statements.                  */
/*                                                              */
/*      FILE:       syntax1.c                                   */
/*                                                              */
/*      REQUIRES:   Modules parser, symbol table, scanner,      */
/*                          error                      		*/
/*								*/
/*      USAGE:      syntax1 sourcefile                          */
/*								*/
/*	    sourcefile	    name of source file containing	*/
/*			    statements to be checked		*/
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE token;
extern int        line_number;
extern int        error_count;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

char buffer[MAX_PRINT_LINE_LENGTH];

/*--------------------------------------------------------------*/
/*  main		Initialize the scanner and call	the	*/
/*			statement routine.			*/
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    /*
    --  Initialize the scanner.
    */
    init_scanner(argv[1]);

    /*
    --  Parse a statement.
    */
    get_token();
    statement();

    /*
    --  Look for the end of file.
    */
    while (token != END_OF_FILE) {
	error(UNEXPECTED_TOKEN);
	get_token();
    }

    quit_scanner();

    /*
    --  Print the parser's summary.
    */
    print_line("\n");
    print_line("\n");
    sprintf(buffer, "%20d Source lines.\n", line_number);
    print_line(buffer);
    sprintf(buffer, "%20d Source errors.\n", error_count);
    print_line(buffer);

    if (error_count == 0) exit(0);
    else                  exit(-SYNTAX_ERROR);
}

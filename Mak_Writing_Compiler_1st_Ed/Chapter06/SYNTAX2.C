/****************************************************************/
/*                                                              */
/*      Program 6-2:  Pascal Syntax Checker II                  */
/*                                                              */
/*      Check the syntax of Pascal declarations and             */
/*      statements.  Perform type checking.                     */
/*                                                              */
/*      FILE:       syntax2.c                                   */
/*                                                              */
/*      REQUIRES:   Modules parser, symbol table, scanner,      */
/*                          error                       	*/
/*								*/
/*      USAGE:      syntax2 sourcefile                          */
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
#include "parser.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE token;
extern int        line_number, error_count;

extern TYPE_STRUCT dummy_type;

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
    SYMTAB_NODE_PTR program_idp;        /* artificial program id */

    /*
    --  Initialize the scanner and the symbol table.
    */
    init_scanner(argv[1]);
    init_symtab();

    /*
    --  Create an artifical program id node.
    */
    program_idp = alloc_struct(SYMTAB_NODE);
    program_idp->defn.key = PROG_DEFN;
    program_idp->defn.info.routine.key = DECLARED;
    program_idp->defn.info.routine.parm_count = 0;
    program_idp->defn.info.routine.total_parm_size = 0;
    program_idp->defn.info.routine.total_local_size = 0;
    program_idp->typep = &dummy_type;
    program_idp->label_index = 0;

    /*
    --  Parse a block.
    */
    get_token();
    block(program_idp);

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

    compound_statement();
}

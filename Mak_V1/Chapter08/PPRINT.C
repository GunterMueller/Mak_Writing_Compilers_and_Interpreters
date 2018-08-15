/****************************************************************/
/*                                                              */
/*      Program 8-1:  Pascal Pretty Printer                     */
/*                                                              */
/*      Read and check the syntax of a Pascal program,          */
/*      and then print it out in a nicely-indented format.      */
/*                                                              */
/*      FILE:       pprint.c                                    */
/*                                                              */
/*      REQUIRES:   Modules parser, symbol table, scanner,      */
/*                          error                               */
/*                                                              */
/*                  Files ppdecl.c, ppstmt.c                    */
/*                                                              */
/*      FLAGS:      Macro flag "analyze" must be defined        */
/*								*/
/*      USAGE:      pprint sourcefile                           */
/*                                                              */
/*          sourcefile      name of source file containing      */
/*                          the program to be pretty-printed    */
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
#include "pprint.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern BOOLEAN print_flag;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

char *code_buffer;                      /* code buffer */
char *code_bufferp;                     /* code buffer ptr */
char *code_segmentp;                    /* code segment ptr */
char *code_segment_limit;               /* end of code segment */

TOKEN_CODE ctoken;                      /* token from code segment */

char pprint_buffer[MAX_PRINT_LINE_LENGTH];   /* print buffer */
int  left_margin = 0;                        /* margin in buffer */

/*--------------------------------------------------------------*/
/*  main                Initialize the scanner and call         */
/*                      routine program.                        */
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    /*
    --  Initialize the scanner.
    */
    print_flag = FALSE;
    init_scanner(argv[1]);

    /*
    --  Process a program.
    */
    get_token();
    program();
    quit_scanner();
}

/*--------------------------------------------------------------*/
/*  emit                Emit a string to the print buffer.      */
/*--------------------------------------------------------------*/

emit(string)

    char *string;

{
    int buffer_length = strlen(pprint_buffer);
    int string_length = strlen(string);

    if (buffer_length + string_length >= MAX_PRINT_LINE_LENGTH - 1) {
	flush();
	indent();
    }

    strcat(pprint_buffer, string);
}

/*--------------------------------------------------------------*/
/*  indent              Indent left_margin spaces in the print  */
/*                      buffer.                                 */
/*--------------------------------------------------------------*/

indent()

{
    if (left_margin > 0)
	sprintf(pprint_buffer, "%*s", left_margin, " ");
    else
	pprint_buffer[0] = '\0';
}

/*--------------------------------------------------------------*/
/*  flush               Print the print buffer if there is      */
/*                      anything in it.                         */
/*--------------------------------------------------------------*/

flush()

{
    if (pprint_buffer[0] != '\0') {
	printf("%s\n", pprint_buffer);
	pprint_buffer[0] = '\0';
    }
}

/****************************************************************/
/*                                                              */
/*      Program 7-1:  Pascal Syntax Checker III                 */
/*                                                              */
/*      Read and check the syntax of a Pascal program.          */
/*                                                              */
/*      FILE:       syntax3.c                                   */
/*                                                              */
/*      REQUIRES:   Modules parser, symbol table, scanner,      */
/*                          error                       	*/
/*								*/
/*      USAGE:      syntax3 sourcefile                          */
/*                                                              */
/*          sourcefile      name of source file containing      */
/*                          the program to be checked           */
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
    --  Process a program.
    */
    get_token();
    program();
}

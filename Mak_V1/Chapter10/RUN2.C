/****************************************************************/
/*                                                              */
/*      Program 10-1:  Pascal Interpreter II                    */
/*                                                              */
/*      Interpret a Pascal program.                             */
/*                                                              */
/*      FILE:       run2.c                                      */
/*                                                              */
/*      REQUIRES:   Modules parser, symbol table, scanner,      */
/*                          executor, error             	*/
/*								*/
/*      USAGE:      run2 sourcefile                             */
/*                                                              */
/*          sourcefile      name of source file containing      */
/*                          the Pascal program to interpret     */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "symtab.h"
#include "exec.h"

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
    init_scanner(argv[1]);

    /*
    --  Process a program.
    */
    get_token();
    program();
}


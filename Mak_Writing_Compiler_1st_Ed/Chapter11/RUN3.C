/****************************************************************/
/*                                                              */
/*      Program 11-1:  Interactive Pascal Debugger              */
/*                                                              */
/*      Interpret a Pascal program under the control of an      */
/*      interactive debugger.                                   */
/*                                                              */
/*      FILE:       run3.c                                      */
/*                                                              */
/*      REQUIRES:   Modules parser, symbol table, scanner,      */
/*                          executor, error             	*/
/*                                                              */
/*      FLAGS:      Macro flag "trace" must be defined          */
/*                                                              */
/*      USAGE:      run3 sourcefile                             */
/*                                                              */
/*          sourcefile      name of source file containing      */
/*                          the Pascal program to interpret     */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>

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


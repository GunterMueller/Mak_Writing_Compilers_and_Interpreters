//fig 4-13
//  *************************************************************
//  *                                                           *
//  *   Program 4-2:  Source File Cruncher                      *
//  *                                                           *
//  *   Crunch a Pascal source file into a compressed           *
//  *   intermediate code.  It can be restored later with the   *
//  *   Decruncher utility.                                     *
//  *                                                           *
//  *   FILE:   prog4-2/crunch.cpp                              *
//  *                                                           *
//  *   USAGE:  crunch <source file> <crunch file>              *
//  *                                                           *
//  *               <source file>  name of source file to       *
//  *                              crunch                       *
//  *                                                           *
//  *               <icode file>   name of the intermediate     *
//  *                              code file                    *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include "common.h"
#include "error.h"
#include "buffer.h"
#include "symtab.h"
#include "icode.h"
#include "parser.h"

TIcode *pIcode;  // ptr to the intermediate code

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    //--Check the command line arguments.
    if (argc != 3) {
	cerr << "Usage: crunch <source file> <icode file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the intermediate code.
    pIcode = new TIcode(argv[2], TIcode::output);

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser parser(new TSourceBuffer(argv[1]));
    parser.Parse();

    //--Clean up.
    delete pIcode;
}
//endfig


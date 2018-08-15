//fig 5-13
//  *************************************************************
//  *                                                           *
//  *   Program 5-1: Calculator                                 *
//  *                                                           *
//  *   Execute assignment statements with simple expressions   *
//  *   line-by-line.  Print the name and value of each         *
//  *   variable that is used in an expression or that is the   *
//  *   target of an assignment.                                *
//  *                                                           *
//  *   FILE:   prog5-1/calc.cpp                                *
//  *                                                           *
//  *   USAGE:  calc <source file>                              *
//  *                                                           *
//  *               <source file>  name of the source file      *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include "error.h"
#include "buffer.h"
#include "parser.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: calc <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser parser(new TSourceBuffer(argv[1]));
    parser.Parse();
}
//endfig

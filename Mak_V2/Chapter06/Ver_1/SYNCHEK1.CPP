//fig 6-8
//  *************************************************************
//  *                                                           *
//  *   Program 6-1: Syntax Checker I                           *
//  *                                                           *
//  *   Check the syntax of a "program" consisting of a         *
//  *   compound statement containing simple assignment and     *
//  *   control statements.                                     *
//  *                                                           *
//  *   FILE:   prog6-1/synchek1.cpp                            *
//  *                                                           *
//  *   USAGE:  synchek1 <source file>                          *
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
	cerr << "Usage: synchek1 <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *pParser = new TParser(new TSourceBuffer(argv[1]));
    pParser->Parse();
    delete pParser;
}
//endfig

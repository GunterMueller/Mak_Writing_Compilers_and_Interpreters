//fig 7-21
//  *************************************************************
//  *                                                           *
//  *   Program 7-2: Syntax Checker II                          *
//  *                                                           *
//  *   Check the syntax of a "program" consisting of           *
//  *   declarations followed by a statement.                   *
//  *                                                           *
//  *   FILE:   prog7-2/synchek2.cpp                            *
//  *                                                           *
//  *   USAGE:  synchek2 <source file>                          *
//  *                                                           *
//  *               <source file>  name of the source file      *
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
#include "parser.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: synchek2 <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *pParser = new TParser(new TSourceBuffer(argv[1]));
    pParser->Parse();
    delete pParser;
}
//endfig

//fig 8-21
//  *************************************************************
//  *                                                           *
//  *   Program 8-1: Syntax Checker III                         *
//  *                                                           *
//  *   Check the syntax of a complete Pascal program.          *
//  *                                                           *
//  *   FILE:   prog8-1/synchek3.cpp                            *
//  *                                                           *
//  *   USAGE:  synchek3 <source file>                          *
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
	cerr << "Usage: synchek3 <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *pParser = new TParser(new TSourceBuffer(argv[1]));
    pParser->Parse();
    delete pParser;
}
//endfig


//fig 7-15
//  *************************************************************
//  *                                                           *
//  *   Program 7-1: Cross-Referencer II                        *
//  *                                                           *
//  *   Generate a cross-reference listing that includes        *
//  *   information about each identifier.                      *
//  *                                                           *
//  *   FILE:   prog7-1/xref2.cpp                               *
//  *                                                           *
//  *   USAGE:  xref2 <source file>                             *
//  *                                                           *
//  *               <source file>  name of the source file      *
//  *                                                           *
//  *               -x             turn on cross-referencing    *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include <string.h>
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
    extern int xrefFlag;

    //--Check the command line arguments.
    if ((argc != 2) && (argc != 3)) {
	cerr << "Usage: xref2 <source file> [-x]" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Set the cross-referencing flag.
    xrefFlag = (argc == 3) && (strcmp(argv[2], "-x") == 0);

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *pParser = new TParser(new TSourceBuffer(argv[1]));
    pParser->Parse();
    delete pParser;

    //--Print the cross-reference.
    if (xrefFlag) {
	list.PutLine();
	list.PutLine("***** Cross-Reference *****");
	list.PutLine();
	globalSymtab.Print();
    }
}
//endfig

//fig 4-9
//  *************************************************************
//  *                                                           *
//  *   Program 4-1:  Cross-Referencer I                        *
//  *                                                           *
//  *   After listing the source file, list all of its          *
//  *   identifiers alphabetically.  For each identifier, list  *
//  *   the line numbers of the source lines that contain it.   *
//  *                                                           *
//  *   FILE:   prog4-1/xref1.cpp                               *
//  *                                                           *
//  *   USAGE:  xref1 <source file> [-x]                        *
//  *                                                           *
//  *               <source file>  name of source file to       *
//  *                              cross-reference              *
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
	cerr << "Usage: xref1 <source file> [-x]" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Set the cross-referencing flag.
    xrefFlag = (argc == 3) && (strcmp(argv[2], "-x") == 0);

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser parser(new TSourceBuffer(argv[1]));
    parser.Parse();

    //--Print the cross-reference.
    if (xrefFlag) {
	list.PutLine();
	list.PutLine("***** Cross-Reference *****");
	list.PutLine();
	globalSymtab.Print();
    }
}
//endfig


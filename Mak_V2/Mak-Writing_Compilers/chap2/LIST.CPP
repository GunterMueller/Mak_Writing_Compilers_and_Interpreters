//fig 2-10
//  *************************************************************
//  *                                                           *
//  *   Program 2-1:  Source File Lister                        *
//  *                                                           *
//  *   Print the contents of the source file in a format that  *
//  *   includes page headings, line numbers, and nesting       *
//  *   levels.                                                 *
//  *                                                           *
//  *   FILE:   prog2-1/list.cpp                              	*
//  *                                                           *
//  *   USAGE:  list <source file>                              *
//  *                                                           *
//  *               <source file>  name of source file to list  *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include "error.h"
#include "buffer.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    char ch;  // character fetched from source

    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: list <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create source file buffer.
    TSourceBuffer source(argv[1]);

    //--Loop to fetch each character of the source.
    do {
	ch = source.GetChar();
    } while (ch != eofChar);
}

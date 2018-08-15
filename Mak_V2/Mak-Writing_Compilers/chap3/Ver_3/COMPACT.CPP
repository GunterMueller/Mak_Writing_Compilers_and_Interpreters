//fig 3-28
//  *************************************************************
//  *                                                           *
//  *   Program 3-3:  Pascal Source Compactor                   *
//  *                                                           *
//  *   Compact a Pascal source file by removing                *
//  *   unnecessary blanks and all comments.                    *
//  *                                                           *
//  *   FILE:   prog3-3/compact.cpp                             *
//  *                                                           *
//  *   USAGE:  compact <source file>                           *
//  *                                                           *
//  *               <source file>  name of source file to       *
//  *                              compact                      *
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
	cerr << "Usage: compact <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    errorArrowFlag = false;  // don't print arrows under syntax errors
    listFlag       = false;  // don't list the source file

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser parser(new TSourceBuffer(argv[1]));
    parser.Parse();
}
//endfig

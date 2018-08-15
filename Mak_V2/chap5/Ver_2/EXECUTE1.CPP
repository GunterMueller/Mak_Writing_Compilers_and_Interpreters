//fig 5-32
//  *************************************************************
//  *                                                           *
//  *   Program 5-2: Simple Executor I				*
//  *                                                           *
//  *   Execute a "program" of simple assignment statements.  	*
//  *   Whenever variable "output" is the assignment target,    *
//  *   print the assigned value.  Whenever variable "input"    *
//  *   appears in an expression, prompt for its value.         *
//  *                                                           *
//  *   FILE:   prog5-2/execute1.cpp                            *
//  *                                                           *
//  *   USAGE:  execute1 <source file>                          *
//  *                                                           *
//  *               <source file>  name of the source file      *
//  *                                                           *
//  *   Copyright (c) 1995 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include "common.h"
#include "error.h"
#include "buffer.h"
#include "symtab.h"
#include "parser.h"
#include "backend.h"
#include "exec.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: execute1 <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *pParser = new TParser(new TSourceBuffer(argv[1]));
    pParser->Parse();
    delete pParser;

    //--If there were no syntax errors, convert the symbol tables,
    //--and create and invoke the backend executor.
    if (errorCount == 0) {
	vpSymtabs = new TSymtab *[cntSymtabs];
	for (TSymtab *pSt = pSymtabList; pSt; pSt = pSt->Next()) {
	    pSt->Convert(vpSymtabs);
	}

	TBackend *pBackend = new TExecutor;
	pBackend->Go();

	delete[] vpSymtabs;
	delete   pBackend;
    }
}
//endfig

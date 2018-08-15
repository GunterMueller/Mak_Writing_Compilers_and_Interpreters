//fig 6-11
//  *************************************************************
//  *                                                           *
//  *   Program 6-2: Simple Executor II                         *
//  *                                                           *
//  *   Execute "programs" consisting of a compound statement   *
//  *   containing assignment statements and REPEAT statements. *
//  *                                                           *
//  *   FILE:   prog6-2/execute2.cpp                            *
//  *                                                           *
//  *   USAGE:  execute2 <source file>                          *
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
#include "backend.h"
#include "exec.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    extern int errorCount;

    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: execute2 <source file>" << endl;
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

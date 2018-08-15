//fig 10-15
//  *************************************************************
//  *                                                           *
//  *   Program 10-1: Pascal Interpreter                     	*
//  *                                                           *
//  *   Interpret a Pascal program.                             *
//  *                                                           *
//  *   FILE:   prog10-1/run.cpp                                *
//  *                                                           *
//  *   USAGE:  run <source file>                              	*
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
    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: run <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser     *pParser    = new TParser(new TSourceBuffer(argv[1]));
    TSymtabNode *pProgramId = pParser->Parse();
    delete pParser;

    //--If there were no syntax errors, convert the symbol tables,
    //--and create and invoke the backend pretty-printer.
    if (errorCount == 0) {
	vpSymtabs = new TSymtab *[cntSymtabs];
	for (TSymtab *pSt = pSymtabList; pSt; pSt = pSt->Next()) {
	    pSt->Convert(vpSymtabs);
	}

	TBackend *pBackend = new TExecutor;
	pBackend->Go(pProgramId);

	delete[] vpSymtabs;
	delete   pBackend;
    }
}
//endfig

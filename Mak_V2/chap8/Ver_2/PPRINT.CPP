//fig 8-27
//  *************************************************************
//  *                                                           *
//  *   Program 8-2: Pascal Pretty Printer                      *
//  *                                                           *
//  *   Reformat and print a Pascal source program in a         *
//  *   "pretty" way.                                           *
//  *                                                           *
//  *   FILE:   prog8-2/pprint.cpp                              *
//  *                                                           *
//  *   USAGE:  pprint <source file>                            *
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
#include "pprinter.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    extern listFlag;

    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: pprint <source file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the parser for the source file,
    //--and then parse the file.  Don't list the source.
    listFlag = false;
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

	TBackend *pBackend = new TPrettyPrinter;
	pBackend->Go(pProgramId);

	delete[] vpSymtabs;
	delete   pBackend;
    }
}
//endfig

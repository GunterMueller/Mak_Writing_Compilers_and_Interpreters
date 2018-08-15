//fig 13-8
//  *************************************************************
//  *                                                           *
//  *   Program 13-1: Code emitter                              *
//  *                                                           *
//  *   Compile a Pascal program consisting of procedures,      *
//  *   functions, assignment statements, and calls to write    *
//  *   and writeln into an assembly language object program.   *
//  *                                                           *
//  *   FILE:   prog13-1/emit.cpp                               *
//  *                                                           *
//  *   USAGE:  emit <source file> <assembly file>              *
//  *                                                           *
//  *               <source file>    name of the source file    *
//  *               <assembly file>  name of the assembly       *
//  *                                  language object file     *
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
#include "codegen.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    extern int execFlag;

    //--Check the command line arguments.
    if (argc != 3) {
	cerr << "Usage: emit <source file> <asssembly file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    execFlag = false;

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser     *pParser    = new TParser(new TSourceBuffer(argv[1]));
    TSymtabNode *pProgramId = pParser->Parse();
    delete pParser;

    //--If there were no syntax errors, convert the symbol tables,
    //--and create and invoke the backend code generator.
    if (errorCount == 0) {
	vpSymtabs = new TSymtab *[cntSymtabs];
	for (TSymtab *pSt = pSymtabList; pSt; pSt = pSt->Next()) {
	    pSt->Convert(vpSymtabs);
	}

	TBackend *pBackend = new TCodeGenerator(argv[2]);
	pBackend->Go(pProgramId);

	delete[] vpSymtabs;
	delete   pBackend;
    }
}
//endfig

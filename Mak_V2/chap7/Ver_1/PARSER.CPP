//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *	Parse the source file for the Cross-Referncer II  	*
//  *	utility program.					*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog7-1/parser.cpp                             *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "common.h"
#include "parser.h"

//fig 7-10
//--------------------------------------------------------------
//  Parse       Parse the source file.
//--------------------------------------------------------------

void TParser::Parse(void)
{
    //--Create a dummy program identifier symbol table node.
    TSymtabNode dummyProgramId("DummyProgram", dcProgram);
    dummyProgramId.defn.routine.locals.pParmIds	    = NULL;
    dummyProgramId.defn.routine.locals.pConstantIds = NULL;
    dummyProgramId.defn.routine.locals.pTypeIds     = NULL;
    dummyProgramId.defn.routine.locals.pVariableIds = NULL;
    dummyProgramId.defn.routine.pSymtab		    = NULL;
    dummyProgramId.defn.routine.pIcode		    = NULL;

    //--Extract the first token and parse the declarations.
    GetToken();
    ParseDeclarations(&dummyProgramId);

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.",  currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();
}
//endfig

//--------------------------------------------------------------
//  Resync          Resynchronize the parser.  If the current
//                  token is not in one of the token lists,
//                  flag it as an error and then skip tokens
//                  up to one that is in a list or end of file.
//--------------------------------------------------------------

void TParser::Resync(const TTokenCode *pList1,
		     const TTokenCode *pList2,
		     const TTokenCode *pList3)
{
    //--Is the current token in one of the lists?
    int errorFlag = (! TokenIn(token, pList1)) &&
		    (! TokenIn(token, pList2)) &&
		    (! TokenIn(token, pList3));

    if (errorFlag) {

	//--Nope.  Flag it as an error.
	TErrorCode errorCode = token == tcEndOfFile
				    ? errUnexpectedEndOfFile
				    : errUnexpectedToken;
	Error(errorCode);

	//--Skip tokens.
	while ((! TokenIn(token, pList1)) &&
	       (! TokenIn(token, pList2)) &&
	       (! TokenIn(token, pList3)) &&
	       (token != tcPeriod)        &&
	       (token != tcEndOfFile)) {
	    GetToken();
	}

	//--Flag an unexpected end of file (if haven't already).
	if ((token == tcEndOfFile) &&
	    (errorCode != errUnexpectedEndOfFile)) {
	    Error(errUnexpectedEndOfFile);
	}
    }
}


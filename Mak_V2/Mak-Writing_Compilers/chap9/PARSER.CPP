//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *   Parse a Pascal source program.                          *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog8-1/parser.cpp                             *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1995 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "common.h"
#include "parser.h"

//fig 8-8
//--------------------------------------------------------------
//  Parse       Parse the source file.
//
//  Return: ptr to the program id's symbol table node
//--------------------------------------------------------------

TSymtabNode *TParser::Parse(void)
{
    //--Extract the first token and parse the program.
    GetToken();
    TSymtabNode *pProgramId = ParseProgram();

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.",  currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();
    list.PutLine();

    return pProgramId;
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


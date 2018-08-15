//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *   Parse the source file for the Cross-Referencer I        *
//  *   utility program.                                        *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog4-1/parser.cpp                             *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"

//fig 4-8
//--------------------------------------------------------------
//  Parse       Parse the source file.  Extract each token, and
//              for each identifier, enter it into the symbol
//              table along with its line numbers.
//--------------------------------------------------------------

void TParser::Parse(void)
{
    //--Loop to extract and process tokens
    //--until the end of the program.
    do {
	GetToken();

	//--Shouldn't see an end of file.
	if (token == tcEndOfFile) {
	    Error(errUnexpectedEndOfFile);
	    break;
	}

	//--Enter each identifier into the symbol table
	//--if it isn't already in there.
	if (token == tcIdentifier) {
	    TSymtabNode *pNode = globalSymtab.Search(pToken->String());
	    if (!pNode)  pNode = globalSymtab.Enter (pToken->String());
	}
    } while (token != tcPeriod);

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.",  currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();
}
//endfig

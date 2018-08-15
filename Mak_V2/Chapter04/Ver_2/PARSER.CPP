//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *   Parse the source file for the Source File Cruncher      *
//  *   utility program.                                        *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog4-2/parser.cpp                             *
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

//fig 4-12
//--------------------------------------------------------------
//  Parse       Parse the source file and build the icode.
//--------------------------------------------------------------

void TParser::Parse(void)
{
    //--Output a placeholder for the size of the crunched program.
    pIcode->Put(0);

    //--Loop to extract and crunch tokens
    //--until the end of the program.
    do {
	GetToken();

	//--Shouldn't see an end of file.
	if (token == tcEndOfFile) {
	    Error(errUnexpectedEndOfFile);
	    break;
	}

	//--Enter each identifier, number, and string into the
	//--symbol table if it isn't already in there.
	//--Crunch the token.
	switch (token) {

	    case tcIdentifier:
	    case tcNumber:
	    case tcString: {
		TSymtabNode *pNode = globalSymtab.Search(pToken->String());
		if (!pNode)  pNode = globalSymtab.Enter (pToken->String());
		pIcode->Put(token, (short) pNode->NodeIndex());
		break;
	    }

	    default: {
		pIcode->Put(token);
		break;
	    }
	}
    } while (token != tcPeriod);

    //--Remember the icode size at this point.
    int programSize = pIcode->Size();

    //--Crunch the symbol table.
    pIcode->Put(globalSymtab.NodeCount());  // count of entries
    pIcode->Put(globalSymtab.Root());       // all the entries

    //--Now go back to the beginning of the intermediate code
    //--file and fill in the size of the crunched program.
    pIcode->GoTo(0);
    pIcode->Put(programSize);

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.",  currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();
}
//endfig

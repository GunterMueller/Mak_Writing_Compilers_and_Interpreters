//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *	Parse the source file for the Calculator utility        *
//  *	program.						*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog5-1/parser.cpp                             *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "parser.h"

//fig 5-10
//--------------------------------------------------------------
//  Parse       Parse the source file.
//--------------------------------------------------------------

void TParser::Parse(void)
{
    //--Extract the first token.
    GetToken();

    //--Loop to parse statements until the end of the program.
    do {
	//--Shouldn't see an end of file.
	if (token == tcEndOfFile) {
	    Error(errUnexpectedEndOfFile);
	    break;
	}

	//--Parse a statement.
	ParseStatement();

	//--If the current token is not a semicolon or the final
	//--period, we have a syntax error.  If the current token
	//--is an identifier (i.e., the start of the next statement),
	//--the error is a missing semicolon.  Otherwise, the error is
	//--an unexpected token, and so we skip tokens until we find a
	//--semicolon or a period, or until we reach the end of the
	//--source file.
	if ((token != tcSemicolon) && (token != tcPeriod)) {
	    if (token == tcIdentifier) Error(errMissingSemicolon);
	    else {
		Error(errUnexpectedToken);

		while ((token != tcSemicolon) &&
		       (token != tcPeriod) &&
		       (token != tcEndOfFile)) {
		    GetToken();
		}
	    }
	}

	//--Skip over any semicolons before parsing the next statement.
	while (token == tcSemicolon) GetToken();

    } while (token != tcPeriod);

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.",  currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();
}
//endfig


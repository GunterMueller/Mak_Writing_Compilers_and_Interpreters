//fig 3-11
//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *   Parse the source file for the Simple Tokenizer          *
//  *   utility program.                                        *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog3-1/parser.cpp                             *
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

//--------------------------------------------------------------
//  Parse       Parse the source file.  After listing each
//              source line, extract and list its tokens.
//--------------------------------------------------------------

void TParser::Parse(void)
{
    //--Loop to extract and print tokens
    //--until the end of the source file.
    do {
	GetToken();

	if (token != tcError) pToken->Print();
	else {
	    sprintf(list.text, "\t%-18s %-s", ">> *** ERROR ***",
			       pToken->String());
	    list.PutLine();
	    ++errorCount;
	}
    } while (token != tcEndOfFile);

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.",  currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();
}
//endfig


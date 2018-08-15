//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *	Parse the source file for the Pascal Source Compactor   *
//  *	utility program.					*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog3-3/parser.cpp                             *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"

//fig 3-26
//--------------------------------------------------------------
//  Parse       Parse the source file.  Output a compacted
//              version with unnecessary blanks and comments
//              removed.
//--------------------------------------------------------------

void TParser::Parse(void)
{
    int currIsDelimiter;         // true if current token is a
				 //   delimiter, else false
    int prevIsDelimiter = true;  // likewise for previous token

    //--Loop to extract and process tokens
    //--until the end of the program.
    do {
	GetToken();

	//--Shouldn't see an end of file.
	if (token == tcEndOfFile) {
	    Error(errUnexpectedEndOfFile);
	    break;
	}

	if (token != tcError) {
	    currIsDelimiter = pToken->IsDelimiter();

	    //--Append a blank only if both the previous and the
	    //--current tokens are not delimiters.  Then append the
	    //--token string to the output record.
	    if (!prevIsDelimiter && !currIsDelimiter) {
		pCompact->PutBlank();
	    }
	    pCompact->Put(pToken->String());

	    prevIsDelimiter = currIsDelimiter;
	}
	else Error(errUnrecognizable);
    } while (token != tcPeriod);

    //--Flush the last output record.
    pCompact->PutLine();
}
//endfig


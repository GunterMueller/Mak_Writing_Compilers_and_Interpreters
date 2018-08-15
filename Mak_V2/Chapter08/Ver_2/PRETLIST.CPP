//fig 8-25
//  *************************************************************
//  *                                                           *
//  *   P R E T T Y   P R I N T   L I S T I N G                 *
//  *                                                           *
//  *   Pretty-printing list buffer routines.                   *
//  *                                                           *
//  *   CLASSES: TPrettyListBuffer                              *
//  *                                                           *
//  *   FILE:    pretlist.cpp                                   *
//  *                                                           *
//  *   MODULE:  Buffer                                         *
//  *                                                           *
//  *   Routines to output a pretty printed source listing.     *
//  *                                                           *
//  *   Copyright (c) 1995 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <string.h>
#include "pretlist.h"

//--------------------------------------------------------------
//  Put         Append a token string to the output record if it
//              fits.  If not, print the current record and
//              append the string to the next record.  If at the
//              beginning of the record, first indent.
//
//      pString : ptr to the token string to append
//--------------------------------------------------------------

void TPrettyListBuffer::Put(const char *pString)
{
    int tokenLength = strlen(pString);

    //--Start a new output record if the current one is full.
    if (textLength + tokenLength >= maxPrettyTextLength - 1) PutLine();

    //--Indent if at the beginning of the output record.
    if ((textLength == 0) && (margin > 0)) {
	int m = margin < maxMargin ? margin : maxMargin;
	sprintf(text, "%*s", m, " ");
	pText      = text + m;
	textLength = m;
    }

    strcpy(pText, pString);
    pText      += tokenLength;
    textLength += tokenLength;
}

//--------------------------------------------------------------
//  PutLine     Print the output record and reset it
//              to the blank line.
//--------------------------------------------------------------

void TPrettyListBuffer::PutLine(void)
{
    if (textLength == 0) return;

    cout << text << endl;

    text[0]    = '\0';
    pText      = text;
    textLength = 0;
}
//endfig

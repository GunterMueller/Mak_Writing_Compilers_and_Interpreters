//fig 3-27
//  *************************************************************
//  *                                                           *
//  *   C O M P A C T   L I S T I N G				*
//  *                                                           *
//  *	Output a compact source listing.			*
//  *                                                           *
//  *	CLASSES: TCompactListBuffer				*
//  *                                                           *
//  *   FILE:    prog3-3/complist.cpp                           *
//  *                                                           *
//  *   MODULE:  Buffer                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include "complist.h"

//--------------------------------------------------------------
//  PutBlank        Append a blank to the output record,
//                  or print the record if it is full.
//--------------------------------------------------------------

void TCompactListBuffer::PutBlank(void)
{
    if (++textLength >= maxCompactTextLength - 1) PutLine();
    else                                          *pText++ = ' ';
}

//--------------------------------------------------------------
//  Put    	    Append a token string to the output record
//                  if it fits.  If not, print the current
//		    record and append the string to the next
//		    record.
//
//      pString : ptr to the token string to append
//--------------------------------------------------------------

void TCompactListBuffer::Put(const char *pString)
{
    int tokenLength = strlen(pString);

    if (textLength + tokenLength >= maxCompactTextLength - 1) {
	PutLine();
    }

    strcpy(pText, pString);
    pText      += tokenLength;
    textLength += tokenLength;
}

//--------------------------------------------------------------
//  PutLine 	    Print the output record.
//--------------------------------------------------------------

void TCompactListBuffer::PutLine(void)
{
    if (textLength > 0) {
	*pText = '\0';
	cout << text << endl;

	pText      = text;
	textLength = 0;
    }
}
//endfig

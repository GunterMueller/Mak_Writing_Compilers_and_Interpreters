//fig 3-8
//  *************************************************************
//  *                                                           *
//  *   T O K E N S                                             *
//  *                                                           *
//  *   Extract simple word, short integer, and period tokens   *
//  *   from the source file for the Simple Tokenizer utility   *
//  *   program.                                                *
//  *                                                           *
//  *   CLASSES: TToken, TWordToken, TNumberToken,              *
//  *            TStringToken, TSpecialToken, TEOFToken,        *
//  *            TErrorToken                                    *
//  *                                                           *
//  *   FILE:    prog3-1/token.cpp                              *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <string.h>
#include "token.h"

//              *****************
//              *               *
//              *  Word Tokens  *
//              *               *
//              *****************

//--------------------------------------------------------------
//  Get         Extract a word token from the source and down-
//              shift its characters.  Check if it's a reserved
//              word.
//
//      buffer : ref to text input buffer
//--------------------------------------------------------------

void TWordToken::Get(TTextInBuffer &buffer)
{
    char  ch = buffer.Char();  // char fetched from input
    char *ps = string;

    //--Extract the word.
    do {
	*ps++ = ch;
	ch = buffer.GetChar();
    } while (   (charCodeMap[ch] == ccLetter)
	     || (charCodeMap[ch] == ccDigit));

    *ps = '\0';
    strlwr(string);        // downshift its characters
    code = tcWord;
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TWordToken::Print(void) const
{
    sprintf(list.text, "\t%-18s %-s", ">> word:",
		       string);
    list.PutLine();
}

//              *******************
//              *                 *
//              *  Number Tokens  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  Get         Extract a number token from the source and set
//              its value.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TNumberToken::Get(TTextInBuffer &buffer)
{
    const int maxDigitCount = 4;
    
    char  ch = buffer.Char();      // char fetched from input
    char *ps = string;
    int   digitCount     = 0;
    int   countErrorFlag = false;  // true if too many digits,
				   //   else false

    //--Accumulate the value as long as the total allowable
    //--number of digits has not been exceeded.
    value.integer = 0;
    do {
	*ps++ = ch;

	//--Shift left and add.
	if (++digitCount <= maxDigitCount) {
	    value.integer = 10*value.integer + (ch - '0');
	}
	else countErrorFlag = true;  // too many digits

	ch = buffer.GetChar();
    } while (charCodeMap[ch] == ccDigit);

    *ps  = '\0';
    code = countErrorFlag ? tcError : tcNumber;
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TNumberToken::Print(void) const
{
    sprintf(list.text, "\t%-18s =%d", ">> number:", value.integer);
    list.PutLine();
}

//              ********************
//              *                  *
//              *  Special Tokens  *
//              *                  *
//              ********************

//--------------------------------------------------------------
//  Get         Extract the period special symbol token from the
//              source.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TSpecialToken::Get(TTextInBuffer &buffer)
{
    char  ch = buffer.Char();
    char *ps = string;

    *ps++ = ch;
    *ps = '\0';
    buffer.GetChar();

    code = (ch == '.') ? tcPeriod : tcError;
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TSpecialToken::Print(void) const
{
    sprintf(list.text, "\t%-18s %-s", ">> special:", string);
    list.PutLine();
}

//              *****************
//              *               *
//              *  Error Token  *
//              *               *
//              *****************

//--------------------------------------------------------------
//  Get         Extract an invalid character from the source.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TErrorToken::Get(TTextInBuffer &buffer)
{
    string[0] = buffer.Char();
    string[1] = '\0';
    buffer.GetChar();
}
//endfig

//fig 3-19
//  *************************************************************
//  *                                                           *
//  *   T O K E N S   (Strings and Specials)                    *
//  *                                                           *
//  *   Routines to extract string and special symbol tokens    *
//  *   from the source file.                                   *
//  *                                                           *
//  *   CLASSES: TStringToken, TSpecialToken, TErrorToken       *
//  *                                                           *
//  *   FILE:    prog3-2/tknstrsp.cpp                           *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "token.h"

//              *******************
//              *                 *
//              *  String Tokens  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  Get     Get a string token from the source.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TStringToken::Get(TTextInBuffer &buffer)
{
    char  ch;           // current character
    char *ps = string;  // ptr to char in string

    *ps++ = '\'';  // opening quote

    //--Get the string.
    ch = buffer.GetChar();  // first char after opening quote
    while (ch != eofChar) {
	if (ch == '\'') {     // look for another quote

	    //--Fetched a quote.  Now check for an adjacent quote,
	    //--since two consecutive quotes represent a single
	    //--quote in the string.
	    ch = buffer.GetChar();
	    if (ch != '\'') break;  // not another quote, so previous
				    //   quote ended the string
	}

	//--Replace the end of line character with a blank.
	else if (ch == '\0') ch = ' ';

	//--Append current char to string, then get the next char.
	*ps++ = ch;
	ch = buffer.GetChar();
    }

    if (ch == eofChar) Error(errUnexpectedEndOfFile);

    *ps++ = '\'';  // closing quote
    *ps   = '\0';
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TStringToken::Print(void) const
{
    sprintf(list.text, "\t%-18s %-s", ">> string:", string);
    list.PutLine();
}

//              ********************
//              *                  *
//              *  Special Tokens  *
//              *                  *
//              ********************

//--------------------------------------------------------------
//  Get         Extract a one- or two-character special symbol
//              token from the source.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TSpecialToken::Get(TTextInBuffer &buffer)
{
    char  ch = buffer.Char();
    char *ps = string;

    *ps++ = ch;

    switch (ch) {
	case '^':   code = tcUpArrow;    buffer.GetChar();  break;
	case '*':   code = tcStar;       buffer.GetChar();  break;
	case '(':   code = tcLParen;     buffer.GetChar();  break;
	case ')':   code = tcRParen;     buffer.GetChar();  break;
	case '-':   code = tcMinus;      buffer.GetChar();  break;
	case '+':   code = tcPlus;       buffer.GetChar();  break;
	case '=':   code = tcEqual;      buffer.GetChar();  break;
	case '[':   code = tcLBracket;   buffer.GetChar();  break;
	case ']':   code = tcRBracket;   buffer.GetChar();  break;
	case ';':   code = tcSemicolon;  buffer.GetChar();  break;
	case ',':   code = tcComma;      buffer.GetChar();  break;
	case '/':   code = tcSlash;      buffer.GetChar();  break;

	case ':':   ch = buffer.GetChar();     // : or :=
		    if (ch == '=') {
			*ps++ = '=';
			code  = tcColonEqual;
			buffer.GetChar();
		    }
		    else code = tcColon;
		    break;

	case '<':   ch = buffer.GetChar();     // < or <= or <>
		    if (ch == '=') {
			*ps++ = '=';
			code  = tcLe;
			buffer.GetChar();
		    }
		    else if (ch == '>') {
			*ps++ = '>';
			code  = tcNe;
			buffer.GetChar();
		    }
		    else code = tcLt;
		    break;

	case '>':   ch = buffer.GetChar();     // > or >=
		    if (ch == '=') {
			*ps++ = '=';
			code  = tcGe;
			buffer.GetChar();
		    }
		    else code = tcGt;
		    break;

	case '.':   ch = buffer.GetChar();     // . or ..
		    if (ch == '.') {
			*ps++ = '.';
			code  = tcDotDot;
			buffer.GetChar();
		    }
		    else code = tcPeriod;
		    break;

	default:    code = tcError;                  // error
		    buffer.GetChar();
		    Error(errUnrecognizable);
		    break;
    }

    *ps = '\0';
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
    Error(errUnrecognizable);
}
//endfig

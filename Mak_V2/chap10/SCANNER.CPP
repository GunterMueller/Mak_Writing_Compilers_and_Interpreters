//  *************************************************************
//  *                                                           *
//  *   S C A N N E R                                           *
//  *                                                           *
//  *   Scan the text input file.                               *
//  *                                                           *
//  *   CLASSES: TTextScanner                                   *
//  *                                                           *
//  *   FILE:    prog3-2/scanner.cpp                            *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "scanner.h"

TCharCode charCodeMap[128];    // maps a character to its code

//--------------------------------------------------------------
//  Constructor     Construct a scanner by constructing the
//                  text input file buffer and initializing the
//                  character code map.
//
//      pBuffer : ptr to text input buffer to scan
//--------------------------------------------------------------

TTextScanner::TTextScanner(TTextInBuffer *pBuffer)
    : pTextInBuffer(pBuffer)
{
    int i;

    //--Initialize the character code map.
    for (i =   0; i <  128; ++i) charCodeMap[i] = ccError;
    for (i = 'a'; i <= 'z'; ++i) charCodeMap[i] = ccLetter;
    for (i = 'A'; i <= 'Z'; ++i) charCodeMap[i] = ccLetter;
    for (i = '0'; i <= '9'; ++i) charCodeMap[i] = ccDigit;
    charCodeMap['+' ] = charCodeMap['-' ] = ccSpecial;
    charCodeMap['*' ] = charCodeMap['/' ] = ccSpecial;
    charCodeMap['=' ] = charCodeMap['^' ] = ccSpecial;
    charCodeMap['.' ] = charCodeMap[',' ] = ccSpecial;
    charCodeMap['<' ] = charCodeMap['>' ] = ccSpecial;
    charCodeMap['(' ] = charCodeMap[')' ] = ccSpecial;
    charCodeMap['[' ] = charCodeMap[']' ] = ccSpecial;
    charCodeMap['{' ] = charCodeMap['}' ] = ccSpecial;
    charCodeMap[':' ] = charCodeMap[';' ] = ccSpecial;
    charCodeMap[' ' ] = charCodeMap['\t'] = ccWhiteSpace;
    charCodeMap['\n'] = charCodeMap['\0'] = ccWhiteSpace;
    charCodeMap['\'']    = ccQuote;
    charCodeMap[eofChar] = ccEndOfFile;
}

//fig 3-16
//--------------------------------------------------------------
//  SkipWhiteSpace      Repeatedly fetch characters from the
//                      text input as long as they're
//                      whitespace. Each comment is a whitespace
//                      character.
//--------------------------------------------------------------

void TTextScanner::SkipWhiteSpace(void)
{
    char ch = pTextInBuffer->Char();

    do {
	if (charCodeMap[ch] == ccWhiteSpace) {

	    //--Saw a whitespace character:  fetch the next character.
	    ch = pTextInBuffer->GetChar();
	}
	else if (ch == '{') {

	    //--Skip over a comment, then fetch the next character.
	    do {
		ch = pTextInBuffer->GetChar();
	    } while ((ch != '}') && (ch != eofChar));
	    if (ch != eofChar) ch = pTextInBuffer->GetChar();
	    else               Error(errUnexpectedEndOfFile);
	}
    } while ((charCodeMap[ch] == ccWhiteSpace) || (ch == '{'));
}
//endfig

//--------------------------------------------------------------
//  Get         Extract the next token from the text input,
//              based on the current character.
//
//  Return: pointer to the extracted token
//--------------------------------------------------------------

TToken *TTextScanner::Get(void)
{
    TToken *pToken;  // ptr to token to return

    SkipWhiteSpace();

    //--Determine the token class, based on the current character.
    switch (charCodeMap[pTextInBuffer->Char()]) {
	case ccLetter:    pToken = &wordToken;    break;
	case ccDigit:     pToken = &numberToken;  break;
	case ccQuote:     pToken = &stringToken;  break;
	case ccSpecial:   pToken = &specialToken; break;
	case ccEndOfFile: pToken = &eofToken;     break;
	default:          pToken = &errorToken;   break;
    }

    //--Extract a token of that class, and return a pointer to it.
    pToken->Get(*pTextInBuffer);
    return pToken;
}


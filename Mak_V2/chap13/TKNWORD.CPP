//fig 3-17
//  *************************************************************
//  *                                                           *
//  *   T O K E N S   (Words)                                   *
//  *                                                           *
//  *   Extract word tokens from the source file.               *
//  *                                                           *
//  *   CLASSES: TWordToken                                     *
//  *                                                           *
//  *   FILE:    prog3-2/tknword.cpp                            *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include <stdio.h>
#include "token.h"

//              *************************
//              *                       *
//              *  Reserved Word Table  *
//              *                       *
//              *************************

const int minResWordLen = 2;    // min and max reserved
const int maxResWordLen = 9;    //   word lengths

//--------------------------------------------------------------
//  Reserved word lists (according to word length)
//--------------------------------------------------------------

struct TResWord {
    char       *pString;  // ptr to word string
    TTokenCode  code;     // word code
};

static TResWord rw2[] = {
    {"do", tcDO}, {"if", tcIF}, {"in", tcIN}, {"of", tcOF},
    {"or", tcOR}, {"to", tcTO}, {NULL},
};

static TResWord rw3[] = {
    {"and", tcAND}, {"div", tcDIV}, {"end", tcEND}, {"for", tcFOR},
    {"mod", tcMOD}, {"nil", tcNIL}, {"not", tcNOT}, {"set", tcSET},
    {"var", tcVAR}, {NULL},
};

static TResWord rw4[] = {
    {"case", tcCASE}, {"else", tcELSE}, {"file", tcFILE},
    {"goto", tcGOTO}, {"then", tcTHEN}, {"type", tcTYPE},
    {"with", tcWITH}, {NULL},
};

static TResWord rw5[] = {
    {"array", tcARRAY}, {"begin", tcBEGIN}, {"const", tcCONST},
    {"label", tcLABEL}, {"until", tcUNTIL}, {"while", tcWHILE},
    {NULL},
};

static TResWord rw6[] = {
    {"downto", tcDOWNTO}, {"packed", tcPACKED}, {"record", tcRECORD},
    {"repeat", tcREPEAT}, {NULL},
};

static TResWord rw7[] = {
    {"program", tcPROGRAM}, {NULL},
};

static TResWord rw8[] = {
    {"function", tcFUNCTION}, {NULL},
};

static TResWord rw9[] = {
    {"procedure", tcPROCEDURE}, {NULL},
};

//--------------------------------------------------------------
//  The reserved word table
//--------------------------------------------------------------

static TResWord *rwTable[] = {
    NULL, NULL, rw2, rw3, rw4, rw5, rw6, rw7, rw8, rw9,
};

//              *****************
//              *               *
//              *  Word Tokens  *
//              *               *
//              *****************

//--------------------------------------------------------------
//  Get     Extract a word token from the source and downshift
//          its characters.  Check if it's a reserved word.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TWordToken::Get(TTextInBuffer &buffer)
{
    extern TCharCode charCodeMap[];

    char  ch = buffer.Char();  // char fetched from input
    char *ps = string;

    //--Get the word.
    do {
	*ps++ = ch;
	ch = buffer.GetChar();
    } while (   (charCodeMap[ch] == ccLetter)
	     || (charCodeMap[ch] == ccDigit));

    *ps = '\0';
    strlwr(string);        // downshift its characters

    CheckForReservedWord();
}

//--------------------------------------------------------------
//  CheckForReservedWord    Is the word token a reserved word?
//                          If yes, set the its token code to
//                          the appropriate code.  If not, set
//                          the token code to tcIdentifier.
//--------------------------------------------------------------

void TWordToken::CheckForReservedWord(void)
{
    int       len = strlen(string);
    TResWord *prw;        // ptr to elmt of reserved word table

    code = tcIdentifier;  // first assume it's an identifier

    //--Is it the right length?
    if ((len >= minResWordLen) && (len <= maxResWordLen)) {

	//--Yes.  Use the word length to pick the appropriate list
	//--from the reserved word table and check to see if the word
	//--is in there.
	for (prw = rwTable[len]; prw->pString; ++prw) {
	    if (strcmp(string, prw->pString) == 0) {
		code = prw->code;  // yes: set reserved word token code
		break;
	    }
	}
    }
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TWordToken::Print(void) const
{
    if (code == tcIdentifier) {
	sprintf(list.text, "\t%-18s %-s", ">> identifier:", string);
    }
    else {
	sprintf(list.text, "\t%-18s %-s", ">> reserved word:", string);
    }

    list.PutLine();
}
//endfig


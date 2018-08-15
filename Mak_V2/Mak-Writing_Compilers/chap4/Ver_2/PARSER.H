//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Header)                                  *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog4-2/parser.h                               *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef parser_h
#define parser_h

#include "buffer.h"
#include "symtab.h"
#include "token.h"
#include "scanner.h"
#include "icode.h"

extern TSymtab  globalSymtab;
extern TIcode  *pIcode;

//--------------------------------------------------------------
//  TParser     Parser class.
//--------------------------------------------------------------

class TParser {
    TTextScanner *const pScanner;  // ptr to the scanner
    TToken       *pToken;          // ptr to the current token
    TTokenCode    token;           // code of current token

    void GetToken(void)
    {
	pToken = pScanner->Get();
	token  = pToken->Code();
    }

public:
    TParser(TTextInBuffer *pBuffer)
	: pScanner(new TTextScanner(pBuffer)) {}

   ~TParser(void) { delete pScanner; }

    void Parse(void);
};

#endif

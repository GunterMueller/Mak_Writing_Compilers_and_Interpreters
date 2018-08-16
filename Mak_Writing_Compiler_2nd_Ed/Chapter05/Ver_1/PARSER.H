//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Header)                                  *
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog5-1/parser.h                               *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef parser_h
#define parser_h

//fig 5-8
#include "misc.h"
#include "buffer.h"
#include "error.h"
#include "symtab.h"
#include "token.h"
#include "scanner.h"
#include "exec.h"

extern TSymtab globalSymtab;

//--------------------------------------------------------------
//  TParser             Parser class.
//--------------------------------------------------------------

class TParser {
    TTextScanner  *const pScanner;  // ptr to the scanner
    TToken        *pToken;          // ptr to the current token
    TTokenCode     token;           // code of current token
    TRuntimeStack  runStack;        // runtime stack

    //--Statements.
    void ParseStatement (void);
    void ParseAssignment(void);

    //--Expressions.
    void ParseExpression      (void);
    void ParseSimpleExpression(void);
    void ParseTerm            (void);
    void ParseFactor          (void);

    void GetToken(void)
    {
	pToken = pScanner->Get();
	token  = pToken->Code();
    }

    TSymtabNode *SearchAll(const char *pString) const
    {
	return globalSymtab.Search(pString);
    }

    TSymtabNode *EnterLocal(const char *pString) const
    {
	return globalSymtab.Enter(pString);
    }

public:
    TParser(TTextInBuffer *pBuffer)
	: pScanner(new TTextScanner(pBuffer)) {}

   ~TParser(void) { delete pScanner; }

    void Parse(void);
};
//endfig

#endif

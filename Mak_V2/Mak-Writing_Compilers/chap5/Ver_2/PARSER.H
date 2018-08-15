//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Header)                                  *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog5-2/parser.h                               *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef parser_h
#define parser_h

//fig 5-20
#include "misc.h"
#include "buffer.h"
#include "error.h"
#include "symtab.h"
#include "token.h"
#include "scanner.h"
#include "icode.h"

extern TSymtab globalSymtab;
extern TIcode  icode;

//--------------------------------------------------------------
//  TParser             Parser class.
//--------------------------------------------------------------

class TParser {
    TTextScanner *const pScanner;  // ptr to the scanner
    TToken       *pToken;          // ptr to the current token
    TTokenCode    token;           // code of current token

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

    void GetTokenAppend(void)
    {
	GetToken();
	icode.Put(token);  // append token code to icode
    }

    void InsertLineMarker(void) { icode.InsertLineMarker(); }

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
	: pScanner(new TTextScanner(pBuffer))
    {
	//--Enter the special "input" and "output" variable identifiers
	//--into the symbol table.
	EnterLocal("input");
	EnterLocal("output");
    }

   ~TParser(void) { delete pScanner; }

    void Parse(void);
};
//endfig

#endif

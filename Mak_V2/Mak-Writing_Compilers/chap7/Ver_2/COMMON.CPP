//  *************************************************************
//  *                                                           *
//  *   C O M M O N                                             *
//  *                                                           *
//  *   FILE:    prog7-1/common.cpp                             *
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Data and routines common to the front and back ends.    *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "common.h"

int currentNestingLevel = 0;
int currentLineNumber   = 0;

TSymtab   globalSymtab;        // the global symbol table
int       cntSymtabs  = 0;     // symbol table counter
TSymtab  *pSymtabList = NULL;  // ptr to head of symtab list
TSymtab **vpSymtabs;           // ptr to vector of symtab ptrs

TIcode icode;                  // the intermediate code

//--------------------------------------------------------------
//  Token lists
//--------------------------------------------------------------

//fig 7-11
//--Tokens that can start a declaration.
extern const TTokenCode tlDeclarationStart[] = {
   tcCONST, tcTYPE, tcVAR, tcPROCEDURE, tcFUNCTION, tcDummy
};

//--Tokens that can follow a declaration.
extern const TTokenCode tlDeclarationFollow[] = {
    tcSemicolon, tcIdentifier, tcDummy
};

//--Tokens that can start an identifier or field.
extern const TTokenCode tlIdentifierStart[] = {
    tcIdentifier, tcDummy
};

//--Tokens that can follow an identifier or field.
extern const TTokenCode tlIdentifierFollow[] = {
    tcComma, tcIdentifier, tcColon, tcSemicolon, tcDummy
};

//--Tokens that can follow an identifier or field sublist.
extern const TTokenCode tlSublistFollow[] = {
    tcColon, tcDummy
};

//--Tokens that can follow a field declaration.
extern const TTokenCode tlFieldDeclFollow[] = {
    tcSemicolon, tcIdentifier, tcEND, tcDummy
};

//--Tokens that can start an enumeration constant.
extern const TTokenCode tlEnumConstStart[] = {
    tcIdentifier, tcDummy
};

//--Tokens that can follow an enumeration constant.
extern const TTokenCode tlEnumConstFollow[] = {
    tcComma, tcIdentifier, tcRParen, tcSemicolon, tcDummy
};

//--Tokens that can follow a subrange limit.
extern const TTokenCode tlSubrangeLimitFollow[] = {
    tcDotDot, tcIdentifier, tcPlus, tcMinus, tcString,
    tcRBracket, tcComma, tcSemicolon, tcOF, tcDummy
};

//--Tokens that can start an index type.
extern const TTokenCode tlIndexStart[] = {
    tcIdentifier, tcNumber, tcString, tcLParen, tcPlus, tcMinus,
    tcDummy
};

//--Tokens that can follow an index type.
extern const TTokenCode tlIndexFollow[] = {
    tcComma, tcRBracket, tcOF, tcSemicolon, tcDummy
};

//--Tokens that can follow the index type list.
extern const TTokenCode tlIndexListFollow[] = {
    tcOF, tcIdentifier, tcLParen, tcARRAY, tcRECORD,
    tcPlus, tcMinus, tcNumber, tcString, tcSemicolon, tcDummy
};

//--Tokens that can start a subscript or field.
extern const TTokenCode tlSubscriptOrFieldStart[] = {
    tcLBracket, tcPeriod, tcDummy
};
//endfig

//--Tokens that can start a statement.
extern const TTokenCode tlStatementStart[] = {
    tcBEGIN, tcCASE, tcFOR, tcIF, tcREPEAT, tcWHILE, tcIdentifier,
    tcDummy
};

//--Tokens that can follow a statement.
extern const TTokenCode tlStatementFollow[] = {
    tcSemicolon, tcPeriod, tcEND, tcELSE, tcUNTIL, tcDummy
};

//--Tokens that can start a CASE label.
extern const TTokenCode tlCaseLabelStart[] = {
    tcIdentifier, tcNumber, tcPlus, tcMinus, tcString, tcDummy
};

//--Tokens that can start an expression.
extern const TTokenCode tlExpressionStart[] = {
    tcPlus, tcMinus, tcIdentifier, tcNumber, tcString,
    tcNOT, tcLParen, tcDummy
};

//--Tokens that can follow an expression.
extern const TTokenCode tlExpressionFollow[] = {
    tcComma, tcRParen, tcRBracket, tcColon, tcTHEN, tcTO, tcDOWNTO,
    tcDO, tcOF, tcDummy
};

//--Relational operators.
extern const TTokenCode tlRelOps[] = {
    tcEqual, tcNe, tcLt, tcGt, tcLe, tcGe, tcDummy
};

//--Unary + and - operators.
extern const TTokenCode tlUnaryOps[] = {
    tcPlus, tcMinus, tcDummy
};

//--Additive operators.
extern const TTokenCode tlAddOps[] = {
    tcPlus, tcMinus, tcOR, tcDummy
};

//--Multiplicative operators.
extern const TTokenCode tlMulOps[] = {
    tcStar, tcSlash, tcDIV, tcMOD, tcAND, tcDummy
};

//--Tokens that can end a program.
extern const TTokenCode tlProgramEnd[] = {
    tcPeriod, tcDummy
};

//--Individual tokens.
extern const TTokenCode tlColonEqual[] = {tcColonEqual,   tcDummy};
extern const TTokenCode tlDO[]         = {tcDO,           tcDummy};
extern const TTokenCode tlTHEN[]       = {tcTHEN,         tcDummy};
extern const TTokenCode tlTODOWNTO[]   = {tcTO, tcDOWNTO, tcDummy};
extern const TTokenCode tlOF[]         = {tcOF,           tcDummy};
extern const TTokenCode tlColon[]      = {tcColon,        tcDummy};
extern const TTokenCode tlEND[]        = {tcEND,          tcDummy};

//--------------------------------------------------------------
//  TokenIn     Check if a token code is in the token list.
//
//      tc    : token code
//      pList : ptr to tcDummy-terminated token list
//
//  Return:  true if in list, false if not or empty list
//--------------------------------------------------------------

int TokenIn(TTokenCode tc, const TTokenCode *pList)
{
    const TTokenCode *pCode;   // ptr to token code in list

    if (!pList) return false;  // empty list

    for (pCode = pList; *pCode; ++pCode) {
	if (*pCode == tc) return true;  // in list
    }

    return false;  // not in list
}


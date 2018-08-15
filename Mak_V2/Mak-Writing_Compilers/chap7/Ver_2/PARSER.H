//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Header)                                  *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog7-2/parser.h                               *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef parser_h
#define parser_h

#include "misc.h"
#include "buffer.h"
#include "error.h"
#include "symtab.h"
#include "types.h"
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

    //--Declarations.
    void ParseDeclarations       (TSymtabNode *pRoutineId);
    void ParseConstantDefinitions(TSymtabNode *pRoutineId);
    void ParseConstant           (TSymtabNode *pConstId);
    void ParseIdentifierConstant (TSymtabNode *pId1, TTokenCode sign);

    void   ParseTypeDefinitions(TSymtabNode *pRoutineId);
    TType *ParseTypeSpec       (void);

    TType *ParseIdentifierType (const TSymtabNode *pId2);
    TType *ParseEnumerationType(void);

    TType *ParseSubrangeType (TSymtabNode *pMinId);
    TType *ParseSubrangeLimit(TSymtabNode *pLimitId, int &limit);

    TType *ParseArrayType (void);
    void   ParseIndexType (TType *pArrayType);
    int    ArraySize      (TType *pArrayType);
    TType *ParseRecordType(void);

    void ParseVariableDeclarations(TSymtabNode *pRoutineId);
    void ParseFieldDeclarations   (TType       *pRecordType,
				   int offset);
    void ParseVarOrFieldDecls     (TSymtabNode *pRoutineId,
				   TType       *pRecordType,
				   int offset);
    TSymtabNode *ParseIdSublist   (const TSymtabNode *pRoutineId,
				   const TType       *pRecordType,
				   TSymtabNode *&pLastId);

    //--Statements.
    void ParseStatement    (void);
    void ParseStatementList(TTokenCode terminator);
    void ParseAssignment   (void);
    void ParseREPEAT       (void);
    void ParseWHILE        (void);
    void ParseIF           (void);
    void ParseFOR          (void);
    void ParseCASE         (void);
    void ParseCaseBranch   (const TType *pExprType);
    void ParseCaseLabel    (const TType *pExprType);
    void ParseCompound     (void);

    //--Expressions.
    TType *ParseExpression      (void);
    TType *ParseSimpleExpression(void);
    TType *ParseTerm            (void);
    TType *ParseFactor          (void);
    TType *ParseVariable  (const TSymtabNode *pId);
    TType *ParseSubscripts(const TType *pType);
    TType *ParseField     (const TType *pType);

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

    void CondGetToken(TTokenCode tc, TErrorCode ec)
    {
	//--Get another token only if the current one matches tc.
	if (tc == token) GetToken();
	else             Error(ec);  // error if no match
    }

    void CondGetTokenAppend(TTokenCode tc, TErrorCode ec)
    {
	//--Get another token only if the current one matches tc.
	if (tc == token) GetTokenAppend();
	else             Error(ec);  // error if no match
    }

    void InsertLineMarker(void) { icode.InsertLineMarker(); }

    TSymtabNode *SearchAll(const char *pString) const
    {
	return globalSymtab.Search(pString);
    }

    TSymtabNode *EnterLocal(const char *pString,
			    TDefnCode dc = dcUndefined) const
    {
	return globalSymtab.Enter(pString, dc);
    }

    TSymtabNode *EnterNewLocal(const char *pString,
			       TDefnCode dc = dcUndefined) const
    {
	return globalSymtab.EnterNew(pString, dc);
    }

    TSymtabNode *Find(const char *pString) const
    {
	TSymtabNode *pNode = SearchAll(pString);  // look for string

	if (!pNode) {
	    Error(errUndefinedIdentifier);        // error if not found
	    pNode = globalSymtab.Enter(pString);  // but enter it anyway
	}

	return pNode;
    }

    void CopyQuotedString(char *pString,
			  const char *pQuotedString) const
    {
	int length = strlen(pQuotedString) - 2;  // don't count quotes
	strncpy(pString, &pQuotedString[1], length);
	pString[length] = '\0';
    }

    void Resync(const TTokenCode *pList1,
		const TTokenCode *pList2 = NULL,
		const TTokenCode *pList3 = NULL);

public:
    TParser(TTextInBuffer *pBuffer)
	: pScanner(new TTextScanner(pBuffer))
    {
	InitializePredefinedTypes(&globalSymtab);
    }

   ~TParser(void)
   {
	delete pScanner;
	RemovePredefinedTypes();
   }

    void Parse(void);
};

#endif


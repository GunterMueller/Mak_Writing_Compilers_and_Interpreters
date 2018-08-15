//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Header)                                  *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog13-1/parser.h                              *
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

//--------------------------------------------------------------
//  TCaseItem           CASE item class.
//--------------------------------------------------------------

class TCaseItem {

public:
    TCaseItem *next;
    int labelValue;
    int atBranchStmt;

    TCaseItem(TCaseItem *&pListHead)
    {
	next         = pListHead;  // insert at head of list
	pListHead    = this;
	atBranchStmt = 0;
    }
};

//--------------------------------------------------------------
//  TParser             Parser class.
//--------------------------------------------------------------

class TParser {
    TTextScanner *const pScanner;  // ptr to the scanner
    TToken       *pToken;          // ptr to the current token
    TTokenCode    token;           // code of current token
    TSymtabStack  symtabStack;     // symbol table stack
    TIcode        icode;           // icode buffer

    //--Routines
    TSymtabNode *ParseProgram               (void);
    TSymtabNode *ParseProgramHeader         (void);
    void         ParseSubroutineDeclarations(TSymtabNode *pRoutineId);
    TSymtabNode *ParseSubroutine            (void);
    TSymtabNode *ParseProcedureHeader       (void);
    TSymtabNode *ParseFunctionHeader        (void);
    void         ParseBlock                 (TSymtabNode *pRoutineId);

    TSymtabNode *ParseFormalParmList(int &count, int &totalSize);

    TType       *ParseSubroutineCall  (const TSymtabNode *pRoutineId,
				       int parmCheckFlag);
    TType *ParseDeclaredSubroutineCall(const TSymtabNode *pRoutineId,
				       int parmCheckFlag);
    TType *ParseStandardSubroutineCall(const TSymtabNode *pRoutineId);
    void   ParseActualParmList        (const TSymtabNode *pRoutineId,
				       int parmCheckFlag);
    void   ParseActualParm            (const TSymtabNode *pFormalId,
				       int parmCheckFlag);
    void   ReverseNodeList	      (TSymtabNode *&head);

    //--Standard subroutines
    TType *ParseReadReadlnCall  (const TSymtabNode *pRoutineId);
    TType *ParseWriteWritelnCall(const TSymtabNode *pRoutineId);
    TType *ParseEofEolnCall              (void);
    TType *ParseAbsSqrCall               (void);
    TType *ParseArctanCosExpLnSinSqrtCall(void);
    TType *ParsePredSuccCall             (void);
    TType *ParseChrCall                  (void);
    TType *ParseOddCall                  (void);
    TType *ParseOrdCall                  (void);
    TType *ParseRoundTruncCall           (void);
    void   SkipExtraParms                (void);

    //--Declarations
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

    //--Statements
    void ParseStatement    (void);
    void ParseStatementList(TTokenCode terminator);
    void ParseAssignment   (const TSymtabNode *pTargetId);
    void ParseREPEAT       (void);
    void ParseWHILE        (void);
    void ParseIF           (void);
    void ParseFOR          (void);
    void ParseCASE         (void);
    void ParseCaseBranch   (const TType *pExprType,
			    TCaseItem *&pCaseItemList);
    void ParseCaseLabel    (const TType *pExprType,
			    TCaseItem *&pCaseItemList);
    void ParseCompound     (void);

    //--Expressions
    TType *ParseExpression      (void);
    TType *ParseSimpleExpression(void);
    TType *ParseTerm            (void);
    TType *ParseFactor          (void);
    TType *ParseVariable        (const TSymtabNode *pId);
    TType *ParseSubscripts      (const TType *pType);
    TType *ParseField           (const TType *pType);

    void GetToken(void)
    {
	pToken = pScanner->Get();
	token  = pToken->Code();
    }

    void GetTokenAppend(void)
    {
	GetToken();
	icode.Put(token);  // append token code to icode buffer
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

    void InsertLineMarker (void) { icode.InsertLineMarker(); }

    int PutLocationMarker(void)
    {
	return icode.PutLocationMarker();
    }

    void FixupLocationMarker(int location)
    {
	icode.FixupLocationMarker(location);
    }

    void PutCaseItem(int value, int location)
    {
	icode.PutCaseItem(value, location);
    }

    TSymtabNode *SearchLocal(const char *pString)
    {
	return symtabStack.SearchLocal(pString);
    }

    TSymtabNode *SearchAll(const char *pString) const
    {
	return symtabStack.SearchAll(pString);
    }

    TSymtabNode *EnterLocal(const char *pString,
			    TDefnCode dc = dcUndefined)
    {
	return symtabStack.EnterLocal(pString, dc);
    }

    TSymtabNode *EnterNewLocal(const char *pString,
			       TDefnCode dc = dcUndefined)
    {
	return symtabStack.EnterNewLocal(pString, dc);
    }

    TSymtabNode *Find(const char *pString) const
    {
	return symtabStack.Find(pString);
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
	: pScanner(new TTextScanner(pBuffer)) {}

   ~TParser(void) { delete pScanner; }

    TToken *GetCommandToken(void)
    {
	GetToken();
	return pToken;
    }

    void ParseCommandExpression        (TIcode     *&pCmdIcode,
					TTokenCode  &cmdToken);
    TSymtabNode *ParseCommandAssignment(TIcode     *&pCmdIcode,
					TTokenCode  &cmdToken);

    void DebugSetCurrentSymtab(TSymtab *pSymtab)
    {
	symtabStack.SetCurrentSymtab(pSymtab);
    }

    TSymtabNode *DebugSearchAll(const char *pString) const
    {
	return SearchAll(pString);
    }

    TSymtabNode *Parse(void);
};

#endif

//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Statements)                              *
//  *                                                           *
//  *   Parse statements.                                       *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog10-1/parsstmt.cpp                          *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "parser.h"

//--------------------------------------------------------------
//  ParseStatement          Parse a statement.
//--------------------------------------------------------------

void TParser::ParseStatement(void)
{
    InsertLineMarker();

    //--Call the appropriate parsing function based on
    //--the statement's first token.
    switch (token) {

	case tcIdentifier: {

	    //--Search for the identifier and enter it if
	    //--necessary.  Append the symbol table node handle
	    //--to the icode.
	    TSymtabNode *pNode = Find(pToken->String());
	    icode.Put(pNode);

	    //--Based on how the identifier is defined,
	    //--parse an assignment statement or a procedure call.
	    if (pNode->defn.how == dcUndefined) {
		pNode->defn.how = dcVariable;
		SetType(pNode->pType, pDummyType);
		ParseAssignment(pNode);
	    }
	    else if (pNode->defn.how == dcProcedure) {
		ParseSubroutineCall(pNode, true);
	    }
	    else ParseAssignment(pNode);

	    break;
	}

	case tcREPEAT:      ParseREPEAT();      break;
	case tcWHILE:       ParseWHILE();       break;
	case tcIF:          ParseIF();          break;
	case tcFOR:         ParseFOR();         break;
	case tcCASE:        ParseCASE();        break;
	case tcBEGIN:       ParseCompound();    break;
    }

    //--Resynchronize at a proper statement ending.
    if (token != tcEndOfFile) {
	Resync(tlStatementFollow, tlStatementStart);
    }
}

//--------------------------------------------------------------
//  ParseStatementList      Parse a statement list until the
//                          terminator token.
//
//      terminator : the token that terminates the list
//--------------------------------------------------------------

void TParser::ParseStatementList(TTokenCode terminator)
{
    //--Loop to parse statements and to check for and skip semicolons.
    do {
	ParseStatement();

	if (TokenIn(token, tlStatementStart)) {
	    Error(errMissingSemicolon);
	}
	else while (token == tcSemicolon) GetTokenAppend();
    } while ((token != terminator) && (token != tcEndOfFile));
}

//--------------------------------------------------------------
//  ParseAssignment         Parse an assignment statement:
//
//                              <id> := <expr>
//
//      pTargetId : ptr to target id's symbol table node
//--------------------------------------------------------------

void TParser::ParseAssignment(const TSymtabNode *pTargetId)
{
    TType *pTargetType = ParseVariable(pTargetId);

    //-- :=
    Resync(tlColonEqual, tlExpressionStart);
    CondGetTokenAppend(tcColonEqual, errMissingColonEqual);

    //--<expr>
    TType *pExprType = ParseExpression();

    //--Check for assignment compatibility.
    CheckAssignmentTypeCompatible(pTargetType, pExprType,
				  errIncompatibleAssignment);
}

//--------------------------------------------------------------
//  ParseREPEAT     Parse a REPEAT statement:
//
//                      REPEAT <stmt-list> UNTIL <expr>
//--------------------------------------------------------------

void TParser::ParseREPEAT(void)
{
    GetTokenAppend();

    //--<stmt-list>
    ParseStatementList(tcUNTIL);

    //--UNTIL
    CondGetTokenAppend(tcUNTIL, errMissingUNTIL);

    //--<expr> : must be boolean
    InsertLineMarker();
    CheckBoolean(ParseExpression());
}

//fig 10-8
//--------------------------------------------------------------
//  ParseWHILE      Parse a WHILE statement.:
//
//                      WHILE <expr> DO <stmt>
//--------------------------------------------------------------

void TParser::ParseWHILE(void)
{
    //--Append a placeholder location marker for the token that
    //--follows the WHILE statement.  Remember the location of this
    //--placeholder so it can be fixed up below.
    int atFollowLocationMarker = PutLocationMarker();

    //--<expr> : must be boolean
    GetTokenAppend();
    CheckBoolean(ParseExpression());

    //--DO
    Resync(tlDO, tlStatementStart);
    CondGetTokenAppend(tcDO, errMissingDO);

    //--<stmt>
    ParseStatement();
    FixupLocationMarker(atFollowLocationMarker);
}

//--------------------------------------------------------------
//  ParseIF         Parse an IF statement:
//
//                      IF <expr> THEN <stmt-1>
//
//                  or:
//
//                      IF <expr> THEN <stmt-1> ELSE <stmt-2>
//--------------------------------------------------------------

void TParser::ParseIF(void)
{
    //--Append a placeholder location marker for where to go to if
    //--<expr> is false.  Remember the location of this placeholder
    //--so it can be fixed up below.
    int atFalseLocationMarker = PutLocationMarker();

    //--<expr> : must be boolean
    GetTokenAppend();
    CheckBoolean(ParseExpression());

    //--THEN
    Resync(tlTHEN, tlStatementStart);
    CondGetTokenAppend(tcTHEN, errMissingTHEN);

    //--<stmt-1>
    ParseStatement();
    FixupLocationMarker(atFalseLocationMarker);

    if (token == tcELSE) {

	//--Append a placeholder location marker for the token that
	//--follows the IF statement.  Remember the location of this
	//--placeholder so it can be fixed up below.
	int atFollowLocationMarker = PutLocationMarker();

	//--ELSE <stmt-2>
	GetTokenAppend();
	ParseStatement();
	FixupLocationMarker(atFollowLocationMarker);
    }
}

//--------------------------------------------------------------
//  ParseFOR        Parse a FOR statement:
//
//                      FOR <id> := <expr-1> TO|DOWNTO <expr-2>
//                          DO <stmt>
//--------------------------------------------------------------

void TParser::ParseFOR(void)
{
    TType *pControlType;  // ptr to the control id's type object

    //--Append a placeholder for the location of the token that
    //--follows the FOR statement.  Remember the location of this
    //--placeholder.
    int atFollowLocationMarker = PutLocationMarker();

    //--<id>
    GetTokenAppend();
    if (token == tcIdentifier) {

	//--Verify the definition and type of the control id.
	TSymtabNode *pControlId = Find(pToken->String());
	if (pControlId->defn.how != dcUndefined) {
	    pControlType = pControlId->pType->Base();
	}
	else {
	    pControlId->defn.how = dcVariable;
	    pControlType = pControlId->pType = pIntegerType;
	}
	if (   (pControlType != pIntegerType)
	    && (pControlType != pCharType)
	    && (pControlType->form != fcEnum)) {
	    Error(errIncompatibleTypes);
	    pControlType = pIntegerType;
	}

	icode.Put(pControlId);
	GetTokenAppend();
    }
    else Error(errMissingIdentifier);

    //-- :=
    Resync(tlColonEqual, tlExpressionStart);
    CondGetTokenAppend(tcColonEqual, errMissingColonEqual);

    //--<expr-1>
    CheckAssignmentTypeCompatible(pControlType, ParseExpression(),
				  errIncompatibleTypes);

    //--TO or DOWNTO
    Resync(tlTODOWNTO, tlExpressionStart);
    if (TokenIn(token, tlTODOWNTO)) GetTokenAppend();
    else Error(errMissingTOorDOWNTO);

    //--<expr-2>
    CheckAssignmentTypeCompatible(pControlType, ParseExpression(),
				  errIncompatibleTypes);

    //--DO
    Resync(tlDO, tlStatementStart);
    CondGetTokenAppend(tcDO, errMissingDO);

    //--<stmt>
    ParseStatement();
    FixupLocationMarker(atFollowLocationMarker);
}
//endfig

//fig 10-11
//--------------------------------------------------------------
//  ParseCASE       Parse a CASE statement:
//
//                      CASE <expr> OF
//                          <case-branch> ;
//                          ...
//                      END
//--------------------------------------------------------------

void TParser::ParseCASE(void)
{
    TCaseItem *pCaseItemList;   // ptr to list of CASE items
    int        caseBranchFlag;  // true if another CASE branch,
				//   else false

    pCaseItemList = NULL;

    //--Append placeholders for the location of the token that
    //--follows the CASE statement and of the CASE branch table.
    //--Remember the locations of these placeholders.
    int atFollowLocationMarker      = PutLocationMarker();
    int atBranchTableLocationMarker = PutLocationMarker();

    //--<expr>
    GetTokenAppend();
    TType *pExprType = ParseExpression()->Base();

    //--Verify the type of the CASE expression.
    if (   (pExprType != pIntegerType)
	&& (pExprType != pCharType)
	&& (pExprType->form != fcEnum)) {
	Error(errIncompatibleTypes);
    }

    //--OF
    Resync(tlOF, tlCaseLabelStart);
    CondGetTokenAppend(tcOF, errMissingOF);

    //--Loop to parse CASE branches.
    caseBranchFlag = TokenIn(token, tlCaseLabelStart);
    while (caseBranchFlag) {
	if (TokenIn(token, tlCaseLabelStart)) {
	    ParseCaseBranch(pExprType, pCaseItemList);
	}

	if (token == tcSemicolon) {
	    GetTokenAppend();
	    caseBranchFlag = true;
	}
	else if (TokenIn(token, tlCaseLabelStart)) {
	    Error(errMissingSemicolon);
	    caseBranchFlag = true;
	}
	else caseBranchFlag = false;
    }

    //--Append the branch table to the intermediate code.
    FixupLocationMarker(atBranchTableLocationMarker);
    TCaseItem *pItem = pCaseItemList;
    TCaseItem *pNext;
    do {
	PutCaseItem(pItem->labelValue, pItem->atBranchStmt);
	pNext = pItem->next;
	delete pItem;
	pItem = pNext;
    } while (pItem);
    PutCaseItem(0, 0);  // end of table

    //--END
    Resync(tlEND, tlStatementStart);
    CondGetTokenAppend(tcEND, errMissingEND);
    FixupLocationMarker(atFollowLocationMarker);
}

//--------------------------------------------------------------
//  ParseCaseBranch     Parse a CASE branch:
//
//                          <case-label-list> : <stmt>
//
//      pExprType     : ptr to the CASE expression's type object
//      pCaseItemList : ref to ptr to list of CASE items
//--------------------------------------------------------------

void TParser::ParseCaseBranch(const TType *pExprType,
			      TCaseItem *&pCaseItemList)
{
    int caseLabelFlag;  // true if another CASE label, else false

    //--<case-label-list>
    do {
	ParseCaseLabel(pExprType, pCaseItemList);
	if (token == tcComma) {

	    //--Saw comma, look for another CASE label.
	    GetTokenAppend();
	    if (TokenIn(token, tlCaseLabelStart)) caseLabelFlag = true;
	    else {
		Error(errMissingConstant);
		caseLabelFlag = false;
	    }
	}
	else caseLabelFlag = false;

    } while (caseLabelFlag);

    //-- :
    Resync(tlColon, tlStatementStart);
    CondGetTokenAppend(tcColon, errMissingColon);

    //--Loop to set the branch statement location into each CASE item
    //--for this branch.
    for (TCaseItem *pItem = pCaseItemList;
	 pItem && (pItem->atBranchStmt == 0);
	 pItem = pItem->next) {
	pItem->atBranchStmt = icode.CurrentLocation() - 1;
    }

    //--<stmt>
    ParseStatement();
}

//--------------------------------------------------------------
//  ParseCaseLabel      Parse a case label.
//
//      pExprType     : ptr to the CASE expression's type object
//      pCaseItemList : ref to ptr to list of case items
//--------------------------------------------------------------

void TParser::ParseCaseLabel(const TType *pExprType,
			     TCaseItem *&pCaseItemList)
{
    TType *pLabelType;        // ptr to the CASE label's type object
    int    signFlag = false;  // true if unary sign, else false

    //--Allocate a new CASE item and insert it at the head of the list.
    TCaseItem *pCaseItem = new TCaseItem(pCaseItemList);

    //--Unary + or -
    if (TokenIn(token, tlUnaryOps)) {
	signFlag = true;
	GetTokenAppend();
    }

    switch (token) {

	//--Identifier:  Must be a constant whose type matches that
	//--             of the CASE expression.
	case tcIdentifier: {

	    TSymtabNode *pLabelId = Find(pToken->String());
	    icode.Put(pLabelId);

	    if (pLabelId->defn.how != dcUndefined) {
		pLabelType = pLabelId->pType->Base();
	    }
	    else {
		pLabelId->defn.how = dcConstant;
		SetType(pLabelId->pType, pDummyType);
		pLabelType = pDummyType;
	    }
	    if (pExprType != pLabelType) Error(errIncompatibleTypes);

	    //--Only an integer constant can have a unary sign.
	    if (signFlag && (pLabelType != pIntegerType)) {
		Error(errInvalidConstant);
	    }

	    //--Set the label value into the CASE item.
	    if ((pLabelType == pIntegerType) ||
		(pLabelType->form == fcEnum)) {
		pCaseItem->labelValue = signFlag
			    ? -pLabelId->defn.constant.value.integer
			    :  pLabelId->defn.constant.value.integer;
	    }
	    else {
		pCaseItem->labelValue = pLabelId->defn.constant
						    .value.character;
	    }

	    GetTokenAppend();
	    break;
	}

	//--Number:  Both the label and the CASE expression
	//--         must be integer.
	case tcNumber: {

	    if (pToken->Type() != tyInteger) Error(errInvalidConstant);
	    if (pExprType != pIntegerType) Error(errIncompatibleTypes);

	    TSymtabNode *pNode = SearchAll(pToken->String());
	    if (!pNode) {
		pNode = EnterLocal(pToken->String());
		pNode->pType = pIntegerType;
		pNode->defn.constant.value.integer =
						pToken->Value().integer;
	    }
	    icode.Put(pNode);

	    //--Set the label value into the CASE item.
	    pCaseItem->labelValue = signFlag
				? -pNode->defn.constant.value.integer
				:  pNode->defn.constant.value.integer;

	    GetTokenAppend();
	    break;
	}

	//--String:  Must be a single character without a unary sign.
	//--         (Note that the string length includes the quotes.)
	//--         The CASE expression type must be character.
	case tcString: {

	    if (signFlag || (strlen(pToken->String()) != 3)) {
		Error(errInvalidConstant);
	    }
	    if (pExprType != pCharType) Error(errIncompatibleTypes);

	    TSymtabNode *pNode = SearchAll(pToken->String());
	    if (!pNode) {
		pNode = EnterLocal(pToken->String());
		pNode->pType = pCharType;
		pNode->defn.constant.value.character =
						pToken->String()[1];
	    }
	    icode.Put(pNode);

	    //--Set the label value into the CASE item.
	    pCaseItem->labelValue = pToken->String()[1];

	    GetTokenAppend();
	    break;
	}
    }
}
//endfig

//--------------------------------------------------------------
//  ParseCompound       Parse a compound statement:
//
//                          BEGIN <stmt-list> END
//--------------------------------------------------------------

void TParser::ParseCompound(void)
{
    GetTokenAppend();

    //--<stmt-list>
    ParseStatementList(tcEND);

    //--END
    CondGetTokenAppend(tcEND, errMissingEND);
}


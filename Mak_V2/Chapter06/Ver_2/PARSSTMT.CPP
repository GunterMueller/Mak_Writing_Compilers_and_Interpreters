//fig 6-6
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Statements)                              *
//  *                                                           *
//  *	Parse statements.					*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog6-1/parsstmt.cpp                           *
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
	case tcIdentifier:  ParseAssignment();  break;
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
//--------------------------------------------------------------

void TParser::ParseAssignment(void)
{
    //--Search for the target variable's identifier and enter it
    //--if necessary.  Append the symbol table node handle
    //--to the icode.
    TSymtabNode       *pTargetNode = SearchAll (pToken->String());
    if (!pTargetNode)  pTargetNode = EnterLocal(pToken->String());

    icode.Put(pTargetNode);
    GetTokenAppend();

    //-- :=
    Resync(tlColonEqual, tlExpressionStart);
    CondGetTokenAppend(tcColonEqual, errMissingColonEqual);

    //--<expr>
    ParseExpression();
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

    //--<expr>
    InsertLineMarker();
    ParseExpression();
}

//--------------------------------------------------------------
//  ParseWHILE      Parse a WHILE statement.:
//
//                      WHILE <expr> DO <stmt>
//--------------------------------------------------------------

void TParser::ParseWHILE(void)
{
    //--<expr>
    GetTokenAppend();
    ParseExpression();

    //--DO
    Resync(tlDO, tlStatementStart);
    CondGetTokenAppend(tcDO, errMissingDO);

    //--<stmt>
    ParseStatement();
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
    //--<expr>
    GetTokenAppend();
    ParseExpression();

    //--THEN
    Resync(tlTHEN, tlStatementStart);
    CondGetTokenAppend(tcTHEN, errMissingTHEN);

    //--<stmt-1>
    ParseStatement();

    if (token == tcELSE) {

	//--ELSE <stmt-2>
	GetTokenAppend();
	ParseStatement();
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
    //--<id>
    GetTokenAppend();
    if ((token == tcIdentifier) && (! SearchAll(pToken->String()))) {
	Error(errUndefinedIdentifier);
    }
    CondGetTokenAppend(tcIdentifier, errMissingIdentifier);

    //-- :=
    Resync(tlColonEqual, tlExpressionStart);
    CondGetTokenAppend(tcColonEqual, errMissingColonEqual);

    //--<expr-1>
    ParseExpression();

    //--TO or DOWNTO
    Resync(tlTODOWNTO, tlExpressionStart);
    if (TokenIn(token, tlTODOWNTO)) GetTokenAppend();
    else Error(errMissingTOorDOWNTO);

    //--<expr-2>
    ParseExpression();

    //--DO
    Resync(tlDO, tlStatementStart);
    CondGetTokenAppend(tcDO, errMissingDO);

    //--<stmt>
    ParseStatement();
}

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
    int caseBranchFlag;  // true if another CASE branch, else false

    //--<expr>
    GetTokenAppend();
    ParseExpression();

    //--OF
    Resync(tlOF, tlCaseLabelStart);
    CondGetTokenAppend(tcOF, errMissingOF);

    //--Loop to parse CASE branches.
    caseBranchFlag = TokenIn(token, tlCaseLabelStart);
    while (caseBranchFlag) {
	if (TokenIn(token, tlCaseLabelStart)) ParseCaseBranch();

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

    //--END
    Resync(tlEND, tlStatementStart);
    CondGetTokenAppend(tcEND, errMissingEND);
}

//--------------------------------------------------------------
//  ParseCaseBranch     Parse a CASE branch:
//
//                          <case-label-list> : <stmt>
//--------------------------------------------------------------

void TParser::ParseCaseBranch(void)
{
    int caseLabelFlag;  // true if another CASE label, else false

    //--<case-label-list>
    do {
	ParseCaseLabel();
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

    //--<stmt>
    ParseStatement();
}

//--------------------------------------------------------------
//  ParseCaseLabel      Parse a CASE label.
//--------------------------------------------------------------

void TParser::ParseCaseLabel(void)
{
    int signFlag = false;  // true if unary sign, else false

    //--Unary + or -
    if (TokenIn(token, tlUnaryOps)) {
	signFlag = true;
	GetTokenAppend();
    }

    switch (token) {

	//--Identifier:  Must be defined.
	case tcIdentifier:
	    if (! SearchAll(pToken->String())) {
		Error(errUndefinedIdentifier);
	    }
	    GetTokenAppend();
	    break;

	//--Number:  Must be integer.
	case tcNumber:
	    if (pToken->Type() != tyInteger) Error(errInvalidConstant);
	    GetTokenAppend();
	    break;

	//--String:  Must be a single character without a unary sign.
	//--         (Note that the string length includes the quotes.)
	case tcString:
	    if (signFlag || (strlen(pToken->String()) != 3)) {
		Error(errInvalidConstant);
	    }
	    GetTokenAppend();
	    break;
    }
}

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
//endfig


//fig 5-23
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Expressions)                             *
//  *                                                           *
//  *	Parse expressions.					*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog5-2/parsexpr.cpp                           *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "parser.h"

//--------------------------------------------------------------
//  ParseExpression     Parse an expression (binary relational
//                      operators = < > <> <= and >= ).
//--------------------------------------------------------------

void TParser::ParseExpression(void)
{
    ParseSimpleExpression();

    //--If there we now see a relational operator,
    //--parse a second simple expression.
    if ((token == tcEqual) || (token == tcNe) ||
	(token == tcLt)    || (token == tcGt) ||
	(token == tcLe)    || (token == tcGe)) {
	GetTokenAppend();
	ParseSimpleExpression();
    }
}

//--------------------------------------------------------------
//  ParseSimpleExpression       Parse a simple expression
//                              (unary operators + or - , and
//                              binary operators + - and OR).
//--------------------------------------------------------------

void TParser::ParseSimpleExpression(void)
{
    //--Unary + or -
    if ((token == tcPlus) || (token == tcMinus)) {
	GetTokenAppend();
    }

    //--Parse the first term.
    ParseTerm();

    //--Loop to parse subsequent additive operators and terms.
    while ((token == tcPlus) || (token == tcMinus) ||
	   (token == tcOR)) {
	GetTokenAppend();
	ParseTerm();
    }
}

//--------------------------------------------------------------
//  ParseTerm           Parse a term (binary operators * / DIV
//                      MOD and AND).
//--------------------------------------------------------------

void TParser::ParseTerm(void)
{
    //--Parse the first factor.
    ParseFactor();

    //--Loop to parse subsequent multiplicative operators and factors.
    while ((token == tcStar) || (token == tcSlash) ||
	   (token == tcDIV)  || (token == tcMOD)   ||
	   (token == tcAND)) {
	GetTokenAppend();
	ParseFactor();
    }
}

//--------------------------------------------------------------
//  ParseFactor         Parse a factor (identifier, number,
//                      string, NOT <factor>, or parenthesized
//                      subexpression).
//--------------------------------------------------------------

void TParser::ParseFactor(void)
{
    switch (token) {

	case tcIdentifier: {

	    //--Search for the identifier.  If found, append the
	    //--symbol table node handle to the icode.  If not
	    //--found, enter it and flag an undefined identifier error.
	    TSymtabNode *pNode = SearchAll(pToken->String());
	    if (pNode) icode.Put(pNode);
	    else {
		Error(errUndefinedIdentifier);
		EnterLocal(pToken->String());
	    }

	    GetTokenAppend();
	    break;
	}

	case tcNumber: {

	    //--Search for the number and enter it if necessary.
	    //--Set the number's value in the symbol table node.
	    //--Append the symbol table node handle to the icode.
	    TSymtabNode *pNode = SearchAll(pToken->String());
	    if (!pNode) {
		pNode = EnterLocal(pToken->String());
		pNode->value = pToken->Type() == tyInteger
				    ? (float) pToken->Value().integer
				    : pToken->Value().real;
	    }
	    icode.Put(pNode);

	    GetTokenAppend();
	    break;
	}

	case tcString:
	    GetTokenAppend();
	    break;

	case tcNOT:
	    GetTokenAppend();
	    ParseFactor();
	    break;

	case tcLParen:

	    //--Parenthesized subexpression:  Call ParseExpression
	    //--                              recursively ...
	    GetTokenAppend();
	    ParseExpression();

	    //-- ... and check for the closing right parenthesis.
	    if (token == tcRParen) GetTokenAppend();
	    else                   Error(errMissingRightParen);
	    
	    break;

	default:
	    Error(errInvalidExpression);
	    break;
    }
}
//endfig

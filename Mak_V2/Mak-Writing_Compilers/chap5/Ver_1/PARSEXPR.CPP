//fig 5-12
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Expressions)                             *
//  *                                                           *
//  *	Parse expressions.					*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog5-1/parsexpr.cpp                           *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "parser.h"

//--------------------------------------------------------------
//  ParseExpression             Parse an expression.
//--------------------------------------------------------------

void TParser::ParseExpression(void)
{
    ParseSimpleExpression();
}

//--------------------------------------------------------------
//  ParseSimpleExpression       Parse a simple expression
//                              (unary operators + or -
//                              and binary operators + and - ).
//--------------------------------------------------------------

void TParser::ParseSimpleExpression(void)
{
    TTokenCode op;                // binary operator
    TTokenCode unaryOp = tcPlus;  // unary  operator

    //--Unary + or -
    if ((token == tcMinus) || (token == tcPlus)) {
	unaryOp = token;
	GetToken();
    }

    //--Parse the first term and then negate its value
    //--if there was a unary -.
    ParseTerm();
    if (unaryOp == tcMinus) runStack.Push(-runStack.Pop());

    //--Loop to parse subsequent additive operators and terms.
    while ((token == tcMinus) || (token == tcPlus)) {
	op = token;

	GetToken();
	ParseTerm();

	//--Pop off the two operand values, ...
	float operand2 = runStack.Pop();
	float operand1 = runStack.Pop();

	//--... perform the operation, and push the resulting value
	//--    onto the runtime stack.
	runStack.Push(op == tcPlus ? operand1 + operand2
				   : operand1 - operand2);
    }
}

//--------------------------------------------------------------
//  ParseTerm           Parse a term (binary operators * and /).
//--------------------------------------------------------------

void TParser::ParseTerm(void)
{
    TTokenCode op;  // binary operator

    //--Parse the first factor.
    ParseFactor();

    //--Loop to parse subsequent multiplicative operators and factors.
    while ((token == tcStar) || (token == tcSlash)) {
	op = token;

	GetToken();
	ParseFactor();

	//--Pop off the two operand values, ...
	float operand2 = runStack.Pop();
	float operand1 = runStack.Pop();

	//--... perform the operation, and push the resulting value
	//--    onto the runtime stack.
	if      (op == tcStar)  runStack.Push(operand1*operand2);
	else if (op == tcSlash) {
	    if (operand2 != 0.0) runStack.Push(operand1/operand2);
	    else {
		//--Division by zero runtime error.
		list.PutLine("*** RUNTIME ERROR: Division by zero.");
		runStack.Push(0.0);
	    }
	}
    }
}

//--------------------------------------------------------------
//  ParseFactor         Parse a factor (identifier, number, or a
//                      parenthesized subexpression).  Print the
//                      value of each identifier.
//--------------------------------------------------------------

void TParser::ParseFactor(void)
{
    switch (token) {

	case tcIdentifier: {

	    //--Search for the identifier.  If found, push and print
	    //--its value.  Undefined identifier error if not found.
	    TSymtabNode *pNode = SearchAll(pToken->String());
	    if (pNode) {

		//--Found:  Extract the value from the symbol table
		//--        node and push it onto the runtime stack.
		runStack.Push(pNode->value);

		//--Print the variable's identifier and value.
		sprintf(list.text, "\t>> %s = %g", pNode->String(),
						   pNode->value);
		list.PutLine();
	    }
	    else {

		//--Not found:  Undefined identifier error.
		Error(errUndefinedIdentifier);
		runStack.Push(0.0);
	    }

	    GetToken();
	    break;
	}

	case tcNumber:

	    //--Push the number's value onto the runtime stack.
	    //--First convert and integer value to real.
	    runStack.Push(pToken->Type() == tyInteger
			    ? (float) pToken->Value().integer
			    : pToken->Value().real);
	    GetToken();
	    break;

	case tcLParen:

	    //--Parenthesized subexpression:  Call ParseExpression
	    //--                              recursively ...
	    GetToken();
	    ParseExpression();

	    //-- ... and check for the closing right parenthesis.
	    if (token == tcRParen) GetToken();
	    else                   Error(errMissingRightParen);

	    break;

	default:
	    Error(errInvalidExpression);
	    break;
    }
}
//endfig


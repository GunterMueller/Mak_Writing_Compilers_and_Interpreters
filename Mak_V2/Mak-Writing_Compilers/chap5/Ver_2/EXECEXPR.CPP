//fig 5-29
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Expressions)                         *
//  *                                                           *
//  *   Execute expressions.             			*
//  *                                                           *
//  *	CLASSES: TExecutor					*
//  *                                                           *
//  *   FILE:    prog5-2/execexpr.cpp				*
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include "exec.h"

//--------------------------------------------------------------
//  ExecuteExpression   Execute an expression (binary relational
//                      operators = < > <> <= and >= ).
//--------------------------------------------------------------

void TExecutor::ExecuteExpression(void)
{
    TTokenCode op;

    ExecuteSimpleExpression();

    //--If there we now see a relational operator,
    //--execute a second simple expression.
    if ((token == tcEqual) || (token == tcNe) ||
	(token == tcLt)    || (token == tcGt) ||
	(token == tcLe)    || (token == tcGe)) {

	op = token;

	GetToken();
	ExecuteSimpleExpression();

	//--Pop off the two operand values, ...
	float operand2 = runStack.Pop();
	float operand1 = runStack.Pop();

	//-- ... perform the operation, and push the resulting value
	//--     onto the runtime stack.
	switch (op) {

	    case tcEqual:
		runStack.Push(operand1 == operand2 ? 1.0 : 0.0);
		break;

	    case tcNe:
		runStack.Push(operand1 != operand2 ? 1.0 : 0.0);
		break;

	    case tcLt:
		runStack.Push(operand1 <  operand2 ? 1.0 : 0.0);
		break;

	    case tcGt:
		runStack.Push(operand1 >  operand2 ? 1.0 : 0.0);
		break;

	    case tcLe:
		runStack.Push(operand1 <= operand2 ? 1.0 : 0.0);
		break;

	    case tcGe:
		runStack.Push(operand1 >= operand2 ? 1.0 : 0.0);
		break;
	}
    }
}

//--------------------------------------------------------------
//  ExecuteSimpleExpression     Execute a simple expression
//                              (unary operators + or -
//                              and binary operators + -
//                              and OR).
//--------------------------------------------------------------

void TExecutor::ExecuteSimpleExpression(void)
{
    TTokenCode op;                // binary operator
    TTokenCode unaryOp = tcPlus;  // unary  operator

    //-- Unary + or -
    if ((token == tcPlus) || (token == tcMinus)) {
	unaryOp = token;
	GetToken();
    }

    //--Execute the first term and then negate its value
    //--if there was a unary -.
    ExecuteTerm();
    if (unaryOp == tcMinus) runStack.Push(-runStack.Pop());

    //--Loop to execute subsequent additive operators and terms.
    while ((token == tcPlus) || (token == tcMinus) ||
	   (token == tcOR)) {
	op = token;

	GetToken();
	ExecuteTerm();

	//--Pop off the two operand values, ...
	float operand2 = runStack.Pop();
	float operand1 = runStack.Pop();

	//-- ... perform the operation, and push the resulting value
	//--     onto the runtime stack.
	switch (op) {

	    case tcPlus:
		runStack.Push(operand1 + operand2);
		break;

	    case tcMinus:
		runStack.Push(operand1 - operand2);
		break;

	    case tcOR:
		runStack.Push((operand1 != 0.0) || (operand2 != 0.0)
				? 1.0 : 0.0);
		break;
	}
    }
}

//--------------------------------------------------------------
//  ExecuteTerm         Execute a term (binary operators * / DIV
//                      MOD and AND).
//--------------------------------------------------------------

void TExecutor::ExecuteTerm(void)
{
    TTokenCode op;

    //--Execute the first factor.
    ExecuteFactor();

    //--Loop to execute subsequent multiplicative operators and factors.
    while ((token == tcStar) || (token == tcSlash) ||
	   (token == tcDIV)  || (token == tcMOD)   ||
	   (token == tcAND)) {
	op = token;

	GetToken();
	ExecuteFactor();

	//--Pop off the two operand values, ...
	float operand2 = runStack.Pop();
	float operand1 = runStack.Pop();

	//-- ... perform the operation, and push the resulting value
	//--     onto the runtime stack.
	int divZeroFlag = false;  // true if division by 0, else false
	switch (op) {

	    case tcStar:
		runStack.Push(operand1 * operand2);
		break;

	    case tcSlash:
		if (operand2 != 0.0) runStack.Push(operand1 / operand2);
		else                 divZeroFlag = true;
		break;

	    case tcDIV:
		if (operand2 != 0.0) runStack.Push(  int(operand1)
						   / int(operand2));
		else                 divZeroFlag = true;
		break;

	    case tcMOD:
		if (operand2 != 0.0) runStack.Push(  int(operand1)
						   % int(operand2));
		else                 divZeroFlag = true;
		break;

	    case tcAND:
		runStack.Push((operand1 != 0.0) && (operand2 != 0.0)
				? 1.0 : 0.0);
		break;
	}
	if (divZeroFlag) {

	    //--Division by zero runtime error.
	    RuntimeError(rteDivisionByZero);
	    runStack.Push(0.0);
	}
    }
}

//--------------------------------------------------------------
//  ExecuteFactor       Execute a factor (identifier, number,
//                      string, NOT <factor>, or parenthesized
//                      subexpression).  If the identifier is
//                      the variable "input", prompt for its
//                      value.
//--------------------------------------------------------------

void TExecutor::ExecuteFactor(void)
{
    switch (token) {

	case tcIdentifier: {

	    //--If the variable is "input", prompt for its value.
	    if (pNode == pInputNode) {
		cout << ">> At " << currentLineNumber << ":  input ? ";
		cin  >> pNode->value;
		if (!cin.good()) RuntimeError(rteInvalidUserInput);
	    }

	    //--Push the variable's value onto the runtime stack.
	    runStack.Push(pNode->value);
	    GetToken();
	    break;
	}

	case tcNumber: {

	    //--Push the number's value onto the runtime stack.
	    runStack.Push(pNode->value);
	    GetToken();
	    break;
	}

	case tcString:

	    //--Just push 0.0 for now.
	    runStack.Push(0.0);
	    GetToken();
	    break;

	case tcNOT:

	    //--Execute factor and invert its value.
	    GetToken();
	    ExecuteFactor();
	    runStack.Push((runStack.Pop() != 0.0) ? 0.0 : 1.0);
	    break;

	case tcLParen: {

	    //--Parenthesized subexpression:  Call ExecuteExpression
	    //--                              recursively.
	    GetToken();           // first token after (
	    ExecuteExpression();
	    GetToken();           // first token after )
	}
    }
}
//endfig

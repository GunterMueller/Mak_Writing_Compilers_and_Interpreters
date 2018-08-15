//fig 9-14
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Expressions)                         *
//  *                                                           *
//  *   Execute expressions.                                    *
//  *                                                           *
//  *   CLASSES: TExecutor                                      *
//  *                                                           *
//  *   FILE:    prog9-1/execexpr.cpp                           *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include "common.h"
#include "exec.h"

//--------------------------------------------------------------
//  ExecuteExpression   Execute an expression (binary relational
//                      operators = < > <> <= and >= ).
//
//  Return: ptr to expression's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteExpression(void)
{
    TType      *pOperand1Type;  // ptr to first  operand's type
    TType      *pOperand2Type;  // ptr to second operand's type
    TType      *pResultType;    // ptr to result type
    TTokenCode  op;             // operator

    //--Execute the first simple expression.
    pResultType = ExecuteSimpleExpression();

    //--If we now see a relational operator,
    //--execute the second simple expression.
    if (TokenIn(token, tlRelOps)) {
	op            = token;
	pOperand1Type = pResultType->Base();
	pResultType   = pBooleanType;

	GetToken();
	pOperand2Type = ExecuteSimpleExpression()->Base();

	//--Perform the operation, and push the resulting value
	//--onto the stack.
	if (   ((pOperand1Type == pIntegerType) &&
		(pOperand2Type == pIntegerType))
	    || ((pOperand1Type == pCharType) &&
		(pOperand2Type == pCharType))
	    || (pOperand1Type->form == fcEnum)) {

	    //--integer <op> integer
	    //--boolean <op> boolean
	    //--char    <op> char
	    //--enum    <op> enum
	    int value1, value2;
	    if (pOperand1Type == pCharType) {
		value2 = Pop()->character;
		value1 = Pop()->character;
	    }
	    else {
		value2 = Pop()->integer;
		value1 = Pop()->integer;
	    }

	    switch (op) {

		case tcEqual:
		    Push(value1 == value2);
		    break;

		case tcNe:
		    Push(value1 != value2);
		    break;

		case tcLt:
		    Push(value1 <  value2);
		    break;

		case tcGt:
		    Push(value1 >  value2);
		    break;

		case tcLe:
		    Push(value1 <= value2);
		    break;

		case tcGe:
		    Push(value1 >= value2);
		    break;
	    }
	}
	else if ((pOperand1Type == pRealType) ||
		 (pOperand2Type == pRealType)) {

	    //--real    <op> real
	    //--real    <op> integer
	    //--integer <op> real
	    float value2 = pOperand2Type == pRealType ? Pop()->real
						      : Pop()->integer;
	    float value1 = pOperand1Type == pRealType ? Pop()->real
						      : Pop()->integer;

	    switch (op) {

		case tcEqual:
		    Push(value1 == value2);
		    break;

		case tcNe:
		    Push(value1 != value2);
		    break;

		case tcLt:
		    Push(value1 <  value2);
		    break;

		case tcGt:
		    Push(value1 >  value2);
		    break;

		case tcLe:
		    Push(value1 <= value2);
		    break;

		case tcGe:
		    Push(value1 >= value2);
		    break;
	    }
	}
	else {

	    //--string <op> string
	    char *addr2 = (char *) Pop()->address;
	    char *addr1 = (char *) Pop()->address;

	    int cmp = strncmp(addr1, addr2, pOperand1Type->size);

	    switch (op) {
		case tcEqual:   Push(cmp == 0);  break;
		case tcNe:      Push(cmp != 0);  break;
		case tcLt:      Push(cmp <  0);  break;
		case tcGt:      Push(cmp >  0);  break;
		case tcLe:      Push(cmp <= 0);  break;
		case tcGe:      Push(cmp >= 0);  break;
	    }
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ExecuteSimpleExpression     Execute a simple expression
//                              (unary operators + or -
//                              and binary operators + -
//                              and OR).
//
//  Return: ptr to expression's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteSimpleExpression(void)
{
    TType      *pOperandType;      // ptr to operand's type
    TType      *pResultType;       // ptr to result type
    TTokenCode  op;                // operator
    TTokenCode  unaryOp = tcPlus;  // unary operator

    //--Unary + or -
    if (TokenIn(token, tlUnaryOps)) {
	unaryOp = token;
	GetToken();
    }

    //--Execute the first term.
    pResultType = ExecuteTerm();

    //--If there was a unary -, negate the first operand value.
    if (unaryOp == tcMinus) {
	if (pResultType == pRealType) Push(-Pop()->real);
	else                            Push(-Pop()->integer);
    }

    //--Loop to execute subsequent additive operators and terms.
    while (TokenIn(token, tlAddOps)) {
	op = token;
	pResultType = pResultType->Base();

	GetToken();
	pOperandType = ExecuteTerm()->Base();

	//--Perform the operation, and push the resulting value
	//--onto the stack.
	if (op == tcOR) {

	    //--boolean OR boolean
	    int value2 = Pop()->integer;
	    int value1 = Pop()->integer;

	    Push(value1 || value2);
	    pResultType = pBooleanType;
	}
	else if ((pResultType  == pIntegerType) &&
		 (pOperandType == pIntegerType)) {

	    //--integer +|- integer
	    int value2 = Pop()->integer;
	    int value1 = Pop()->integer;

	    Push(op == tcPlus ? value1 + value2
			      : value1 - value2);
	    pResultType = pIntegerType;
	}
	else {

	    //--real    +|- real
	    //--real    +|- integer
	    //--integer +|- real
	    float value2 = pOperandType == pRealType ? Pop()->real
						     : Pop()->integer;
	    float value1 = pResultType  == pRealType ? Pop()->real
						     : Pop()->integer;

	    Push(op == tcPlus ? value1 + value2
			      : value1 - value2);
	    pResultType = pRealType;
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ExecuteTerm         Execute a term (binary operators * / DIV
//                      MOD and AND).
//
//  Return: ptr to term's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteTerm(void)
{
    TType      *pOperandType;  // ptr to operand's type
    TType      *pResultType;   // ptr to result type
    TTokenCode  op;            // operator

    //--Execute the first factor.
    pResultType = ExecuteFactor();

    //--Loop to execute subsequent multiplicative operators and factors.
    while (TokenIn(token, tlMulOps)) {
	op = token;
	pResultType = pResultType->Base();

	GetToken();
	pOperandType = ExecuteFactor()->Base();

	//--Perform the operation, and push the resulting value
	//--onto the stack.
	switch (op) {

	    case tcAND: {

		//--boolean AND boolean
		int value2 = Pop()->integer;
		int value1 = Pop()->integer;

		Push(value1 && value2);
		pResultType = pBooleanType;
		break;
	    }

	    case tcStar:

		if ((pResultType  == pIntegerType) &&
		    (pOperandType == pIntegerType)) {

		    //--integer * integer
		    int value2 = Pop()->integer;
		    int value1 = Pop()->integer;

		    Push(value1*value2);
		    pResultType = pIntegerType;
		}
		else {

		    //--real    * real
		    //--real    * integer
		    //--integer * real
		    float value2 = pOperandType == pRealType
					    ? Pop()->real
					    : Pop()->integer;
		    float value1 = pResultType == pRealType
					    ? Pop()->real
					    : Pop()->integer;

		    Push(value1*value2);
		    pResultType = pRealType;
		}
		break;

	    case tcSlash: {

		//--real    / real
		//--real    / integer
		//--integer / real
		//--integer / integer
		float value2 = pOperandType == pRealType
					? Pop()->real
					: Pop()->integer;
		float value1 = pResultType == pRealType
					? Pop()->real
					: Pop()->integer;

		if (value2 == 0.0f) RuntimeError(rteDivisionByZero);

		Push(value1/value2);
		pResultType = pRealType;
		break;
	    }

	    case tcDIV:
	    case tcMOD: {

		//--integer DIV|MOD integer
		int value2 = Pop()->integer;
		int value1 = Pop()->integer;

		if (value2 == 0) RuntimeError(rteDivisionByZero);

		Push(op == tcDIV ? value1/value2
				 : value1%value2);
		pResultType = pIntegerType;
		break;
	    }
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ExecuteFactor       Execute a factor (identifier, number,
//                      string, NOT <factor>, or parenthesized
//                      subexpression).  An identifier can be
//                      a function, constant, or variable.
//
//  Return: ptr to factor's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteFactor(void)
{
    TType *pResultType;  // ptr to result type

    switch (token) {

	case tcIdentifier: {
	    switch (pNode->defn.how) {

		case dcFunction:
		    pResultType = ExecuteSubroutineCall(pNode);
		    break;

		case dcConstant:
		    pResultType = ExecuteConstant(pNode);
		    break;

		default:
		    pResultType = ExecuteVariable(pNode, false);
		    break;
	    }
	    break;
	}

	case tcNumber: {

	    //--Push the number's integer or real value onto the stack.
	    if (pNode->pType == pIntegerType) {
		Push(pNode->defn.constant.value.integer);
	    }
	    else {
		Push(pNode->defn.constant.value.real);
	    }
	    pResultType = pNode->pType;

	    GetToken();
	    break;
	}

	case tcString: {

	    //--Push either a character or a string address onto the
	    //--runtime stack, depending on the string length.
	    int length = strlen(pNode->String()) - 2;  // skip quotes
	    if (length == 1) {

		//--Character
		Push(pNode->defn.constant.value.character);
		pResultType = pCharType;
	    }
	    else {

		//--String address
		Push(pNode->defn.constant.value.pString);
		pResultType = pNode->pType;
	    }

	    GetToken();
	    break;
	}

	case tcNOT:

	    //--Execute boolean factor and invert its value.
	    GetToken();
	    ExecuteFactor();

	    Push(1 - Pop()->integer);
	    pResultType = pBooleanType;
	    break;

	case tcLParen: {

	    //--Parenthesized subexpression:  Call ExecuteExpression
	    //--                              recursively.
	    GetToken();  // first token after (

	    pResultType = ExecuteExpression();

	    GetToken();  // first token after )
	    break;
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ExecuteConstant     Push a constant onto the runtime stack.
//
//      pId : ptr to constant identifier's symbol table node
//
//  Return: ptr to constant's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteConstant(const TSymtabNode *pId)
{
    TType      *pType = pId->pType;
    TDataValue  value = pId->defn.constant.value;

    if      (pType == pRealType)     Push(value.real);
    else if (pType == pCharType)     Push(value.character);
    else if (pType->form == fcArray) Push(value.pString);
    else                             Push(value.integer);

    GetToken();
    TraceDataFetch(pId, TOS(), pType);
    return pType;
}

//--------------------------------------------------------------
//  ExecuteVariable     Push a variable's value or address onto
//                      the runtime stack.
//
//      pId         : ptr to variable's symbol table node
//      addressFlag : true to push address, false to push value
//
//  Return: ptr to variable's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteVariable(const TSymtabNode *pId,
				  int addressFlag)
{
    TType *pType = pId->pType;

    //--Get the variable's runtime stack address.
    TStackItem *pEntry = runStack.GetValueAddress(pId);

    //--If it's a VAR formal parameter, or the type is an array
    //--or record, then the stack item contains the address
    //--of the data.  Push the data address onto the stack.
    Push((pId->defn.how == dcVarParm) || (! pType->IsScalar())
		? pEntry->address
		: pEntry);

    GetToken();

    //--Loop to execute any subscripts and field designators,
    //--which will modify the data address at the top of the stack.
    int doneFlag = false;
    do {
	switch (token) {

	    case tcLBracket:
		pType = ExecuteSubscripts(pType);
		break;

	    case tcPeriod:
		pType = ExecuteField();
		break;

	    default:  doneFlag = true;
	}
    } while (!doneFlag);

    //--If addressFlag is false, and the data is not an array
    //--or a record, replace the address at the top of the stack
    //--with the data value.
    if ((!addressFlag) && (pType->IsScalar())) {
	if (pType == pRealType) {
	    Push(((TStackItem *) Pop()->address)->real);
	}
	else if (pType->Base() == pCharType) {
	    Push(((TStackItem *) Pop()->address)->character);
	}
	else {
	    Push(((TStackItem *) Pop()->address)->integer);
	}
    }

    if (!addressFlag) {
	void *pDataValue = pType->IsScalar() ? TOS() : TOS()->address;

	TraceDataFetch(pId, pDataValue, pType);
    }

    return pType;
}

//--------------------------------------------------------------
//  ExecuteSubscripts   Execute each subscript expression to
//                      modify the data address at the top of
//                      the runtime stack.
//
//      pType : ptr to array type object
//
//  Return: ptr to subscripted variable's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteSubscripts(const TType *pType)
{
    //--Loop to executed subscript lists enclosed in brackets.
    while (token == tcLBracket) {

	//--Loop to execute comma-separated subscript expressions
	//--within a subscript list.
	do {
	    GetToken();
	    ExecuteExpression();

	    //--Evaluate and range check the subscript.
	    int value = Pop()->integer;
	    RangeCheck(pType, value);

	    //--Modify the data address at the top of the stack.
	    Push(((char *) Pop()->address) +
				 pType->array.pElmtType->size
				*(value - pType->array.minIndex));

	    //--Prepare for another subscript in this list.
	    if (token == tcComma) pType = pType->array.pElmtType;

	} while (token == tcComma);

	//--Prepare for another subscript list.
	GetToken();
	if (token == tcLBracket) pType = pType->array.pElmtType;
    }

    return pType->array.pElmtType;
}

//--------------------------------------------------------------
//  ExecuteField        Execute a field designator to modify the
//                      data address at the top of the runtime
//                      stack
//
//  Return: ptr to record field's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteField(void)
{
    GetToken();
    TSymtabNode *pFieldId = pNode;

    Push(((char *) (Pop()->address)) + pFieldId->defn.data.offset);

    GetToken();
    return pFieldId->pType;
}
//endfig

//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Expressions)                             *
//  *                                                           *
//  *   Parse expressions.                                      *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog8-1/parsexpr.cpp                           *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include "common.h"
#include "parser.h"

//--------------------------------------------------------------
//  ParseExpression     Parse an expression (binary relational
//                      operators = < > <> <= and >= ).
//
//  Return: ptr to the expression's type object
//--------------------------------------------------------------

TType *TParser::ParseExpression(void)
{
    TType *pResultType;   // ptr to result type
    TType *pOperandType;  // ptr to operand type

    //--Parse the first simple expression.
    pResultType = ParseSimpleExpression();

    //--If we now see a relational operator,
    //--parse the second simple expression.
    if (TokenIn(token, tlRelOps)) {
	GetTokenAppend();
	pOperandType = ParseSimpleExpression();

	//--Check the operand types and return the boolean type.
	CheckRelOpOperands(pResultType, pOperandType);
	pResultType = pBooleanType;
    }

    //--Make sure the expression ended properly.
    Resync(tlExpressionFollow, tlStatementFollow, tlStatementStart);

    return pResultType;
}

//--------------------------------------------------------------
//  ParseSimpleExpression       Parse a simple expression
//                              (unary operators + or - , and
//                              binary operators + - and OR).
//
//  Return: ptr to the simple expression's type object
//--------------------------------------------------------------

TType *TParser::ParseSimpleExpression(void)
{
    TType      *pResultType;          // ptr to result type
    TType      *pOperandType;         // ptr to operand type
    TTokenCode  op;                   // operator
    int         unaryOpFlag = false;  // true if unary op, else false

    //--Unary + or -
    if (TokenIn(token, tlUnaryOps)) {
	unaryOpFlag = true;
	GetTokenAppend();
    }

    //--Parse the first term.
    pResultType = ParseTerm();

    //--If there was a unary sign, check the term's type.
    if (unaryOpFlag) CheckIntegerOrReal(pResultType);

    //--Loop to parse subsequent additive operators and terms.
    while (TokenIn(token, tlAddOps)) {

	//--Remember the operator and parse the subsequent term.
	op = token;
	GetTokenAppend();
	pOperandType = ParseTerm();

	//--Check the operand types to determine the result type.
	switch (op) {

	    case tcPlus:
	    case tcMinus:

		//--integer <op> integer => integer
		if (IntegerOperands(pResultType, pOperandType)) {
		    pResultType = pIntegerType;
		}

		//--real    <op> real    => real
		//--real    <op> integer => real
		//--integer <op> real    => real
		else if (RealOperands(pResultType, pOperandType)) {
		    pResultType = pRealType;
		}

		else Error(errIncompatibleTypes);
		break;

	    case tcOR:

		//--boolean OR boolean => boolean
		CheckBoolean(pResultType, pOperandType);
		pResultType = pBooleanType;
		break;
	}

    }

    return pResultType;
}

//--------------------------------------------------------------
//  ParseTerm           Parse a term (binary operators * / DIV
//                      MOD and AND).
//
//  Return: ptr to the term's type object
//--------------------------------------------------------------

TType *TParser::ParseTerm(void)
{
    TType      *pResultType;   // ptr to result type
    TType      *pOperandType;  // ptr to operand type
    TTokenCode  op;               // operator

    //--Parse the first factor.
    pResultType = ParseFactor();

    //--Loop to parse subsequent multiplicative operators and factors.
    while (TokenIn(token, tlMulOps)) {

	//--Remember the operator and parse the subsequent factor.
	op = token;
	GetTokenAppend();
	pOperandType = ParseFactor();

	//--Check the operand types to determine the result type.
	switch (op) {

	    case tcStar:

		//--integer * integer => integer
		if (IntegerOperands(pResultType, pOperandType)) {
		    pResultType = pIntegerType;
		}

		//--real    * real    => real
		//--real    * integer => real
		//--integer * real    => real
		else if (RealOperands(pResultType, pOperandType)) {
		    pResultType = pRealType;
		}

		else Error(errIncompatibleTypes);
		break;

	    case tcSlash:

		//--integer / integer => real
		//--real    / real    => real
		//--real    / integer => real
		//--integer / real    => real
		if (   IntegerOperands(pResultType, pOperandType)
		    || RealOperands   (pResultType, pOperandType)) {
		    pResultType = pRealType;
		}
		else Error(errIncompatibleTypes);
		break;

	    case tcDIV:
	    case tcMOD:

		//--integer <op> integer => integer
		if (IntegerOperands(pResultType, pOperandType)) {
		    pResultType = pIntegerType;
		}
		else Error(errIncompatibleTypes);
		break;

	    case tcAND:

		//--boolean AND boolean => boolean
		CheckBoolean(pResultType, pOperandType);
		pResultType = pBooleanType;
		break;
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ParseFactor         Parse a factor (identifier, number,
//                      string, NOT <factor>, or parenthesized
//                      subexpression).
//
//  Return: ptr to the factor's type object
//--------------------------------------------------------------

TType *TParser::ParseFactor(void)
{
    TType *pResultType;  // ptr to result type

    switch (token) {

//fig 8-19
	case tcIdentifier: {

	    //--Search for the identifier and enter it if
	    //--necessary.  Append the symbol table node handle
	    //--to the icode.
	    TSymtabNode *pNode = Find(pToken->String());
	    icode.Put(pNode);

	    if (pNode->defn.how == dcUndefined) {
		pNode->defn.how = dcVariable;
		SetType(pNode->pType, pDummyType);
	    }

	    //--Based on how the identifier is defined,
	    //--parse a constant, function call, or variable.
	    switch (pNode->defn.how) {

		case dcFunction:
		    pResultType = ParseSubroutineCall(pNode, true);
		    break;

		case dcProcedure:
		    Error(errInvalidIdentifierUsage);
		    pResultType = ParseSubroutineCall(pNode, false);
		    break;

		case dcConstant:
		    GetTokenAppend();
		    pResultType = pNode->pType;
		    break;

		default:
		    pResultType = ParseVariable(pNode);
		    break;
	    }

	    break;
	}
//endfig

	case tcNumber: {

	    //--Search for the number and enter it if necessary.
	    TSymtabNode *pNode = SearchAll(pToken->String());
	    if (!pNode) {
		pNode = EnterLocal(pToken->String());

		//--Determine the number's type, and set its value into
		//--the symbol table node.
		if (pToken->Type() == tyInteger) {
		    pResultType = pIntegerType;
		    pNode->defn.constant.value.integer =
						pToken->Value().integer;
		}
		else {
		    pResultType = pRealType;
		    pNode->defn.constant.value.real =
						pToken->Value().real;
		}
		SetType(pNode->pType, pResultType);
	    }

	    //--Append the symbol table node handle to the icode.
	    icode.Put(pNode);

	    pResultType = pNode->pType;
	    GetTokenAppend();
	    break;
	}

	case tcString: {

	    //--Search for the string and enter it if necessary.
	    char        *pString = pToken->String();
	    TSymtabNode *pNode   = SearchAll(pString);
	    if (!pNode) {
		pNode   = EnterLocal(pString);
		pString = pNode->String();

		//--Compute the string length (without the quotes).
		//--If the length is 1, the result type is character,
		//--else create a new string type.
		int length  = strlen(pString) - 2;
		pResultType = length == 1 ? pCharType
					  : new TType(length);
		SetType(pNode->pType, pResultType);

		//--Set the character value or string pointer into the
		//--symbol table node.
		if (length == 1) {
		    pNode->defn.constant.value.character = pString[1];
		}
		else {
		    pNode->defn.constant.value.pString = &pString[1];
		}
	    }

	    //--Append the symbol table node handle to the icode.
	    icode.Put(pNode);

	    pResultType = pNode->pType;
	    GetTokenAppend();
	    break;
	}

	case tcNOT:

	    //--The operand type must be boolean.
	    GetTokenAppend();
	    CheckBoolean(ParseFactor());
	    pResultType = pBooleanType;

	    break;

	case tcLParen:

	    //--Parenthesized subexpression:  Call ParseExpression
	    //--                              recursively ...
	    GetTokenAppend();
	    pResultType = ParseExpression();

	    //-- ... and check for the closing right parenthesis.
	    if (token == tcRParen) GetTokenAppend();
	    else                   Error(errMissingRightParen);

	    break;

	default:

	    Error(errInvalidExpression);
	    pResultType = pDummyType;
	    
	    break;
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ParseVariable       Parse a variable, which can be a simple
//                      identifier, an array identifier followed
//                      subscripts, or a record identifier
//                      followed by  fields.
//
//      pId : ptr to the identifier's symbol table node
//
//  Return: ptr to the variable's type object
//--------------------------------------------------------------

TType *TParser::ParseVariable(const TSymtabNode *pId)
{
    TType *pResultType = pId->pType;  // ptr to result type

    //--Check how the variable identifier was defined.
    switch (pId->defn.how) {
	case dcVariable:
	case dcValueParm:
	case dcVarParm:
	case dcFunction:
	case dcUndefined:       break;  // OK

	default:
	    pResultType = pDummyType;
	    Error(errInvalidIdentifierUsage);
	    break;
    }

    GetTokenAppend();

    //-- [ or . : Loop to parse any subscripts and fields.
    int doneFlag = false;
    do {
	switch (token) {

	    case tcLBracket:
		pResultType = ParseSubscripts(pResultType);
		break;

	    case tcPeriod:
		pResultType = ParseField(pResultType);
		break;

	    default:  doneFlag = true;
	}
    } while (!doneFlag);

    return pResultType;
}

//--------------------------------------------------------------
//  ParseSubscripts     Parse a bracketed list of subscript
//                      separated by commas, following an
//                      array variable:
//
//                          [ <expr>, <expr>, ... ]
//
//      pType : ptr to the array's type object
//
//  Return: ptr to the array element's type object
//--------------------------------------------------------------

TType *TParser::ParseSubscripts(const TType *pType)
{
    //--Loop to parse a list of subscripts separated by commas.
    do {
	//-- [ (first) or , (subsequent)
	GetTokenAppend();

	//-- The current variable is an array type.
	if (pType->form == fcArray) {

	    //--The subscript expression must be assignment type
	    //--compatible with the corresponding subscript type.
	    CheckAssignmentTypeCompatible(pType->array.pIndexType,
					  ParseExpression(),
					  errIncompatibleTypes);

	    //--Update the variable's type.
	    pType = pType->array.pElmtType;
	}

	//--No longer an array type, so too many subscripts.
	//--Parse the extra subscripts anyway for error recovery.
	else {
	    Error(errTooManySubscripts);
	    ParseExpression();
	}

    } while (token == tcComma);

    //-- ]
    CondGetTokenAppend(tcRBracket, errMissingRightBracket);

    return (TType *) pType;
}

//--------------------------------------------------------------
//  ParseField          Parse a field following a record
//                      variable:
//
//                          . <id>
//
//      pType : ptr to the record's type object
//
//  Return: ptr to the field's type object
//--------------------------------------------------------------

TType *TParser::ParseField(const TType *pType)
{
    GetTokenAppend();

    if ((token == tcIdentifier) && (pType->form == fcRecord)) {
	TSymtabNode *pFieldId = pType->record.pSymtab->
					Search(pToken->String());
	if (!pFieldId) Error(errInvalidField);
	icode.Put(pFieldId);

	GetTokenAppend();
	return pFieldId ? pFieldId->pType : pDummyType;
    }
    else {
	Error(errInvalidField);
	GetTokenAppend();
	return pDummyType;
    }
}


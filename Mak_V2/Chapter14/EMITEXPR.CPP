//fig 13-6
//  *************************************************************
//  *                                                           *
//  *   C O D E   G E N E R A T O R   (Expressions)             *
//  *                                                           *
//  *   Generating and emit assembly code for expressions.      *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog13-1/emitexpr.cpp                          *
//  *                                                           *
//  *   MODULE:  Code generator                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include "common.h"
#include "codegen.h"

//--------------------------------------------------------------
//  EmitExpression  Emit code for an expression (binary
//                  relational operators = < > <> <= and >= ).
//
//  Return: ptr to expression's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitExpression(void)
{
    TType        *pOperand1Type;  // ptr to first  operand's type
    TType        *pOperand2Type;  // ptr to second operand's type
    TType        *pResultType;    // ptr to result type
    TTokenCode    op;             // operator
    TInstruction  jumpOpcode;     // jump instruction opcode
    int           jumpLabelIndex; // assembly jump label index

    //--Emit code for the first simple expression.
    pResultType = EmitSimpleExpression();

    //--If we now see a relational operator,
    //--emit code for the second simple expression.
    if (TokenIn(token, tlRelOps)) {
	EmitPushOperand(pResultType);
	op            = token;
	pOperand1Type = pResultType->Base();

	GetToken();
	pOperand2Type = EmitSimpleExpression()->Base();

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
	    //--Compare dx (operand 1) to ax (operand 2).
	    Emit1(pop, Reg(dx));
	    Emit2(cmp, Reg(dx), Reg(ax));
	}
	else if ((pOperand1Type == pRealType) ||
		 (pOperand2Type == pRealType)) {

	    //--real    <op> real
	    //--real    <op> integer
	    //--integer <op> real
	    //--Convert the integer operand to real.
	    //--Call _FloatCompare to do the comparison, which
	    //--returns -1 (less), 0 (equal), or +1 (greater).
	    EmitPushOperand(pOperand2Type);
	    EmitPromoteToReal(pOperand1Type, pOperand2Type);

	    Emit1(call, NameLit(FLOAT_COMPARE));
	    Emit2(add,  Reg(sp), IntegerLit(8));
	    Emit2(cmp,  Reg(ax), IntegerLit(0));
	}
	else {

	    //--string <op> string
	    //--Compare the string pointed to by si (operand 1)
	    //--to the string pointed to by di (operand 2).
	    Emit1(pop, Reg(di));
	    Emit1(pop, Reg(si));
	    Emit2(mov, Reg(ax), Reg(ds));
	    Emit2(mov, Reg(es), Reg(ax));
	    Emit0(cld);
	    Emit2(mov, Reg(cx),
		       IntegerLit(pOperand1Type->array.elmtCount));
	    Emit0(repe_cmpsb);
	}

	Emit2(mov, Reg(ax), IntegerLit(1));  // default: load 1 

	switch (op) {
	    case tcLt:    jumpOpcode = jl;   break;
	    case tcLe:    jumpOpcode = jle;  break;
	    case tcEqual: jumpOpcode = je;   break;
	    case tcNe:    jumpOpcode = jne;  break;
	    case tcGe:    jumpOpcode = jge;  break;
	    case tcGt:    jumpOpcode = jg;   break;
	}

	jumpLabelIndex = ++asmLabelIndex;
	Emit1(jumpOpcode, Label(STMT_LABEL_PREFIX, jumpLabelIndex));

	Emit2(sub, Reg(ax), Reg(ax));     // load 0 if false
	EmitStatementLabel(jumpLabelIndex);

	pResultType = pBooleanType;
    }

    return pResultType;
}

//--------------------------------------------------------------
//  EmitSimpleExpression    Emit code for a simple expression
//                          (unary operators + or -
//                          and binary operators + -
//                          and OR).
//
//  Return: ptr to expression's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitSimpleExpression(void)
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

    //--Emit code for the first term.
    pResultType = EmitTerm();

    //--If there was a unary operator, negate in integer value in ax
    //--with the neg instruction, or negate a real value in dx:ax
    //--by calling _FloatNegate.
    if (unaryOp == tcMinus) {
	if (pResultType->Base() == pIntegerType) Emit1(neg, Reg(ax))
	else if (pResultType == pRealType) {
	    EmitPushOperand(pResultType);
	    Emit1(call, NameLit(FLOAT_NEGATE));
	    Emit2(add,  Reg(sp), IntegerLit(4));
	}
    }

    //--Loop to execute subsequent additive operators and terms.
    while (TokenIn(token, tlAddOps)) {
	op = token;
	pResultType = pResultType->Base();
	EmitPushOperand(pResultType);

	GetToken();
	pOperandType = EmitTerm()->Base();

	//--Perform the operation, and push the resulting value
	//--onto the stack.
	if (op == tcOR) {

	    //--boolean OR boolean => boolean
	    //--ax = ax OR dx
	    Emit1(pop, Reg(dx));
	    Emit2(or,  Reg(ax), Reg(dx));
	    pResultType = pBooleanType;
	}
	else if ((pResultType  == pIntegerType) &&
		 (pOperandType == pIntegerType)) {

	    //--integer +|- integer => integer
	    Emit1(pop, Reg(dx));
	    if (op == tcPlus) Emit2(add, Reg(ax), Reg(dx))
	    else {
		Emit2(sub, Reg(dx), Reg(ax));
		Emit2(mov, Reg(ax), Reg(dx));
	    }
	    pResultType = pIntegerType;
	}
	else {

	    //--real    +|- real    => real
	    //--real    +|- integer => real
	    //--integer +|- real    => real
	    //--Convert the integer operand to real and then
	    //--call _FloatAdd or _FloatSubtract.
	    EmitPushOperand(pOperandType);
	    EmitPromoteToReal(pResultType, pOperandType);

	    Emit1(call, NameLit(op == tcPlus ? FLOAT_ADD
					     : FLOAT_SUBTRACT));
	    Emit2(add, Reg(sp), IntegerLit(8));
	    pResultType = pRealType;
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  EmitTerm            Emit code for a term (binary operators
//                      * / DIV tcMOD and AND).
//
//  Return: ptr to term's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitTerm(void)
{
    TType      *pOperandType;  // ptr to operand's type
    TType      *pResultType;   // ptr to result type
    TTokenCode  op;            // operator

    //--Emit code for the first factor.
    pResultType = EmitFactor();

    //--Loop to execute subsequent multiplicative operators and factors.
    while (TokenIn(token, tlMulOps)) {
	op = token;
	pResultType = pResultType->Base();
	EmitPushOperand(pResultType);

	GetToken();
	pOperandType = EmitFactor()->Base();

	//--Perform the operation, and push the resulting value
	//--onto the stack.
	switch (op) {

	    case tcAND: {

		//--boolean AND boolean => boolean
		Emit1(pop, Reg(dx));
		Emit2(and, Reg(ax), Reg(dx));
		pResultType = pBooleanType;
		break;
	    }

	    case tcStar:

		if ((pResultType  == pIntegerType) &&
		    (pOperandType == pIntegerType)) {

		    //--integer * integer => integer
		    //--ax = ax*dx
		    Emit1(pop,  Reg(dx));
		    Emit1(imul, Reg(dx));
		    pResultType = pIntegerType;
		}
		else {

		    //--real    * real    => real
		    //--real    * integer => real
		    //--integer * real    => real
		    //--Convert the integer operand to real
		    //--and then call _FloatMultiply, which
		    //--leaves the value in dx:ax.
		    EmitPushOperand(pOperandType);
		    EmitPromoteToReal(pResultType, pOperandType);

		    Emit1(call, NameLit(FLOAT_MULTIPLY));
		    Emit2(add,  Reg(sp), IntegerLit(8));
		    pResultType = pRealType;
		}
		break;

	    case tcSlash: {

		//--real    / real    => real
		//--real    / integer => real
		//--integer / real    => real
		//--integer / integer => real
		//--Convert any integer operand to real
		//--and then call _FloatDivide, which
		    //--leaves the value in dx:ax.
		EmitPushOperand(pOperandType);
		EmitPromoteToReal(pResultType, pOperandType);

		Emit1(call, NameLit(FLOAT_DIVIDE));
		Emit2(add,  Reg(sp), IntegerLit(8));
		pResultType = pRealType;
		break;
	    }

	    case tcDIV:
	    case tcMOD: {

		//--integer DIV|MOD integer => integer
		//--ax = ax IDIV cx
		Emit2(mov,  Reg(cx), Reg(ax));
		Emit1(pop,  Reg(ax));
		Emit2(sub,  Reg(dx), Reg(dx));
		Emit1(idiv, Reg(cx));
		if (op == tcMOD) Emit2(mov, Reg(ax), Reg(dx));
		pResultType = pIntegerType;
		break;
	    }
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  EmitFactor      Emit code for a factor (identifier, number,
//                  string, NOT <factor>, or parenthesized
//                  subexpression).  An identifier can be
//                  a function, constant, or variable.
//
//  Return: ptr to factor's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitFactor(void)
{
    TType *pResultType;  // ptr to result type

    switch (token) {

	case tcIdentifier: {
	    switch (pNode->defn.how) {

		case dcFunction:
		    pResultType = EmitSubroutineCall(pNode);
		    break;

		case dcConstant:
		    pResultType = EmitConstant(pNode);
		    break;

		default:
		    pResultType = EmitVariable(pNode, false);
		    break;
	    }
	    break;
	}

	case tcNumber: {

	    //--Push the number's integer or real value onto the stack.
	    if (pNode->pType == pIntegerType) {

		//--ax = value
		Emit2(mov, Reg(ax),
		      IntegerLit(pNode->defn.constant.value.integer));
		pResultType = pIntegerType;
	    }
	    else {

		//--dx:ax = value
		EmitLoadFloatLit(pNode);
		pResultType = pRealType;
	    }

	    GetToken();
	    break;
	}

	case tcString: {

	    //--Push either a character or a string address onto the
	    //--runtime stack, depending on the string length.
	    int length = strlen(pNode->String()) - 2;  // skip quotes
	    if (length == 1) {

		//--Character
		//--ah = 0
		//--al = value
		Emit2(mov, Reg(ax),
		      CharLit(pNode->defn.constant.value.character));
		pResultType = pCharType;
	    }
	    else {

		//--String
		//--ax = string address
		EmitPushStringLit(pNode);
		pResultType = pNode->pType;
	    }

	    GetToken();
	    break;
	}

	case tcNOT:

	    //--Emit code for boolean factor and invert its value.
	    //--ax = NOT ax
	    GetToken();
	    EmitFactor();
	    Emit2(xor, Reg(ax), IntegerLit(1));
	    pResultType = pBooleanType;
	    break;

	case tcLParen: {

	    //--Parenthesized subexpression:  Call EmitExpression
	    //--                              recursively.
	    GetToken();  // first token after (
	    pResultType = EmitExpression();
	    GetToken();  // first token after )
	    break;
	}
    }

    return pResultType;
}

//--------------------------------------------------------------
//  EmitConstant        Emit code to load a scalar constant into
//                      ax or dx:ax, or to push a string address
//                      onto the runtime stack.
//
//      pId : ptr to constant identifier's symbol table node
//
//  Return: ptr to constant's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitConstant(TSymtabNode *pId)
{
    TType *pType = pId->pType;

    if (pType == pRealType) {

	//--Real: dx:ax = value
	EmitLoadFloatLit(pId);
    }
    else if (pType == pCharType) {

	//--Character: ax = value
	Emit2(mov, Reg(ax),
		   CharLit(pId->defn.constant.value.character));
    }
    else if (pType->form == fcArray) {

	//--String constant: push string address
	EmitPushStringLit(pId);
    }
    else {

	//--Integer or enumeration: ax = value
	Emit2(mov, Reg(ax),
		   IntegerLit(pId->defn.constant.value.integer));
    }

    GetToken();
    return pType;
}

//--------------------------------------------------------------
//  EmitVariable        Emit code to load a variable's value
//                      ax or dx:ax, or push its address onto
//                      the runtime stack.
//
//      pId         : ptr to variable's symbol table node
//      addressFlag : true to push address, false to load value
//
//  Return: ptr to variable's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitVariable(const TSymtabNode *pId,
				    int addressFlag)
{
    TType *pType = pId->pType;

    //--It's not a scalar, or addressFlag is true, push the
    //--data address onto the stack. Otherwise, load the
    //--data value into ax or dx:ax.
    if (addressFlag || (! pType->IsScalar())) EmitPushAddress(pId);
    else                                      EmitLoadValue(pId);

    GetToken();

    //--If there are any subscripts and field designators,
    //--emit code to evaluate them and modify the address.
    if ((token == tcLBracket) || (token == tcPeriod)) {
	int doneFlag = false;

	do {
	    switch (token) {

		case tcLBracket:
		    pType = EmitSubscripts(pType);
		    break;

		case tcPeriod:
		    pType = EmitField();
		    break;

		default:  doneFlag = true;
	    }
	} while (!doneFlag);

	//--If addresssFlag is false and the variable is scalar,
	//--pop the address off the top of the stack and use it
	//--to load the value into ax or dx:ax.
	if ((!addressFlag) && (pType->IsScalar())) {
	    Emit1(pop, Reg(bx));
	    if (pType == pRealType) {

		//--Read: dx:ax = value
		Emit2(mov, Reg(ax), WordIndirect(bx));
		Emit2(mov, Reg(dx), HighDWordIndirect(bx));
	    }
	    else if (pType->Base() == pCharType) {

		//--Character: al = value
		Emit2(sub, Reg(ax), Reg(ax));
		Emit2(mov, Reg(al), ByteIndirect(bx));
	    }
	    else {

		//--Integer or enumeration: ax = value
		Emit2(mov, Reg(ax), WordIndirect(bx));
	    }
	}
    }

    return pType;
}

//--------------------------------------------------------------
//  EmitSubscripts      Emit code for each subscript expression
//                      to modify the data address at the top of
//                      the runtime stack.
//
//      pType : ptr to array type object
//
//  Return: ptr to subscripted variable's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitSubscripts(const TType *pType)
{
    int minIndex, elmtSize;

    //--Loop to executed subscript lists enclosed in brackets.
    while (token == tcLBracket) {

	//--Loop to execute comma-separated subscript expressions
	//--within a subscript list.
	do {
	    GetToken();
	    EmitExpression();

	    minIndex = pType->array.minIndex;
	    elmtSize = pType->array.pElmtType->size;

	    //--Convert the subscript into an offset by subracting
	    //--the mininum index from it and then multiplying the
	    //--result by the element size.   Add the offset to the
	    //--address at the top of the stack.
	    if (minIndex != 0) Emit2(sub, Reg(ax),
					  IntegerLit(minIndex));
	    if (elmtSize > 1) {
		Emit2(mov,  Reg(dx), IntegerLit(elmtSize));
		Emit1(imul, Reg(dx));
	    }
	    Emit1(pop,  Reg(dx));
	    Emit2(add,  Reg(dx), Reg(ax));
	    Emit1(push, Reg(dx));

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
//  EmitField   Emit code for a field designator to modify the
//              data address at the top of the runtime stack.
//
//  Return: ptr to record field's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitField(void)
{
    GetToken();
    TSymtabNode *pFieldId = pNode;
    int          offset   = pFieldId->defn.data.offset;

    //--Add the field's offset to the data address
    //--if the offset is greater than 0.
    if (offset > 0) {
	Emit1(pop,  Reg(ax));
	Emit2(add,  Reg(ax), IntegerLit(offset));
	Emit1(push, Reg(ax));
    }

    GetToken();
    return pFieldId->pType;
}
//endfig

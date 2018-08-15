//  *************************************************************
//  *                                                           *
//  *   C O D E   G E N E R A T O R   (Standard Routines)       *
//  *                                                           *
//  *   Generating and emit assembly code for calls to the      *
//  *   standard procedures and functions.                      *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog14-1/emitstd.cpp                           *
//  *                                                           *
//  *   MODULE:  Code generator                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "types.h"
#include "buffer.h"
#include "symtab.h"
#include "codegen.h"

//fig 14-18
//--------------------------------------------------------------
//  EmitStandardSubroutineCall  Emit code for a call to a
//                              standard procedure or function.
//
//      pRoutineId : ptr to the subroutine name's symtab node
//
//  Return: ptr to the call's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitStandardSubroutineCall
				(const TSymtabNode *pRoutineId)
{
    switch (pRoutineId->defn.routine.which) {

	case rcRead:
	case rcReadln:   return EmitReadReadlnCall(pRoutineId);

	case rcWrite:
	case rcWriteln:  return EmitWriteWritelnCall(pRoutineId);

	case rcEof:
	case rcEoln:     return EmitEofEolnCall(pRoutineId);

	case rcAbs:
	case rcSqr:      return EmitAbsSqrCall(pRoutineId);

	case rcArctan:
	case rcCos:
	case rcExp:
	case rcLn:
	case rcSin:
	case rcSqrt:     return EmitArctanCosExpLnSinSqrtCall
							(pRoutineId);

	case rcPred:
	case rcSucc:     return EmitPredSuccCall(pRoutineId);

	case rcChr:      return EmitChrCall();
	case rcOdd:      return EmitOddCall();
	case rcOrd:      return EmitOrdCall();

	case rcRound:
	case rcTrunc:    return EmitRoundTruncCall(pRoutineId);

	default:         return pDummyType;
    }
}
//endfig

//fig 14-19
//--------------------------------------------------------------
//  EmitReadReadlnCall          Emit code for a call to read or
//                              readln.
//
//  Return: ptr to the dummy type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitReadReadlnCall(const TSymtabNode *pRoutineId)
{
    //--Actual parameters are optional for readln.
    GetToken();
    if (token == tcLParen) {

	//--Loop to emit code to read each parameter value.
	do {
	    //--Variable
	    GetToken();
	    TSymtabNode *pVarId   = pNode;
	    TType       *pVarType = EmitVariable(pVarId, true)->Base();

	    //--Read the value.
	    if (pVarType == pIntegerType) {
		Emit1(call, NameLit(READ_INTEGER));
		Emit1(pop,  Reg(bx));
		Emit2(mov,  WordIndirect(bx), Reg(ax));
	    }
	    else if (pVarType == pRealType) {
		Emit1(call, NameLit(READ_REAL));
		Emit1(pop,  Reg(bx));
		Emit2(mov,  WordIndirect(bx), Reg(ax));
		Emit2(mov,  HighDWordIndirect(bx), Reg(dx));
	    }
	    else if (pVarType == pCharType) {
		Emit1(call, NameLit(READ_CHAR));
		Emit1(pop,  Reg(bx));
		Emit2(mov,  ByteIndirect(bx), Reg(al));
	    }
	} while (token == tcComma);

	GetToken();  // token after )
    }

    //--Skip the rest of the input line if readln.
    if (pRoutineId->defn.routine.which == rcReadln) {
	Emit1(call, NameLit(READ_LINE));
    }

    return pDummyType;
}
//endfig

//--------------------------------------------------------------
//  EmitWriteWritelnCall        Emit code for a call to write or
//                              writeln.
//
//  Return: ptr to the dummy type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitWriteWritelnCall
				(const TSymtabNode *pRoutineId)
{
    const int defaultFieldWidth = 10;
    const int defaultPrecision  =  2;

    //--Actual parameters are optional for writeln.
    GetToken();
    if (token == tcLParen) {

	//--Loop to emit code for each parameter value.
	do {
	    //--<expr-1>
	    GetToken();
	    TType *pExprType = EmitExpression()->Base();

	    //--Push the scalar value to be written onto the stack.
	    //--A string value is already on the stack.
	    if (pExprType->form != fcArray) {
		EmitPushOperand(pExprType);
	    }

	    if (token == tcColon) {

		//--Field width <expr-2>
		//--Push its value onto the stack.
		GetToken();
		EmitExpression();
		Emit1(push, Reg(ax));

		if (token == tcColon) {

		    //--Precision <expr-3>
		    //--Push its value onto the stack.
		    GetToken();
		    EmitExpression();
		    Emit1(push, Reg(ax));
		}
		else if (pExprType == pRealType) {

		    //--No precision: Push the default precision.
		    Emit2(mov, Reg(ax), IntegerLit(defaultPrecision));
		    Emit1(push, Reg(ax));
		}
	    }
	    else {

		//--No field width: Push the default field width and
		//--                the default precision.
		if (pExprType == pIntegerType) {
		    Emit2(mov,  Reg(ax), IntegerLit(defaultFieldWidth));
		    Emit1(push, Reg(ax));
		}
		else if (pExprType == pRealType) {
		    Emit2(mov,  Reg(ax), IntegerLit(defaultFieldWidth));
		    Emit1(push, Reg(ax));
		    Emit2(mov,  Reg(ax), IntegerLit(defaultPrecision));
		    Emit1(push, Reg(ax));
		}
		else {
		    Emit2(mov,  Reg(ax), IntegerLit(0));
		    Emit1(push, Reg(ax));
		}
	    }

	    //--Emit the code to write the value.
	    if (pExprType == pIntegerType) {
		Emit1(call, NameLit(WRITE_INTEGER));
		Emit2(add,  Reg(sp), IntegerLit(4));
	    }
	    else if (pExprType == pRealType) {
		Emit1(call, NameLit(WRITE_REAL));
		Emit2(add,  Reg(sp), IntegerLit(8));
	    }
	    else if (pExprType == pBooleanType) {
		Emit1(call, NameLit(WRITE_BOOLEAN));
		Emit2(add,  Reg(sp), IntegerLit(4));
	    }
	    else if (pExprType == pCharType) {
		Emit1(call, NameLit(WRITE_CHAR));
		Emit2(add,  Reg(sp), IntegerLit(4));
	    }
	    else {      // string

		//--Push the string length onto the stack.
		Emit2(mov, Reg(ax),
			   IntegerLit(pExprType->array.elmtCount));

		Emit1(push, Reg(ax));
		Emit1(call, NameLit(WRITE_STRING));
		Emit2(add,  Reg(sp), IntegerLit(6));
	    }

	} while (token == tcComma);

	GetToken();  // token after )
    }

    //--End the line if writeln.
    if (pRoutineId->defn.routine.which == rcWriteln) {
	Emit1(call, NameLit(WRITE_LINE));
    }

    return pDummyType;
}

//fig 14-20
//--------------------------------------------------------------
//  EmitEofEolnCall         Emit code for a call to eof or eoln.
//
//  Return: ptr to the boolean type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitEofEolnCall(const TSymtabNode *pRoutineId)
{
    Emit1(call, NameLit(pRoutineId->defn.routine.which == rcEof
			      ? STD_EOF
			      : STD_EOLN));

    GetToken();  // token after function name
    return pBooleanType;
}

//--------------------------------------------------------------
//  EmitAbsSqrCall           Emit code for a call to abs or sqr.
//
//  Return: ptr to the result's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitAbsSqrCall(const TSymtabNode *pRoutineId)
{
    GetToken();  // (
    GetToken();

    TType *pParmType = EmitExpression()->Base();

    switch (pRoutineId->defn.routine.which) {

	case rcAbs:
	    if (pParmType == pIntegerType) {
		int nonNegativeLabelIndex = ++asmLabelIndex;

		Emit2(cmp, Reg(ax), IntegerLit(0));
		Emit1(jge,
		      Label(STMT_LABEL_PREFIX, nonNegativeLabelIndex));
		Emit1(neg, Reg(ax));

		EmitStatementLabel(nonNegativeLabelIndex);
	    }
	    else {
		EmitPushOperand(pParmType);
		Emit1(call, NameLit(STD_ABS));
		Emit2(add,  Reg(sp), IntegerLit(4));
	    }
	    break;

	case rcSqr:
	    if (pParmType == pIntegerType) {
		Emit2(mov,  Reg(dx), Reg(ax));
		Emit1(imul, Reg(dx));
	    }
	    else {
		EmitPushOperand(pParmType);
		EmitPushOperand(pParmType);
		Emit1(call, NameLit(FLOAT_MULTIPLY));
		Emit2(add,  Reg(sp), IntegerLit(8));
	    }
	    break;
	}

    GetToken();  // token after )
    return pParmType;
}

//--------------------------------------------------------------
//  EmitArctanCosExpLnSinSqrtCall       Emit code for a call to
//                                      arctan, cos, exp, ln,
//                                      sin, or sqrt.
//
//  Return: ptr to the real type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitArctanCosExpLnSinSqrtCall
					(const TSymtabNode *pRoutineId)
{
    char *stdFuncName;

    GetToken();  // (
    GetToken();

    //--Evaluate the parameter, and convert an integer value to
    //--real if necessary.
    TType *pParmType = EmitExpression()->Base();
    if (pParmType == pIntegerType) {
	Emit1(push, Reg(ax));
	Emit1(call, NameLit(FLOAT_CONVERT));
	Emit2(add,  Reg(sp), IntegerLit(2));
    }

    EmitPushOperand(pRealType);

    switch (pRoutineId->defn.routine.which) {
	case rcArctan:  stdFuncName = STD_ARCTAN;  break;
	case rcCos:     stdFuncName = STD_COS;     break;
	case rcExp:     stdFuncName = STD_EXP;     break;
	case rcLn:      stdFuncName = STD_LN;      break;
	case rcSin:     stdFuncName = STD_SIN;     break;
	case rcSqrt:    stdFuncName = STD_SQRT;    break;
    }

    Emit1(call, NameLit(stdFuncName));
    Emit2(add,  Reg(sp), IntegerLit(4));

    GetToken();  // token after )
    return pRealType;
}

//--------------------------------------------------------------
//  EmitPredSuccCall            Emit code for a call to pred
//                              or succ.
//
//  Return: ptr to the result's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitPredSuccCall(const TSymtabNode *pRoutineId)
{
    GetToken();  // (
    GetToken();

    TType *pParmType = EmitExpression();

    Emit1(pRoutineId->defn.routine.which == rcPred ? decr : incr,
	  Reg(ax));

    GetToken();  // token after )
    return pParmType;
}

//--------------------------------------------------------------
//  EmitChrCall                 Emit code for a call to chr.
//
//  Return: ptr to the character type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitChrCall(void)
{
    GetToken();  // (
    GetToken();
    EmitExpression();

    GetToken();  // token after )
    return pCharType;
}

//--------------------------------------------------------------
//  EmitOddCall                 Emit code for a call to odd.
//
//  Return: ptr to the boolean type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitOddCall(void)
{
    GetToken();  // (
    GetToken();
    EmitExpression();

    Emit2(and, Reg(ax), IntegerLit(1));

    GetToken();  // token after )
    return pBooleanType;
}

//--------------------------------------------------------------
//  EmitOrdCall                 Emit code for a call to ord.
//
//  Return: ptr to the integer type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitOrdCall(void)
{
    GetToken();  // (
    GetToken();
    EmitExpression();

    GetToken();  // token after )
    return pIntegerType;
}

//--------------------------------------------------------------
//  EmitRoundTruncCall          Emit code for a call to round
//                              or trunc.
//
//  Return: ptr to the integer type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitRoundTruncCall(const TSymtabNode *pRoutineId)
{
    GetToken();  // (
    GetToken();
    EmitExpression();

    EmitPushOperand(pRealType);
    Emit1(call, NameLit(pRoutineId->defn.routine.which == rcRound
			      ? STD_ROUND : STD_TRUNC));
    Emit2(add, Reg(sp), IntegerLit(4));

    GetToken();  // token after )
    return pIntegerType;
}
//endfig

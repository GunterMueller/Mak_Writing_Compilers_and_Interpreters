//fig 10-14
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Standard Routines)                   *
//  *                                                           *
//  *   Execute calls to standard procedures and functions.     *
//  *                                                           *
//  *   CLASSES: TExecutor                                      *
//  *                                                           *
//  *   FILE:    prog10-1/execstd.cpp                           *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1995 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <memory.h>
#include <math.h>
#include <iostream.h>
#include <iomanip.h>
#include "buffer.h"
#include "exec.h"

//--------------------------------------------------------------
//  ExecuteStandardSubroutineCall   Execute a call to a standard
//                                  procedure or function.
//
//      pRoutineId : ptr to the subroutine name's symtab node
//
//  Return: ptr to the call's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteStandardSubroutineCall
				(const TSymtabNode *pRoutineId)
{
    switch (pRoutineId->defn.routine.which) {

	case rcRead:
	case rcReadln:   return ExecuteReadReadlnCall(pRoutineId);

	case rcWrite:
	case rcWriteln:  return ExecuteWriteWritelnCall(pRoutineId);

	case rcEof:
	case rcEoln:     return ExecuteEofEolnCall(pRoutineId);

	case rcAbs:
	case rcSqr:      return ExecuteAbsSqrCall(pRoutineId);

	case rcArctan:
	case rcCos:
	case rcExp:
	case rcLn:
	case rcSin:
	case rcSqrt:     return ExecuteArctanCosExpLnSinSqrtCall
							(pRoutineId);

	case rcPred:
	case rcSucc:     return ExecutePredSuccCall(pRoutineId);

	case rcChr:      return ExecuteChrCall();
	case rcOdd:      return ExecuteOddCall();
	case rcOrd:      return ExecuteOrdCall();

	case rcRound:
	case rcTrunc:    return ExecuteRoundTruncCall(pRoutineId);

	default:         return pDummyType;
    }
}

//--------------------------------------------------------------
//  ExecuteReadReadlnCall       Execute a call to read or
//                              readln.
//
//  Return: ptr to the dummy type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteReadReadlnCall(const TSymtabNode *pRoutineId)
{
    //--Actual parameters are optional for readln.
    GetToken();
    if (token == tcLParen) {

	//--Loop to read each parameter value.
	do {
	    //--Variable
	    GetToken();
	    TSymtabNode *pVarId    = pNode;
	    TType       *pVarType  = ExecuteVariable(pVarId, true);
	    TStackItem  *pVarValue = (TStackItem *) Pop()->address;

	    //--Read the value.
	    if (pVarType->Base() == pIntegerType) {
		cin >> pVarValue->integer;
		RangeCheck(pVarType, pVarValue->integer);
	    }
	    else if (pVarType == pRealType) {
		cin >> pVarValue->real;
	    }
	    else {
		char ch = cin.get();
		if (cin.eof() || (ch == '\n')) ch = ' ';
		pVarValue->character = ch;
		RangeCheck(pVarType, ch);
	    }

	    eofFlag = cin.eof();
	    TraceDataStore(pVarId, pVarValue, pVarType);

	} while (token == tcComma);

	GetToken();  // token after )
    }

    //--Skip the rest of the input line if readln.
    if (pRoutineId->defn.routine.which == rcReadln) {
	char ch;
	do {
	    ch = cin.get();
	    eofFlag = cin.eof();
	} while (!eofFlag && (ch != '\n'));
    }

    return pDummyType;
}

//--------------------------------------------------------------
//  ExecuteWriteWritelnCall     Execute a call to write or
//                              writeln. Actual parms can be:
//
//                                  <expr-1>
//                                  <expr-1>:<expr-2>
//                                  <expr-1>:<expr-2>:<expr-3>
//
//  Return: ptr to the dummy type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteWriteWritelnCall(const TSymtabNode *pRoutineId)
{
    const int defaultFieldWidth = 10;
    const int defaultPrecision  =  2;

    //--Actual parameters are optional for writeln.
    GetToken();
    if (token == tcLParen) {

	//--Loop to write each parameter value.
	do {
	    //--<expr-1>
	    GetToken();
	    TType *pExprType = ExecuteExpression()->Base();

	    //--Set default width and precision.
	    if (pExprType == pIntegerType) {
		cout << setw(defaultFieldWidth);
	    }
	    else if (pExprType == pRealType) {
		cout << setw(defaultFieldWidth)
		     << setprecision(defaultPrecision);
	    }
	    else cout << setw(0);

	    //--Optional field width <expr-2>
	    if (token == tcColon) {
		GetToken();
		ExecuteExpression();
		cout << setw(Pop()->integer);

		//--Optional precision <expr-3>
		if (token == tcColon) {
		    GetToken();
		    ExecuteExpression();
		    cout << setprecision(Pop()->integer);
		}
	    }

	    //--Write the value.
	    if (pExprType == pIntegerType) {
		cout << Pop()->integer;
	    }
	    else if (pExprType == pRealType) {
		cout << Pop()->real;
	    }
	    else if (pExprType == pBooleanType) {
		cout << (Pop()->integer == 0 ? "FALSE" : "TRUE");
	    }
	    else if (pExprType == pCharType) {
		cout << Pop()->character;
	    }
	    else if (pExprType->form == fcArray) {
		char text[maxInputBufferSize];
		int length = pExprType->array.elmtCount;
		memcpy(text, Pop()->address, length);
		text[length] = '\0';
		cout << text;
	    }

	} while (token == tcComma);

	GetToken();  // token after )
    }

    //--End the line if writeln.
    if (pRoutineId->defn.routine.which == rcWriteln) cout << endl;

    return pDummyType;
}

//--------------------------------------------------------------
//  ExecuteEofEolnCall          Execute a call to eof or eoln.
//
//  Return: ptr to the boolean type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteEofEolnCall(const TSymtabNode *pRoutineId)
{
    if (pRoutineId->defn.routine.which == rcEof) {

	//--eof
	Push(eofFlag ? 1 : 0);
    }
    else {

	//--eoln
	if (eofFlag) Push(1);  // end of line when at end of file
	else {
	    char ch = cin.peek();
	    Push(ch == '\n' ? 1 : 0);
	}
    }

    GetToken();  // token after function name
    return pBooleanType;
}

//--------------------------------------------------------------
//  ExecuteAbsSqrCall           Execute a call to abs or sqr.
//                              Integer parm => integer result
//                              Real parm    => real result
//
//  Return: ptr to the result's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteAbsSqrCall(const TSymtabNode *pRoutineId)
{
    int absFlag = pRoutineId->defn.routine.which == rcAbs;

    GetToken();  // (
    GetToken();

    TType *pParmType = ExecuteExpression()->Base();

    if (pParmType == pIntegerType) {
	TOS()->integer = absFlag ? abs(TOS()->integer)
				 : TOS()->integer * TOS()->integer;
    }
    else {
	TOS()->real = absFlag ? float(fabs(TOS()->real))
			      : TOS()->real * TOS()->real;
    }

    GetToken();  // token after )
    return pParmType;
}

//--------------------------------------------------------------
//  ExecuteArctanCosExpLnSinSqrtCall    Execute a call to arctan
//                                      cos, exp, ln, sin, or
//                                      sqrt.
//
//                                  integer parm => real result
//                                  real parm    => real result
//
//  Return: ptr to the real type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteArctanCosExpLnSinSqrtCall
					(const TSymtabNode *pRoutineId)
{
    TRoutineCode which = pRoutineId->defn.routine.which;

    GetToken();  // (
    GetToken();

    //--Evaluate the parameter, and convert an integer value to
    //--real if necessary.
    TType *pParmType = ExecuteExpression()->Base();
    float  parmValue = pParmType == pRealType ? Pop()->real
					      : Pop()->integer;

    //--Check the parameter value.
    if (   ((which == rcLn)   && (parmValue <= 0.0))
	|| ((which == rcSqrt) && (parmValue <  0.0)) ) {
	RuntimeError(rteInvalidFunctionArgument);
    }

    //--Evaluate the function call.
    switch (which) {
	case rcArctan:  Push(float(atan(parmValue)));   break;
	case rcCos:     Push(float(cos (parmValue)));   break;
	case rcExp:     Push(float(exp (parmValue)));   break;
	case rcLn:      Push(float(log (parmValue)));   break;
	case rcSin:     Push(float(sin (parmValue)));   break;
	case rcSqrt:    Push(float(sqrt(parmValue)));   break;
    }

    GetToken();  // token after )
    return pRealType;
}

//--------------------------------------------------------------
//  ExecutePredSuccCall         Execute a call to pred or succ.
//                              Integer parm => integer result
//                              Enum parm    => enum result
//
//  Return: ptr to the result's type object
//--------------------------------------------------------------

TType *TExecutor::ExecutePredSuccCall(const TSymtabNode *pRoutineId)
{
    GetToken();  // (
    GetToken();

    TType *pParmType = ExecuteExpression();
    int    parmValue = Pop()->integer;

    if (pRoutineId->defn.routine.which == rcPred) --parmValue;
    else                                          ++parmValue;
    RangeCheck(pParmType, parmValue);
    Push(parmValue);

    GetToken();  // token after )
    return pParmType;
}

//--------------------------------------------------------------
//  ExecuteChrCall              Execute a call to chr.
//                              Integer parm => character result
//
//  Return: ptr to the character type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteChrCall(void)
{
    GetToken();  // (
    GetToken();
    ExecuteExpression();

    TOS()->integer &= 0xff;

    GetToken();  // token after )
    return pCharType;
}

//--------------------------------------------------------------
//  ExecuteOddCall              Execute a call to odd.
//                              Integer parm => boolean result
//
//  Return: ptr to the boolean type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteOddCall(void)
{
    GetToken();  // (
    GetToken();
    ExecuteExpression();

    TOS()->integer &= 1;

    GetToken();  // token after )
    return pBooleanType;
}

//--------------------------------------------------------------
//  ExecuteOrdCall              Execute a call to ord.
//                              Character parm => integer result
//                              Enum parm      => integer result
//
//  Return: ptr to the integer type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteOrdCall(void)
{
    GetToken();  // (
    GetToken();
    ExecuteExpression();

    GetToken();  // token after )
    return pIntegerType;
}

//--------------------------------------------------------------
//  ExecuteRoundTruncCall       Execute a call to round or
//                              trunc.
//                              Real parm => integer result
//
//  Return: ptr to the integer type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteRoundTruncCall(const TSymtabNode *pRoutineId)
{
    GetToken();  // (
    GetToken();
    ExecuteExpression();

    float parmValue = TOS()->real;

    if (pRoutineId->defn.routine.which == rcRound) {    // round
	if (parmValue > 0.0) TOS()->integer = int(parmValue + 0.5);
	else                 TOS()->integer = int(parmValue - 0.5);
    }
    else TOS()->integer = int(parmValue);               // trunc

    GetToken();  // token after )
    return pIntegerType;
}
//endfig

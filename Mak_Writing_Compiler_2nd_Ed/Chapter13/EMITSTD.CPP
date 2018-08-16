//fig 13-7
//  *************************************************************
//  *                                                           *
//  *   C O D E   G E N E R A T O R   (Standard Routines)       *
//  *                                                           *
//  *   Generating and emit assembly code for calls to the      *
//  *   standard procedures and functions.                      *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog13-1/emitstd.cpp                           *
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
    return EmitWriteWritelnCall(pRoutineId);
}

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
//endfig

//fig 13-5
//  *************************************************************
//  *                                                           *
//  *   C O D E   G E N E R A T O R   (Statements)              *
//  *                                                           *
//  *   Generating and emit assembly code for statements.       *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog13-1/emitstmt.cpp                          *
//  *                                                           *
//  *   MODULE:  Code generator                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "common.h"
#include "codegen.h"

//--------------------------------------------------------------
//  EmitStatement       Emit code for a statement.
//--------------------------------------------------------------

void TCodeGenerator::EmitStatement(void)
{
    //--Emit the current statement as a comment.
    EmitStmtComment();

    switch (token) {

	case tcIdentifier: {
	    if (pNode->defn.how == dcProcedure) {
		EmitSubroutineCall(pNode);
	    }
	    else {
		EmitAssignment(pNode);
	    }
	    break;
	}

	case tcBEGIN:   EmitCompound();      break;
    }
}

//--------------------------------------------------------------
//  EmitStatementList   Emit code for a statement list until
//                      the terminator token.
//
//      terminator : the token that terminates the list
//--------------------------------------------------------------

void TCodeGenerator::EmitStatementList(TTokenCode terminator)
{
    //--Loop to emit code for statements and skip semicolons.
    do {
	EmitStatement();
	while (token == tcSemicolon) GetToken();
    } while (token != terminator);
}

//--------------------------------------------------------------
//  EmitAssignment      Emit code for an assignment statement.
//--------------------------------------------------------------

void TCodeGenerator::EmitAssignment(const TSymtabNode *pTargetId)
{
    TType *pTargetType = pTargetId->pType;
			    // ptr to target type object
    TType *pExprType;       // ptr to expression type object
    int    addressOnStack;  // true if target address has been pushed
			    //   onto the runtime stack

    //--Assignment to a function name.
    if (pTargetId->defn.how == dcFunction) {
	EmitPushReturnValueAddress(pTargetId);
	addressOnStack = true;
	GetToken();
    }

    //--Assignment to a nonscalar, a formal VAR parameter, or to
    //--a nonglobal and nonlocal variable. EmitVariable emits code
    //--that leaves the target address on top of the runtime stack.
    else if ((pTargetId->defn.how == dcVarParm) ||
	     (! pTargetType->IsScalar()) ||
	     ((pTargetId->level > 1)
		    && (pTargetId->level < currentNestingLevel))) {
	pTargetType = EmitVariable(pTargetId, true);
	addressOnStack = true;
    }

    //--Assignment to a global or local scalar. A mov will be emitted
    //--after the code for the expression.
    else {
	GetToken();
	pTargetType = pTargetId->pType;
	addressOnStack = false;
    }

    //--Emit code for the expression.
    GetToken();
    pExprType = EmitExpression();

    //--Emit code to do the assignment.
    if ((pTargetType->Base() == pIntegerType)
	    || (pTargetType->Base()->form == fcEnum)) {
	if (addressOnStack) {
	    Emit1(pop, Reg(bx));
	    Emit2(mov, WordIndirect(bx), Reg(ax));
	}
	else Emit2(mov, Word(pTargetId), Reg(ax));
    }
    else if (pTargetType->Base() == pCharType) {

	//--char := char
	if (addressOnStack) {
	    Emit1(pop, Reg(bx));
	    Emit2(mov, ByteIndirect(bx), Reg(al));
	}
	else Emit2(mov, Byte(pTargetId), Reg(al));
    }
    else if (pTargetType == pRealType) {

	//--real := ...
	if (pExprType == pIntegerType) {

	    //--Convert an integer value to real.
	    Emit1(push, Reg(ax));
	    Emit1(call, NameLit(FLOAT_CONVERT));
	    Emit2(add, Reg(sp), IntegerLit(2));
	}

	//--... real
	if (addressOnStack) {
	    Emit1(pop, Reg(bx));
	    Emit2(mov, WordIndirect(bx), Reg(ax));
	    Emit2(mov, HighDWordIndirect(bx), Reg(dx));
	}
	else {
	    Emit2(mov, Word(pTargetId), Reg(ax));
	    Emit2(mov, HighDWord(pTargetId), Reg(dx));
	}
    }
    else {

	//--array  := array
	//--record := record
	Emit2(mov, Reg(cx), IntegerLit(pTargetType->size));
	Emit1(pop, Reg(si));
	Emit1(pop, Reg(di));
	Emit2(mov, Reg(ax), Reg(ds));
	Emit2(mov, Reg(es), Reg(ax));
	Emit0(cld);
	Emit0(rep_movsb);
    }
}

//--------------------------------------------------------------
//  EmitCompound        Emit code for a compound statement:
//
//                          BEGIN <stmt-list> END
//--------------------------------------------------------------

void TCodeGenerator::EmitCompound(void)
{
    StartComment("BEGIN");
    PutLine();

    //--<stmt-list> END
    GetToken();
    EmitStatementList(tcEND);

    GetToken();

    StartComment("END");
    PutLine();
}
//endfig

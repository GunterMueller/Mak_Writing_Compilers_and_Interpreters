//fig 13-3
//  *************************************************************
//  *                                                           *
//  *   C O D E   G E N E R A T O R   (Routines)                *
//  *                                                           *
//  *   Generating and emit assembly code for declared          *
//  *   procedures and functions.                               *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog13-1/emitrtn.cpp                           *
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
//  EmitProgramPrologue         Emit the program prologue.
//--------------------------------------------------------------

void TCodeGenerator::EmitProgramPrologue(void)
{
    PutLine("\tDOSSEG");
    PutLine("\t.MODEL  small");
    PutLine("\t.STACK  1024");
    PutLine();
    PutLine("\t.CODE");
    PutLine();
    PutLine("\tPUBLIC\t_PascalMain");
    PutLine("\tINCLUDE\tpasextrn.inc");
    PutLine();

    //--Equates for stack frame components.
    sprintf(AsmText(), "%s\t\tEQU\t<WORD PTR [bp+4]>", STATIC_LINK);
    PutLine();
    sprintf(AsmText(), "%s\t\tEQU\t<WORD PTR [bp-4]>", RETURN_VALUE);
    PutLine();
    sprintf(AsmText(), "%s\tEQU\t<WORD PTR [bp-2]>", HIGH_RETURN_VALUE);
    PutLine();
}

//--------------------------------------------------------------
//  EmitProgramEpilogue         Emit the program epilogue.
//--------------------------------------------------------------

void TCodeGenerator::EmitProgramEpilogue(const TSymtabNode *pProgramId)
{
    TSymtabNode *pId;
    TType       *pType;
 
    PutLine();
    PutLine("\t.DATA");
    PutLine();

    //--Emit declarations for the program's global variables.
    for (pId = pProgramId->defn.routine.locals.pVariableIds;
	 pId; pId = pId->next) {
	sprintf(AsmText(), "%s_%03d\t", pId->String(), pId->labelIndex);
	Advance();

	pType = pId->pType;
	if (pType == pCharType) {
	    sprintf(AsmText(), "DB\t0");
	}
	else if (pType == pRealType) {
	    sprintf(AsmText(), "DD\t0.0");
	}
	else if (! pType->IsScalar()) {
	    sprintf(AsmText(), "DB\t%d DUP(0)", pType->size);
	}
	else {  // integer or enumeration
	    sprintf(AsmText(), "DW\t0");
	}

	PutLine();
    }

    //--Emit declarations for the program's floating point literals.
    for (pId = pFloatLitList; pId; pId = pId->next) {
	sprintf(AsmText(), "%s_%03d\tDD\t%e", FLOAT_LABEL_PREFIX,
		pId->labelIndex, pId->defn.constant.value.real);
	PutLine();
    }

    //--Emit declarations for the program's string literals.
    for (pId = pStringLitList; pId; pId = pId->next) {
	int i;
	char *pString = pId->String();
	int length    = strlen(pString) - 2;  // don't count quotes

	sprintf(AsmText(), "%s_%03d\tDB\t\"", STRING_LABEL_PREFIX,
					      pId->labelIndex);
	Advance();

	for (i = 1; i <= length; ++i) Put(pString[i]);
	Put('\"');
	PutLine();
    }

    PutLine();
    sprintf(AsmText(), "\tEND");
    PutLine();
}

//--------------------------------------------------------------
//  EmitMain            Emit code for the main routine.
//--------------------------------------------------------------

void TCodeGenerator::EmitMain(const TSymtabNode *pMainId)
{
    TSymtabNode *pRtnId;

    EmitProgramHeaderComment(pMainId);
    EmitVarDeclComment(pMainId->defn.routine.locals.pVariableIds);

    //--Emit code for nested subroutines.
    for (pRtnId = pMainId->defn.routine.locals.pRoutineIds;
	 pRtnId; pRtnId = pRtnId->next) {
	EmitRoutine(pRtnId);
    }

    //--Switch to main's intermediate code and emit code
    //--for its compound statement.
    pIcode = pMainId->defn.routine.pIcode;
    currentNestingLevel = 1;
    EmitMainPrologue();
    EmitCompound();
    EmitMainEpilogue();
}

//--------------------------------------------------------------
//  EmitMainPrologue    Emit the prologue for the main routine.
//--------------------------------------------------------------

void TCodeGenerator::EmitMainPrologue(void)
{
    PutLine();
    PutLine("_PascalMain\tPROC");
    PutLine();

    Emit1(push, Reg(bp));               // dynamic link
    Emit2(mov,  Reg(bp), Reg(sp));      // new stack frame base
}

//--------------------------------------------------------------
//  EmitMainEpilogue    Emit the epilogue for the main routine.
//--------------------------------------------------------------

void TCodeGenerator::EmitMainEpilogue(void)
{
    PutLine();

    Emit1(pop, Reg(bp));        // restore caller's stack frame
    Emit0(ret);                 // return

    PutLine();
    PutLine("_PascalMain\tENDP");
}

//--------------------------------------------------------------
//  EmitRoutine         Emit code for a procedure or function.
//--------------------------------------------------------------

void TCodeGenerator::EmitRoutine(const TSymtabNode *pRoutineId)
{
    TSymtabNode *pRtnId;

    EmitSubroutineHeaderComment(pRoutineId);

    //--Emit code for the parameters and local variables.
    EmitDeclarations(pRoutineId);

    //--Emit code for nested subroutines.
    for (pRtnId = pRoutineId->defn.routine.locals.pRoutineIds;
	 pRtnId; pRtnId = pRtnId->next) {
	EmitRoutine(pRtnId);
    }

    //--Switch to the routine's intermediate code and emit code
    //--for its compound statement.
    pIcode = pRoutineId->defn.routine.pIcode;
    currentNestingLevel = pRoutineId->level + 1;  // level of locals
    EmitRoutinePrologue(pRoutineId);
    EmitCompound();
    EmitRoutineEpilogue(pRoutineId);
}

//--------------------------------------------------------------
//  EmitRoutinePrologue         Emit the prologue for a
//                              procedure or function.
//--------------------------------------------------------------

void TCodeGenerator::EmitRoutinePrologue(const TSymtabNode *pRoutineId)
{
    PutLine();
    sprintf(AsmText(), "%s_%03d\tPROC",
	    pRoutineId->String(), pRoutineId->labelIndex);
    PutLine();
    PutLine();

    Emit1(push, Reg(bp));               // dynamic link
    Emit2(mov,  Reg(bp), Reg(sp));      // new stack frame base

    //--Allocate stack space for a function's return value.
    if (pRoutineId->defn.how == dcFunction) {
	Emit2(sub, Reg(sp), IntegerLit(4));
    }

    //--Allocate stack space for the local variables.
    if (pRoutineId->defn.routine.totalLocalSize > 0) {
	Emit2(sub, Reg(sp),
	      IntegerLit(pRoutineId->defn.routine.totalLocalSize));
    }
}

//--------------------------------------------------------------
//  EmitRoutineEpilogue         Emit the epilogue for a
//                              procedure or function.
//--------------------------------------------------------------

void TCodeGenerator::EmitRoutineEpilogue(const TSymtabNode *pRoutineId)
{
    PutLine();

    //--Load a function's return value into the ax or dx:ax registers.
    if (pRoutineId->defn.how == dcFunction) {
	Emit2(mov, Reg(ax), NameLit(RETURN_VALUE));
	if (pRoutineId->pType == pRealType) {
	    Emit2(mov, Reg(dx), NameLit(HIGH_RETURN_VALUE));
	}
    }

    Emit2(mov, Reg(sp), Reg(bp));       // cut back to caller's stack
    Emit1(pop, Reg(bp));                // restore caller's stack frame

    Emit1(ret, IntegerLit(pRoutineId->defn.routine.totalParmSize + 2));
					// return and cut back stack

    PutLine();
    sprintf(AsmText(), "%s_%03d\tENDP",
	    pRoutineId->String(), pRoutineId->labelIndex);
    PutLine();
}

//--------------------------------------------------------------
//  EmitSubroutineCall          Emit code for a call to a
//                              procedure or a function.
//
//      pRoutineId : ptr to the subroutine name's symtab node
//
//  Return: ptr to the call's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitSubroutineCall(const TSymtabNode *pRoutineId)
{
    return pRoutineId->defn.routine.which == rcDeclared
		? EmitDeclaredSubroutineCall(pRoutineId)
		: EmitStandardSubroutineCall(pRoutineId);
}

//--------------------------------------------------------------
//  EmitDeclaredSubroutineCall   Emit code for a call to a
//                               declared procedure or function.
//
//      pRoutineId : ptr to the subroutine name's symtab node
//
//  Return: ptr to the call's type object
//--------------------------------------------------------------

TType *TCodeGenerator::EmitDeclaredSubroutineCall
				(const TSymtabNode *pRoutineId)
{
    int oldLevel = currentNestingLevel;    // level of caller
    int newLevel = pRoutineId->level + 1;  // level of callee's locals

    //--Emit code to push the actual parameter values onto the stack.
    GetToken();
    if (token == tcLParen) {
	EmitActualParameters(pRoutineId);
	GetToken();
    }

    //--Push the static link onto the stack.
    if (newLevel == oldLevel + 1) {
	
	//--Calling a routine nested within the caller:
	//--Push pointer to caller's stack frame.
	Emit1(push, Reg(bp));
    }
    else if (newLevel == oldLevel) {
	
	//--Calling another routine at the same level:
	//--Push pointer to stack frame of common parent.
	Emit1(push, NameLit(STATIC_LINK));
    }
    else {      // newLevel < oldLevel

	//--Calling a routine at a lesser level (nested less deeply):
	//--Push pointer to stack frame of nearest common ancestor
	//--(the callee's parent).
	EmitAdjustBP(newLevel - 1);
	Emit1(push, Reg(bp));
	EmitRestoreBP(newLevel - 1);
    }

    Emit1(call, TaggedName(pRoutineId));

    return pRoutineId->pType;
}

//--------------------------------------------------------------
//  EmitActualParameters    Emit code for the actual parameters
//                          of a declared subroutine call.
//
//      pRoutineId : ptr to the subroutine name's symtab node
//--------------------------------------------------------------

void TCodeGenerator::EmitActualParameters(const TSymtabNode *pRoutineId)
{
    TSymtabNode *pFormalId;  // ptr to formal parm's symtab node

    //--Loop to emit code for each actual parameter.
    for (pFormalId = pRoutineId->defn.routine.locals.pParmIds;
	 pFormalId;
	 pFormalId = pFormalId->next) {

	TType *pFormalType = pFormalId->pType;
	GetToken();

	//--VAR parameter: EmitVariable will leave the actual
	//--               parameter's addresss on top of the stack.
	if (pFormalId->defn.how == dcVarParm) {
	    EmitVariable(pNode, true);
	}

	//--Value parameter: Emit code to load a scalar value into
	//--                 ax or dx:ax, or push an array or record
	//--                 address onto the stack.
	else {
	    TType *pActualType = EmitExpression();

	    if (pFormalType == pRealType) {

		//--Real formal parm
		if (pActualType == pIntegerType) {
		    Emit1(push, Reg(ax));
		    Emit1(call, NameLit(FLOAT_CONVERT));
		    Emit2(add,  Reg(sp), IntegerLit(2));
		}
		Emit1(push, Reg(dx));
		Emit1(push, Reg(ax));
	    }
	    else if (! pActualType->IsScalar()) {

		//--Block move onto the stack.  Round the next offset
		//--up to an even number.
		int size   = pActualType->size;
		int offset = size & 1 ? size + 1 : size;

		Emit0(cld);
		Emit1(pop, Reg(si));
		Emit2(sub, Reg(sp), IntegerLit(offset));
		Emit2(mov, Reg(di), Reg(sp));
		Emit2(mov, Reg(cx), IntegerLit(size));
		Emit2(mov, Reg(ax), Reg(ds));
		Emit2(mov, Reg(es), Reg(ax));
		Emit0(rep_movsb);
	    }
	    else {
		Emit1(push, Reg(ax));
	    }
	}
    }
}
//endfig

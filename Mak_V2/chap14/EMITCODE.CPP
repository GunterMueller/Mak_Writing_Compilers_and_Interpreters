//fig 13-2
//  *************************************************************
//  *                                                           *
//  *   E M I T   C O D E   S E Q U E N C E S                   *
//  *                                                           *
//  *   Routines for generating and emitting various assembly   *
//  *   language code sequences.                                *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog13-1/emitcode.c                            *
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
//  Go                          Start the compilation.
//--------------------------------------------------------------

void TCodeGenerator::Go(const TSymtabNode *pProgramId)
{
    EmitProgramPrologue();

    //--Emit code for the program.
    currentNestingLevel = 1;
    EmitMain(pProgramId);

    EmitProgramEpilogue(pProgramId);
}

//--------------------------------------------------------------
//  EmitStatementLabel      Emit a statement label constructed
//                          from the label index.
//
//                          Example:  $L_007:
//
//      index : index value
//--------------------------------------------------------------

void TCodeGenerator::EmitStatementLabel(int index)
{
    sprintf(AsmText(), "%s_%03d:", STMT_LABEL_PREFIX, index);
    PutLine();
}

//              ******************
//              *                *
//              *  Declarations  *
//              *                *
//              ******************

//--------------------------------------------------------------
//  EmitDeclarations    Emit code for the parameter and local
//                      variable declarations of a routine.
//
//      pRoutineId : ptr to the routine's symbol table node
//--------------------------------------------------------------

void TCodeGenerator::EmitDeclarations(const TSymtabNode *pRoutineId)
{
    TSymtabNode *pParmId = pRoutineId->defn.routine.locals.pParmIds;
    TSymtabNode *pVarId  = pRoutineId->defn.routine.locals.pVariableIds;

    EmitVarDeclComment(pRoutineId->defn.routine.locals.pVariableIds);
    PutLine();

    //--Subroutine parameters
    while (pParmId) {
	EmitStackOffsetEquate(pParmId);
	pParmId = pParmId->next;
    }

    //--Variables
    while (pVarId) {
	EmitStackOffsetEquate(pVarId);
	pVarId = pVarId->next;
    }
}

//--------------------------------------------------------------
//  EmitStackOffsetEquate       Emit a stack frame offset equate
//                              for a parameter id or a local
//                              variable id.
//
//                              Examples: parm_007 EQU <pb+6>
//                                        var_008  EQU <bp-10>
//
//      pId : ptr to symbol table node
//--------------------------------------------------------------

void TCodeGenerator::EmitStackOffsetEquate(const TSymtabNode *pId)
{
    char  *pName      = pId->String();
    int    labelIndex = pId->labelIndex;
    int    offset     = pId->defn.data.offset;
    TType *pType      = pId->pType;

    if (pType == pCharType) {
	sprintf(AsmText(), "%s_%03d\tEQU\t<BYTE PTR [bp%+d]>",
			   pName, labelIndex, offset);
    }
    else {
	sprintf(AsmText(), "%s_%03d\tEQU\t<WORD PTR [bp%+d]>",
			   pName, labelIndex, offset);
    }

    PutLine();
}

//              **********************
//              *                    *
//              *  Loads and Pushes  *
//              *                    *
//              **********************

//--------------------------------------------------------------
//  EmitAdjustBP        Emit code to adjust register bp if
//                      necessary to point to the stack frame
//                      of an enclosing subroutine.
//
//      level : nesting level of enclosing subroutine's data
//--------------------------------------------------------------

void TCodeGenerator::EmitAdjustBP(int level)
{
    //--Don't do anything if local or global.
    if ((level == currentNestingLevel) || (level == 1)) return;

    //--Emit code to chase static links.
    Emit2(mov, Reg(cx), Reg(bp));  // save bp in cx
    do {
	Emit2(mov, Reg(bp), NameLit(STATIC_LINK));  // chase
    } while (++level < currentNestingLevel);
}

//--------------------------------------------------------------
//  EmitRestoreBP       Emit code to restore register bp if
//                      necessary to point to the current
//                      stack frame.
//
//      level : nesting level of enclosing subroutine's data
//--------------------------------------------------------------

void TCodeGenerator::EmitRestoreBP(int level)
{
    //--Don't do anything if local or global.
    if ((level == currentNestingLevel) || (level == 1)) return;

    //--Emit code to restore bp.
    Emit2(mov, Reg(bp), Reg(cx));
}

//--------------------------------------------------------------
//  EmitLoadValue       Emit code to load a scalar value
//                      into ax or dx:ax.
//
//      pId : ptr to symbol table node of parm or variable
//--------------------------------------------------------------

void TCodeGenerator::EmitLoadValue(const TSymtabNode *pId)
{
    TType *pType = pId->pType;

    EmitAdjustBP(pId->level);

    if (pId->defn.how == dcVarParm) {
	//--VAR formal parameter.
	//--ax or dx:ax = value the address points to
	Emit2(mov, Reg(bx), Word(pId));
	if (pType == pCharType) {

	    //--Character:  al = value
	    Emit2(sub, Reg(ax), Reg(ax));
	    Emit2(mov, Reg(al), ByteIndirect(bx));
	}
	else if (pType == pRealType) {

	    //--Real: dx:ax = value
	    Emit2(mov, Reg(ax), WordIndirect(bx));
	    Emit2(mov, Reg(ax), HighDWordIndirect(bx));
	}
	else {

	    //--Integer or enumeration: ax = value
	    Emit2(mov, Reg(ax), WordIndirect(bx));
	}
    }
    else {

	//--Load the value into ax or dx:ax.
	if (pType == pCharType) {

	    //--Character:  al = value
	    Emit2(sub, Reg(ax), Reg(ax));
	    Emit2(mov, Reg(al), Byte(pId));
	}
	else if (pType == pRealType) {

	    //--Real: dx:ax = value
	    Emit2(mov, Reg(ax), Word(pId));
	    Emit2(mov, Reg(dx), HighDWord(pId));
	}
	else {

	    //--Integer or enumeration: ax = value
	    Emit2(mov, Reg(ax), Word(pId));
	}
    }

    EmitRestoreBP(pId->level);
}

//--------------------------------------------------------------
//  EmitLoadFloatLit    Emit code to load a float literal into
//                      dx:ax. Append the literal to the float
//                      literal list.
//
//      pNode : ptr to symbol table node of literal
//--------------------------------------------------------------

void TCodeGenerator::EmitLoadFloatLit(TSymtabNode *pNode)
{
    TSymtabNode *pf;

    //--dx:ax = value
    Emit2(mov, Reg(ax),
	       WordLabel(FLOAT_LABEL_PREFIX, pNode->labelIndex));
    Emit2(mov, Reg(dx),
	       HighDWordLabel(FLOAT_LABEL_PREFIX, pNode->labelIndex));

    //--Check if the float is already in the float literal list.
    for (pf = pFloatLitList; pf; pf = pf->next) {
	if (pf == pNode) return;
    }

    //--Append it to the list if it isn't already there.
    pNode->next   = pFloatLitList;
    pFloatLitList = pNode;
}

//--------------------------------------------------------------
//  EmitPushStringLit   Emit code to push the address of a
//                      string literal onto the runtime stack.
//                      Append the literal to the string literal
//                      list.
//
//      pNode : ptr to symbol table node of literal
//--------------------------------------------------------------

void TCodeGenerator::EmitPushStringLit(TSymtabNode *pNode)
{
    TSymtabNode *ps;

    //--ax = addresss of string
    Emit2(lea,  Reg(ax),
		WordLabel(STRING_LABEL_PREFIX, pNode->labelIndex));
    Emit1(push, Reg(ax));

    //--Check if the string is already in the string literal list.
    for (ps = pStringLitList; ps; ps = ps->next) {
	if (ps == pNode) return;
    }

    //--Append it to the list if it isn't already there.
    pNode->next    = pStringLitList;
    pStringLitList = pNode;
}

//--------------------------------------------------------------
//  EmitPushOperand             Emit code to push a scalar
//                              operand value onto the stack.
//
//      pType : ptr to type of value
//--------------------------------------------------------------

void TCodeGenerator::EmitPushOperand(const TType *pType)
{
    if (! pType->IsScalar()) return;

    if (pType == pRealType) Emit1(push, Reg(dx));
    Emit1(push, Reg(ax));
}

//--------------------------------------------------------------
//  EmitPushAddress             Emit code to push an address
//                              onto the stack.
//
//      pId : ptr to symbol table node of parm or variable
//--------------------------------------------------------------

void TCodeGenerator::EmitPushAddress(const TSymtabNode *pId)
{
    int varLevel  = pId->level;
    int isVarParm = pId->defn.how == dcVarParm;

    EmitAdjustBP(varLevel);

    Emit2(isVarParm ? mov : lea, Reg(ax), Word(pId))
    Emit1(push, Reg(ax));

    EmitRestoreBP(varLevel);
}

//--------------------------------------------------------------
//  EmitPushReturnValueAddress      Emit code to push the   
//                                  address of the function
//                                  return value in the
//                                  stack frame.
//
//      pId : ptr to symbol table node of function
//--------------------------------------------------------------

void TCodeGenerator::EmitPushReturnValueAddress(const TSymtabNode *pId)
{
    EmitAdjustBP(pId->level + 1);

    Emit2(lea,  Reg(ax), NameLit(RETURN_VALUE));
    Emit1(push, Reg(ax));

    EmitRestoreBP(pId->level + 1);
}

//--------------------------------------------------------------
//  EmitPromoteToReal        Emit code to convert integer    
//                           operands to real.
//
//      pType1 : ptr to type of first  operand
//      pType2 : ptr to type of second operand
//--------------------------------------------------------------

void TCodeGenerator::EmitPromoteToReal(const TType *pType1,
				       const TType *pType2)
{
    if (pType2 == pIntegerType) {              // xxx_1 integer_2
	Emit1(call, NameLit(FLOAT_CONVERT));
	Emit2(add,  Reg(sp), IntegerLit(2));
	Emit1(push, Reg(dx));
	Emit1(push, Reg(ax));                  // xxx_1 real_2
    }

    if (pType1 == pIntegerType) {              // integer_1 real_2
	Emit1(pop,  Reg(ax));
	Emit1(pop,  Reg(dx));
	Emit1(pop,  Reg(bx));
	Emit1(push, Reg(dx));
	Emit1(push, Reg(ax));
	Emit1(push, Reg(bx));                  // real_2 integer_1 

	Emit1(call, NameLit(FLOAT_CONVERT));
	Emit2(add,  Reg(sp), IntegerLit(2));   // real_2 real_1

	Emit1(pop,  Reg(bx));
	Emit1(pop,  Reg(cx));
	Emit1(push, Reg(dx));
	Emit1(push, Reg(ax));
	Emit1(push, Reg(cx));
	Emit1(push, Reg(bx));                  // real_1 real_2
    }
}
//endfig

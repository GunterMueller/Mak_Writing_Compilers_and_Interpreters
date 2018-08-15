//  *************************************************************
//  *                                                           *
//  *   C O D E   G E N E R A T O R   (Statements)              *
//  *                                                           *
//  *   Generating and emit assembly code for statements.       *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog14-1/emitstmt.cpp                          *
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

//fig 14-1
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

	case tcREPEAT:  EmitREPEAT();        break;
	case tcWHILE:   EmitWHILE();         break;
	case tcFOR:     EmitFOR();           break;
	case tcIF:      EmitIF();            break;
	case tcCASE:    EmitCASE();          break;
	case tcBEGIN:   EmitCompound();      break;
    }
}
//endfig

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

//fig 14-2
//--------------------------------------------------------------
//  EmitREPEAT      Emit code for a REPEAT statement:
//
//                      REPEAT <stmt-list> UNTIL <expr>
//--------------------------------------------------------------

void TCodeGenerator::EmitREPEAT(void)
{
    int stmtListLabelIndex = ++asmLabelIndex;
    int followLabelIndex   = ++asmLabelIndex;

    EmitStatementLabel(stmtListLabelIndex);

    //--<stmt-list> UNTIL
    GetToken();
    EmitStatementList(tcUNTIL);

    EmitStmtComment();

    //--<expr>
    GetToken();
    EmitExpression();

    //--Decide whether or not to branch back to the loop start.
    Emit2(cmp, Reg(ax), IntegerLit(1));
    Emit1(je,  Label(STMT_LABEL_PREFIX, followLabelIndex));
    Emit1(jmp, Label(STMT_LABEL_PREFIX, stmtListLabelIndex));

    EmitStatementLabel(followLabelIndex);
}

//--------------------------------------------------------------
//  EmitWHILE       Emit code for a WHILE statement:
//
//                      WHILE <expr> DO <stmt>
//--------------------------------------------------------------

void TCodeGenerator::EmitWHILE(void)
{
    int exprLabelIndex   = ++asmLabelIndex;
    int stmtLabelIndex   = ++asmLabelIndex;
    int followLabelIndex = ++asmLabelIndex;

    GetToken();
    GetLocationMarker();    // ignored

    EmitStatementLabel(exprLabelIndex);

    //--<expr>
    GetToken();
    EmitExpression();

    Emit2(cmp, Reg(ax), IntegerLit(1));
    Emit1(je,  Label(STMT_LABEL_PREFIX, stmtLabelIndex));
    Emit1(jmp, Label(STMT_LABEL_PREFIX, followLabelIndex));

    EmitStatementLabel(stmtLabelIndex);

    //--DO <stmt>
    GetToken();
    EmitStatement();

    Emit1(jmp, Label(STMT_LABEL_PREFIX, exprLabelIndex));
    EmitStatementLabel(followLabelIndex);
}

//--------------------------------------------------------------
//  EmitIF          Emit code for an IF statement:
//
//                      IF <expr> THEN <stmt-1>
//
//                  or:
//
//                      IF <expr> THEN <stmt-1> ELSE <stmt-2>
//--------------------------------------------------------------

void TCodeGenerator::EmitIF(void)
{
    int trueLabelIndex  = ++asmLabelIndex;
    int falseLabelIndex = ++asmLabelIndex;

    GetToken();
    GetLocationMarker();    // ignored

    //--<expr>
    GetToken();
    EmitExpression();

    Emit2(cmp, Reg(ax), IntegerLit(1));
    Emit1(je,  Label(STMT_LABEL_PREFIX, trueLabelIndex));
    Emit1(jmp, Label(STMT_LABEL_PREFIX, falseLabelIndex));

    EmitStatementLabel(trueLabelIndex);

    StartComment("THEN");
    PutLine();

    //--THEN <stmt-1>
    GetToken();
    EmitStatement();

    if (token == tcELSE) {
	GetToken();
	GetLocationMarker();    // ignored

	int followLabelIndex = ++asmLabelIndex;
	Emit1(jmp, Label(STMT_LABEL_PREFIX, followLabelIndex));

	StartComment("ELSE");
	PutLine();

	EmitStatementLabel(falseLabelIndex);

	GetToken();
	EmitStatement();

	EmitStatementLabel(followLabelIndex);
    }
    else {
	EmitStatementLabel(falseLabelIndex);
    }
}

//--------------------------------------------------------------
//  EmitFOR         Emit code for a FOR statement:
//
//                      FOR <id> := <expr-1> TO|DOWNTO <expr-2>
//                          DO <stmt>
//--------------------------------------------------------------

void TCodeGenerator::EmitFOR(void)
{
    int testLabelIndex      = ++asmLabelIndex;
    int stmtLabelIndex      = ++asmLabelIndex;
    int terminateLabelIndex = ++asmLabelIndex;

    GetToken();
    GetLocationMarker();    // ignored

    //--Get pointers to the control variable and to its type object.
    GetToken();
    TSymtabNode *pControlId   = pNode;
    TType       *pControlType = pNode->pType;

    int charFlag = (pControlType->Base() == pCharType);

    //-- <id> := <expr-1>
    EmitAssignment(pControlId);

    //--TO or DOWNTO
    int toFlag = token == tcTO;

    EmitStatementLabel(testLabelIndex);

    //--<expr-2>
    GetToken();
    EmitExpression();

    if (charFlag) Emit2(cmp, Byte(pControlId), Reg(al))
    else          Emit2(cmp, Word(pControlId), Reg(ax))
    Emit1(toFlag ? jle : jge,
	  Label(STMT_LABEL_PREFIX, stmtLabelIndex));
    Emit1(jmp, Label(STMT_LABEL_PREFIX, terminateLabelIndex));

    EmitStatementLabel(stmtLabelIndex);

    //--DO <stmt>
    GetToken();
    EmitStatement();

    Emit1(toFlag   ? incr : decr,
	  charFlag ? Byte(pControlId) : Word(pControlId));
    Emit1(jmp, Label(STMT_LABEL_PREFIX, testLabelIndex));

    EmitStatementLabel(terminateLabelIndex);

    Emit1(toFlag   ? decr : incr,
	  charFlag ? Byte(pControlId) : Word(pControlId));
}

//--------------------------------------------------------------
//  EmitCASE        Emit code for a CASE statement:
//
//                      CASE <expr> OF
//                          <case-branch> ;
//                          ...
//                      END
//--------------------------------------------------------------

void TCodeGenerator::EmitCASE(void)
{
    int i, j;
    int followLabelIndex = ++asmLabelIndex;

    struct TBranchEntry {
	int labelValue;
	int branchLocation;
	int labelIndex;
    } *pBranchTable;

    //--Get the locations of the token that follows the
    //--CASE statement and of the branch table.
    GetToken();
    int atFollow      = GetLocationMarker();
    GetToken();
    int atBranchTable = GetLocationMarker();

    //--<epxr>
    GetToken();
    TType *pExprType = EmitExpression();

    int labelValue, branchLocation;
    int charFlag = pExprType->Base() == pCharType;

    //--Loop through the branch table in the icode
    //--to count the number of entries.
    int count = 0;
    GoTo(atBranchTable + 1);
    for (;;) {
	GetCaseItem(labelValue, branchLocation);
	if (branchLocation == 0) break;
	else			 ++count;
    }

    //--Make a copy of the branch table.
    pBranchTable = new TBranchEntry[count];
    GoTo(atBranchTable + 1);
    for (i = 0; i < count; ++i) {
	GetCaseItem(labelValue, branchLocation);
	pBranchTable[i].labelValue     = labelValue;
	pBranchTable[i].branchLocation = branchLocation;
    }

    //--Loop through the branch table copy to emit test code.
    for (i = 0; i < count; ++i) {
	int testLabelIndex   = ++asmLabelIndex;
	int branchLabelIndex = ++asmLabelIndex;

	Emit2(cmp, charFlag ? Reg(al) : Reg(ax),
		   IntegerLit(pBranchTable[i].labelValue));
	Emit1(jne, Label(STMT_LABEL_PREFIX, testLabelIndex));

	//--See if the branch location is already in the branch table
	//--copy. If so, reuse the branch label index.
	for (j = 0; j < i; ++j) {
	    if (pBranchTable[j].branchLocation ==
				pBranchTable[i].branchLocation) {
		branchLabelIndex = pBranchTable[j].labelIndex;
		break;
	    }
	}

	Emit1(jmp, Label(STMT_LABEL_PREFIX, branchLabelIndex));
	EmitStatementLabel(testLabelIndex);

	//--Enter the branch label index into the branch table copy
	//--only if it is new; otherwise, enter 0.
	pBranchTable[i].labelIndex = j < i ? 0 : branchLabelIndex;
    }
    Emit1(jmp, Label(STMT_LABEL_PREFIX, followLabelIndex));

    //--Loop through the branch table copy again to emit
    //--branch statement code that hasn't already been emitted.
    for (i = 0; i < count; ++i) if (pBranchTable[i].labelIndex) {
	GoTo(pBranchTable[i].branchLocation);
	EmitStatementLabel(pBranchTable[i].labelIndex);

	GetToken();
	EmitStatement();
	Emit1(jmp, Label(STMT_LABEL_PREFIX, followLabelIndex));
    }

    delete[] pBranchTable;

    GoTo(atFollow);
    GetToken();

    StartComment("END");
    PutLine();

    EmitStatementLabel(followLabelIndex);
}
//endfig

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

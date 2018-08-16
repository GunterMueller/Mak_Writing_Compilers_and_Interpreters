//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Statements)                          *
//  *                                                           *
//  *   Execute statements.                                     *
//  *                                                           *
//  *   CLASSES: TExecutor                                      *
//  *                                                           *
//  *   FILE:    prog9-1/execstmt.cpp                           *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

//fig 9-12
#include <memory.h>
#include "exec.h"

//--------------------------------------------------------------
//  ExecuteStatement    Execute a statement.
//--------------------------------------------------------------

void TExecutor::ExecuteStatement(void)
{
    if (token != tcBEGIN) {
	++stmtCount;
	TraceStatement();
    }

    switch (token) {

	case tcIdentifier: {
	    if (pNode->defn.how == dcProcedure) {
		ExecuteSubroutineCall(pNode);
	    }
	    else {
		ExecuteAssignment(pNode);
	    }
	    break;
	}

	case tcREPEAT:      ExecuteREPEAT();      break;
	case tcBEGIN:       ExecuteCompound();    break;

	case tcWHILE:
	case tcIF:
	case tcFOR:
	case tcCASE:
	    RuntimeError(rteUnimplementedRuntimeFeature);
	    break;
    }
}
//endfig

//--------------------------------------------------------------
//  ExecuteStatementList        Execute a statement list until
//                              the terminator token.
//
//      terminator : the token that terminates the list
//--------------------------------------------------------------

void TExecutor::ExecuteStatementList(TTokenCode terminator)
{
    //--Loop to execute statements and skip semicolons.
    do {
	ExecuteStatement();
	while (token == tcSemicolon) GetToken();
    } while (token != terminator);
}

//fig 9-13
//--------------------------------------------------------------
//  ExecuteAssignment   Execute an assignment statement.
//                      Print the assigned value of the
//                      target variable if it is "output".
//
//      pTargetId : ptr to target's symbol table node
//--------------------------------------------------------------

void TExecutor::ExecuteAssignment(const TSymtabNode *pTargetId)
{
    TStackItem *pTarget;      // runtime stack address of target
    TType      *pTargetType;  // ptr to target type object
    TType      *pExprType;    // ptr to expression type object

    //--Assignment to function name.
    if (pTargetId->defn.how == dcFunction) {
	pTargetType = pTargetId->pType;
	pTarget     = runStack.GetValueAddress(pTargetId);
	GetToken();
    }

    //--Assignment to variable or formal parameter.
    //--ExecuteVariable leaves the target address on
    //--top of the runtime stack.
    else {
	pTargetType = ExecuteVariable(pTargetId, true);
	pTarget     = (TStackItem *) Pop()->address;
    }

    //--Execute the expression and leave its value
    //--on top of the runtime stack.
    GetToken();
    pExprType = ExecuteExpression();

    //--Do the assignment.
    if (pTargetType == pRealType) {
	pTarget->real = pExprType->Base() == pIntegerType
			    ? Pop()->integer  // real := integer
			    : Pop()->real;    // real := real
    }
    else if ((pTargetType->Base() == pIntegerType) ||
	     (pTargetType->Base()->form == fcEnum)) {
	int value = Pop()->integer;
	RangeCheck(pTargetType, value);

	//--integer     := integer
	//--enumeration := enumeration
	pTarget->integer = value;
    }
    else if (pTargetType->Base() == pCharType) {
	char value = Pop()->character;
	RangeCheck(pTargetType, value);

	//--character := character
	pTarget->character = value;
    }
    else {
	void *pSource = Pop()->address;

	//--array  := array
	//--record := record
	memcpy(pTarget, pSource, pTargetType->size);
    }

    TraceDataStore(pTargetId, pTarget, pTargetType);
}
//endfig

//--------------------------------------------------------------
//  ExecuteREPEAT    Execute a repeat statement:
//
//                      REPEAT <stmt-list> UNTIL <expr>
//--------------------------------------------------------------

void TExecutor::ExecuteREPEAT(void)
{
    int atLoopStart = CurrentLocation();  // location of the loop start

    do {
	GetToken();  // first token after REPEAT

	//--<stmt-list> UNTIL
	ExecuteStatementList(tcUNTIL);

	//--<expr>
	GetToken();
	ExecuteExpression();

	//--Decide whether or not to branch back to the loop start.
	if (runStack.Pop()->integer == 0) GoTo(atLoopStart);

    } while (CurrentLocation() == atLoopStart);
}

//--------------------------------------------------------------
//  ExecuteCompound     Execute a compound statement:
//
//                          BEGIN <stmt-list> END
//--------------------------------------------------------------

void TExecutor::ExecuteCompound(void)
{
    GetToken();

    //--<stmt-list> END
    ExecuteStatementList(tcEND);

    GetToken();
}

//fig 6-10
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Statements)                          *
//  *                                                           *
//  *   Execute statements.                                     *
//  *                                                           *
//  *   CLASSES: TExecutor                                      *
//  *                                                           *
//  *   FILE:    prog6-2/execstmt.cpp				*
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include "exec.h"

//--------------------------------------------------------------
//  ExecuteStatement    Execute a statement.
//--------------------------------------------------------------

void TExecutor::ExecuteStatement(void)
{
    if (token != tcBEGIN) ++stmtCount;

    switch (token) {
	case tcIdentifier:  ExecuteAssignment();  break;
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

//--------------------------------------------------------------
//  ExecuteAssignment   Execute an assignment statement.
//                      Print the assigned value of the
//                      target variable if it is "output".
//--------------------------------------------------------------

void TExecutor::ExecuteAssignment(void)
{
    TSymtabNode *pTargetNode = pNode;

    GetToken();  // :=
    GetToken();  // first token of expression

    //--Execute the expression and pop its value into the
    //--target variable's symbol table node.
    ExecuteExpression();
    pTargetNode->value = runStack.Pop();

    //--If the target variable is "output", print its value
    //--preceded by the current source line number.
    if (pTargetNode == pOutputNode) {
	cout << ">> At " << currentLineNumber << ": output = "
	     << pTargetNode->value << endl;
    }
}

//--------------------------------------------------------------
//  ExecuteREPEAT    Execute a REPEAT statement:
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
	if (runStack.Pop() == 0.0) GoTo(atLoopStart);

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
//endfig

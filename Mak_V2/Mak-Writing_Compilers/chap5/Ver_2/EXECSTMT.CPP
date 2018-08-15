//fig 5-28
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Statements)                          *
//  *                                                           *
//  *   Execute statements.             			*
//  *                                                           *
//  *	CLASSES: TExecutor					*
//  *                                                           *
//  *   FILE:    prog5-2/execstmt.cpp				*
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

    //--Only assignment statements for now.
    ExecuteAssignment();
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
//endfig

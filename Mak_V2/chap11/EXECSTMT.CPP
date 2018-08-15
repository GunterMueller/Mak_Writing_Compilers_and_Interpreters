//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Statements)                          *
//  *                                                           *
//  *   Execute statements.                                     *
//  *                                                           *
//  *   CLASSES: TExecutor                                      *
//  *                                                           *
//  *   FILE:    prog10-1/execstmt.cpp                          *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1995 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <memory.h>
#include "exec.h"

//fig 10-12
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

	case tcREPEAT:  ExecuteREPEAT();        break;
	case tcWHILE:   ExecuteWHILE();         break;
	case tcFOR:     ExecuteFOR();           break;
	case tcIF:      ExecuteIF();            break;
	case tcCASE:    ExecuteCASE();          break;
	case tcBEGIN:   ExecuteCompound();      break;
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
	if (runStack.Pop()->integer == 0) GoTo(atLoopStart);

    } while (CurrentLocation() == atLoopStart);
}

//fig 10-13
//--------------------------------------------------------------
//  ExecuteWHILE    Execute a WHILE statement:
//
//                      WHILE <expr> DO <stmt>
//--------------------------------------------------------------

void TExecutor::ExecuteWHILE(void)
{
    GetToken();

    //--Get the location of the token that follows the WHILE statement,
    //--and remember the current location of the boolean expression.
    int atFollow = GetLocationMarker();
    int atExpr   = CurrentLocation();

    //--Loop to evaluate the boolean expression and to execute the
    //--statement if the expression is true.
    int doneFlag = false;
    do {
	//--<expr>
	GetToken();
	ExecuteExpression();

	if (Pop()->integer) {

	    //--True: DO <stmt>
	    //--      Go back to re-evaluate the boolean expression.
	    GetToken();
	    ExecuteStatement();
	    GoTo(atExpr);
	}
	else {

	    //--False: Jump out of the loop.
	    doneFlag = true;
	    GoTo(atFollow);
	}
    } while (!doneFlag);

    GetToken();  // token following the WHILE statement
}

//--------------------------------------------------------------
//  ExecuteIF       Execute an IF statement:
//
//                      IF <expr> THEN <stmt-1>
//
//                  or:
//
//                      IF <expr> THEN <stmt-1> ELSE <stmt-2>
//--------------------------------------------------------------

void TExecutor::ExecuteIF(void)
{
    GetToken();

    //--Get the location of where to go to if <expr> is false.
    int atFalse = GetLocationMarker();

    //--<expr>
    GetToken();
    ExecuteExpression();

    if (Pop()->integer) {

	//--True: THEN <stmt-1>
	GetToken();
	ExecuteStatement();

	//--If there is an ELSE part, jump around it.
	if (token == tcELSE) {
	    GetToken();
	    GoTo(GetLocationMarker());
	    GetToken();  // token following the IF statement
	}
    }
    else {

	//--False: Go to the false location.
	GoTo(atFalse);
	GetToken();

	if (token == tcELSE) {

	    //--ELSE <stmt-2>
	    GetToken();
	    GetLocationMarker();  // skip over location marker
	    GetToken();
	    ExecuteStatement();
	}
    }
}

//--------------------------------------------------------------
//  ExecuteFOR      Execute a FOR statement:
//
//                      FOR <id> := <expr-1> TO|DOWNTO <expr-2>
//                          DO <stmt>
//--------------------------------------------------------------

void TExecutor::ExecuteFOR(void)
{
    GetToken();

    //--Get the location of the token that follows the FOR statement.
    int atFollow = GetLocationMarker();

    //--Get a pointer to the control variable's type object and
    //--a pointer to its value on the runtime stack item.
    GetToken();
    TSymtabNode *pControlId    = pNode;
    TType       *pControlType  = ExecuteVariable(pControlId, true);
    TStackItem  *pControlValue = (TStackItem *) Pop()->address;

    //-- := <expr-1>
    GetToken();
    ExecuteExpression();
    int integerFlag  = (pControlType->Base() == pIntegerType) ||
		       (pControlType->Base()->form == fcEnum);
    int initialValue = integerFlag ? Pop()->integer : Pop()->character;
    int controlValue = initialValue;

    //--TO or DOWNTO
    int delta = token == tcTO ? 1 : -1;

    //--<expr-2>
    GetToken();
    ExecuteExpression();
    int finalValue = integerFlag ? Pop()->integer : Pop()->character;

    //--Remember the current location of the start of the loop.
    int atLoopStart = CurrentLocation();

    //--Execute the loop until the control value
    //--reaches the final value.
    while (   ((delta ==  1) && (controlValue <= finalValue))
	   || ((delta == -1) && (controlValue >= finalValue))) {

	//--Set the control variable's value.
	if (integerFlag) pControlValue->integer   = controlValue;
	else             pControlValue->character = controlValue & 0xFF;
	RangeCheck(pControlType, controlValue);
	TraceDataStore(pControlId, pControlValue, pControlType);

	//--DO <stmt>
	GetToken();
	ExecuteStatement();

	//--Increment or decrement the control value,
	//--and possibly execute <stmt> again.
	controlValue += delta;
	GoTo(atLoopStart);
    }

    //--Jump out of the loop.
    //--The control variable is left with the final value.
    GoTo(atFollow);
    GetToken();  // token following the FOR statement
}

//--------------------------------------------------------------
//  ExecuteCASE     Execute a CASE statement:
//
//                      CASE <expr> OF
//                          <case-branch> ;
//                          ...
//                      END
//--------------------------------------------------------------

void TExecutor::ExecuteCASE(void)
{
    GetToken();

    //--Get the locations of the token that follows the
    //--CASE statement and of the branch table.
    int atFollow = GetLocationMarker();
    GetToken();
    int atBranchTable = GetLocationMarker();

    //--Evaluate the CASE expression.
    GetToken();
    TType *pExprType = ExecuteExpression();
    int    exprValue = (pExprType->Base() == pIntegerType) ||
		       (pExprType->Base()->form == fcEnum)
						? Pop()->integer
						: Pop()->character;

    //--Search the branch table for the expression value.
    int labelValue, branchLocation;
    GoTo(atBranchTable + 1);
    do {
	GetCaseItem(labelValue, branchLocation);
    } while ((exprValue != labelValue) && (branchLocation != 0));

    //--If found, execute the appropriate CASE branch statement,
    //--and go to the statement after the CASE statement.
    if (branchLocation != 0) {
	GoTo(branchLocation);
	GetToken();
	ExecuteStatement();

	GoTo(atFollow);
	GetToken();  // token following the CASE statement
    }
    else RuntimeError(rteInvalidCaseValue);
}
//endfig

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

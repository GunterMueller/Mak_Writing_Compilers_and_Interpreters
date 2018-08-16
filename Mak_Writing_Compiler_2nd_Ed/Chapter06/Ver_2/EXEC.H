//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Header)                              *
//  *                                                           *
//  *   CLASSES: TRuntimeStack, TExecutor                       *
//  *                                                           *
//  *   FILE:    prog6-2/exec.h                                 *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef exec_h
#define exec_h

#include "misc.h"
#include "error.h"
#include "symtab.h"
#include "backend.h"

extern TSymtab globalSymtab;

//--------------------------------------------------------------
//  TRuntimeStack       Runtime stack class.
//--------------------------------------------------------------

class TRuntimeStack {
    enum {stackSize = 32};

    float  stack[stackSize];  // stack elements
    float *tos;               // ptr to the top of the stack

public:
    TRuntimeStack(void) { tos = &stack[-1]; }

    float Pop(void)     { return *tos--; }

    void Push(float value)
    {
	if (tos < &stack[stackSize-1]) *++tos = value;
	else RuntimeError(rteStackOverflow);
    }
};

//--------------------------------------------------------------
//  TExecutor           Executor subclass of TBackend.
//--------------------------------------------------------------

class TExecutor : public TBackend {
    long          stmtCount;  // count of executed statements
    TRuntimeStack runStack;   // the runtime stack

    //--Pointers to the special "input" and "output"
    //--symbol table nodes entered by the parser.
    TSymtabNode *const pInputNode;
    TSymtabNode *const pOutputNode;

    //--Statements.
    void ExecuteStatement(void);
    void ExecuteStatementList(TTokenCode terminator);
    void ExecuteAssignment(void);
    void ExecuteREPEAT(void);
    void ExecuteCompound(void);

    //--Expressions.
    void ExecuteExpression(void);
    void ExecuteSimpleExpression(void);
    void ExecuteTerm  (void);
    void ExecuteFactor(void);

public:
    TExecutor(void)
	: pInputNode (globalSymtab.Search("input")),
	  pOutputNode(globalSymtab.Search("output")) { stmtCount = 0; }

    virtual void Go(void);
};

#endif

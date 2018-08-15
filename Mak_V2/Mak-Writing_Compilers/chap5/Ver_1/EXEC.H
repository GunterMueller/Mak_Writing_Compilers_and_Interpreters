//fig 5-7
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Header)                              *
//  *                                                           *
//  *   CLASSES: TRuntimeStack					*
//  *                                                           *
//  *   FILE:    prog5-1/exec.h                                 *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef exec_h
#define exec_h

#include "common.h"

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
	else AbortTranslation(abortStackOverflow);
    }
};
//endfig

#endif

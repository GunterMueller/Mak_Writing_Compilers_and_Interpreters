//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Header)                              *
//  *                                                           *
//  *   CLASSES: TCommandBuffer, TStackItem, TRuntimeStack,	*
//  *            TBreakpointItem, TWatchItem, TExecutor		*
//  *                                                           *
//  *   FILE:    prog11-1/exec.h                                *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1995 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef exec_h
#define exec_h

#include <string.h>
#include "error.h"
#include "symtab.h"
#include "types.h"
#include "icode.h"
#include "backend.h"
#include "parser.h"

#define COMMAND_PROMPT "Command? "

//fig 11-4
//--------------------------------------------------------------
//  TCommandBuffer     Command buffer subclass of TTextInBuffer.
//--------------------------------------------------------------

class TCommandBuffer : public TTextInBuffer {
    virtual char GetLine(void);

public:
    TCommandBuffer(void)
	: TTextInBuffer("con", abortSourceFileOpenFailed)
    {
	GetLine();
    }
};
//endfig

//--------------------------------------------------------------
//  TStackItem          Item pushed onto the runtime stack.
//--------------------------------------------------------------

union TStackItem {
    int    integer;
    float  real;
    char   character;
    void  *address;
};

//--------------------------------------------------------------
//  TRuntimeStack       Runtime stack class.
//--------------------------------------------------------------

class TRuntimeStack {
    enum {
	stackSize       = 128,
	frameHeaderSize =   5,
    };

    //--Stack frame header
    struct TFrameHeader {
	TStackItem functionValue;
	TStackItem staticLink;
	TStackItem dynamicLink;

	struct {
	    TStackItem icode;
	    TStackItem location;
	} returnAddress;
    };

    TStackItem  stack[stackSize];  // stack items
    TStackItem *tos;               // ptr to the top of the stack
    TStackItem *pFrameBase;        // ptr to current stack frame base

public:
    TRuntimeStack(void);

    void Push(int value)
    {
	if (tos < &stack[stackSize-1]) (++tos)->integer = value;
	else RuntimeError(rteStackOverflow);
    }

    void Push(float value)
    {
	if (tos < &stack[stackSize-1]) (++tos)->real = value;
	else RuntimeError(rteStackOverflow);
    }

    void Push(char value)
    {
	if (tos < &stack[stackSize-1]) (++tos)->character = value;
	else RuntimeError(rteStackOverflow);
    }

    void Push(void *addr)
    {
	if (tos < &stack[stackSize-1]) (++tos)->address = addr;
	else RuntimeError(rteStackOverflow);
    }

    TStackItem *PushFrameHeader(int oldLevel, int newLevel,
				TIcode *pIcode);
    void ActivateFrame(TStackItem *pNewFrameBase, int location);
    void PopFrame     (const TSymtabNode *pRoutineId, TIcode *&pIcode);

    TStackItem *Pop(void)       { return tos--; }
    TStackItem *TOS(void) const { return tos;   }

    void AllocateValue  (const TSymtabNode *pId);
    void DeallocateValue(const TSymtabNode *pId);

    TStackItem *GetValueAddress(const TSymtabNode *pId);
};

//fig 11-7
//--------------------------------------------------------------
//  TBreakpointItem     Breakpoint item class.
//--------------------------------------------------------------

class TBreakpointItem {

    friend class TExecutor;

    TBreakpointItem *next;        // ptr to next item in list
    int              lineNumber;  // breakpoint line number

    TBreakpointItem(TBreakpointItem *&pbp, int number)
    {
	next = pbp;   // insert before *pbp
	pbp  = this;

	lineNumber = number;
    }
};

//--------------------------------------------------------------
//  TWatchItem          Watch item class.
//--------------------------------------------------------------

class TWatchItem {

    friend class TExecutor;

    TWatchItem  *next;       // ptr to next item in list
    TSymtabNode *pId;        // ptr to watched id's symtab node
    int          fetchFlag;  // true to watch data fetches
    int          storeFlag;  // true to watch data stores

    TWatchItem(TWatchItem *&pw, TSymtabNode *pNode,
	       int fFlag, int sFlag)
    {
	next = pw;    // insert before *pw
	pw   = this;

	pId       = pNode;
	fetchFlag = fFlag;
	storeFlag = sFlag;
    }
};
//endfig

//--------------------------------------------------------------
//  TExecutor           Executor subclass of TBackend.
//--------------------------------------------------------------

class TExecutor : public TBackend {
    long          stmtCount;  // count of executed statements
    TRuntimeStack runStack;   // ptr to runtime stack

    int eofFlag;  // true if at end of file, else false

    //--Debugger commands
    TParser         *pCmdParser;       // ptr to command parser
    TToken          *pCmdToken;        // ptr to current command token
    TTokenCode       cmdToken;         // code of current command token
    TBreakpointItem *pBreakpointList;  // ptr to head of breakpt list
    TWatchItem      *pWatchList;       // ptr to head of watch list

    //--Trace flags
    int traceRoutineFlag;    // true to trace routine entry/exit
    int traceStatementFlag;  // true to trace statements
    int traceStoreFlag;      // true to trace data stores
    int traceFetchFlag;      // true to trace data fetches

    //--Interactive debugging flags
    int breakStatementFlag;  // true to break at next statement
    int singleSteppingFlag;  // true if single-stepping

    //--Routines
    void   ExecuteRoutine(const TSymtabNode *pRoutineId);
    void   EnterRoutine  (const TSymtabNode *pRoutineId);
    void   ExitRoutine   (const TSymtabNode *pRoutineId);
    TType *ExecuteSubroutineCall        (const TSymtabNode *pRoutineId);
    TType *ExecuteDeclaredSubroutineCall(const TSymtabNode *pRoutineId);
    TType *ExecuteStandardSubroutineCall(const TSymtabNode *pRoutineId);
    void   ExecuteActualParameters      (const TSymtabNode *pRoutineId);

    //--Standard subroutines
    TType *ExecuteReadReadlnCall  (const TSymtabNode *pRoutineId);
    TType *ExecuteWriteWritelnCall(const TSymtabNode *pRoutineId);
    TType *ExecuteEofEolnCall     (const TSymtabNode *pRoutineId);
    TType *ExecuteAbsSqrCall      (const TSymtabNode *pRoutineId);
    TType *ExecuteArctanCosExpLnSinSqrtCall
				  (const TSymtabNode *pRoutineId);
    TType *ExecutePredSuccCall    (const TSymtabNode *pRoutineId);
    TType *ExecuteChrCall(void);
    TType *ExecuteOddCall(void);
    TType *ExecuteOrdCall(void);
    TType *ExecuteRoundTruncCall  (const TSymtabNode *pRoutineId);

    //--Statements
    void ExecuteStatement(void);
    void ExecuteStatementList(TTokenCode terminator);
    void ExecuteAssignment(const TSymtabNode *pTargetId);
    void ExecuteREPEAT(void);
    void ExecuteWHILE(void);
    void ExecuteIF(void);
    void ExecuteFOR(void);
    void ExecuteCASE(void);
    void ExecuteCompound(void);

    //--Expressions
    TType *ExecuteExpression(void);
    TType *ExecuteSimpleExpression(void);
    TType *ExecuteTerm(void);
    TType *ExecuteFactor(void);
    TType *ExecuteConstant  (const TSymtabNode *pId);
    TType *ExecuteVariable  (const TSymtabNode *pId, int addressFlag);
    TType *ExecuteSubscripts(const TType *pType);
    TType *ExecuteField(void);

    //--Tracing
    void TraceRoutineEntry(const TSymtabNode *pRoutineId);
    void TraceRoutineExit (const TSymtabNode *pRoutineId);
    void TraceStatement(void);
    void TraceDataStore(const TSymtabNode *pTargetId,
			const void        *pDataValue,
			const TType       *pDataType);
    void TraceDataFetch(const TSymtabNode *pId,
			const void        *pDataValue,
			const TType       *pDataType);
    void TraceDataValue(const void        *pDataValue,
			const TType       *pDataType);

    //--Debugging
    void ReadCommand(void);
    void DoCommand(void);
    void PrintStatement(void);
    void PrintLineNumber(void);
    void SetBreakpoint(void);
    void RemoveBreakpoint(void);
    void RemoveBreakpointList(void);
    void SetWatch(int fFlag, int sFlag);
    void RemoveWatch(void);
    void RemoveWatchList(void);
    void ShowValue(void);
    void DoAssignment(void);

    void RecreateStatement(void);
    
    void RangeCheck  (const TType *pTargetType, int value);
    void RuntimeError(TRuntimeErrorCode ec);

    void Push(int    value) { runStack.Push(value); }
    void Push(float  value) { runStack.Push(value); }
    void Push(char   value) { runStack.Push(value); }
    void Push(void  *addr)  { runStack.Push(addr);  }

    TStackItem *Pop(void)       { return runStack.Pop(); }
    TStackItem *TOS(void) const { return runStack.TOS(); }

    void GetCommandToken(void)
    {
	pCmdToken = pCmdParser->GetCommandToken();
	cmdToken  = pCmdToken->Code();
    }

public:

//fig 11-8
    TExecutor(void)
    {
	stmtCount       = 0;
	pCmdParser      = NULL;
	pBreakpointList = NULL;
	pWatchList      = NULL;

	traceRoutineFlag   = false;
	traceStatementFlag = false;
	traceStoreFlag     = false;
	traceFetchFlag     = false;

	breakStatementFlag = false;
	singleSteppingFlag = false;

	listFlag         = false;
	errorArrowOffset = strlen(COMMAND_PROMPT);
    }

   ~TExecutor(void)
    {
	delete pCmdParser;

	RemoveBreakpointList();
	RemoveWatchList();
    }
//endfig

    virtual void Go(const TSymtabNode *pProgramId);
};

#endif

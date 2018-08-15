//fig 9-11
//  *************************************************************
//  *                                                           *
//  *   T R A C E R                                             *
//  *                                                           *
//  *   Print debugging runtime trace messages.                 *
//  *                                                           *
//  *   CLASSES: TExecutor                                      *
//  *                                                           *
//  *   FILE:    prog9-1/tracer.cpp                             *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include "common.h"
#include "buffer.h"
#include "exec.h"

//--------------------------------------------------------------
//  TraceRoutineEntry   Trace the entry into a routine.
//
//      pId : ptr to the routine name's symbol table node
//--------------------------------------------------------------

void TExecutor::TraceRoutineEntry(const TSymtabNode *pRoutineId)
{
    if (traceRoutineFlag) {
	cout << ">> Entering routine " << pRoutineId->String() << endl;
    }
}

//--------------------------------------------------------------
//  TraceRoutineExit    Trace the exit from a routine.
//
//      pId : ptr to the routine name's symbol table node
//--------------------------------------------------------------

void TExecutor::TraceRoutineExit(const TSymtabNode *pRoutineId)
{
    if (traceRoutineFlag) {
	cout << ">> Exiting routine " << pRoutineId->String() << endl;
    }
}

//--------------------------------------------------------------
//  TraceStatement      Trace the execution of a statement.
//--------------------------------------------------------------

void TExecutor::TraceStatement(void)
{
    if (traceStatementFlag) cout << ">>  At " << currentLineNumber
				 << endl;
}

//--------------------------------------------------------------
//  TraceDataStore      Trace the storing of data into a
//                      variable or formal parameter.
//
//      pTargetId  : ptr to the target name's symbol table node
//      pDataValue : ptr to the data value
//      pDataType  : ptr to the data's type object
//--------------------------------------------------------------

void TExecutor::TraceDataStore(const TSymtabNode *pTargetId,
			       const void        *pDataValue,
			       const TType       *pDataType)
{
    if (traceStoreFlag) {
	TFormCode form = pTargetId->pType->form;

	cout << ">>   " << pTargetId->String();
	if      (form == fcArray)  cout << "[*]";
	else if (form == fcRecord) cout << ".*";
	cout << " <== ";

	TraceDataValue(pDataValue, pDataType);
    }
}

//--------------------------------------------------------------
//  TraceDataFetch      Trace the fetching of data from a
//                      variable or formal parameter.
//
//      pId        : ptr to the variable name's symbol table node
//      pDataValue : ptr to the data value
//      pDataType  : ptr to the data's type object
//--------------------------------------------------------------

void TExecutor::TraceDataFetch(const TSymtabNode *pId,
			       const void        *pDataValue,
			       const TType       *pDataType)
{
    if (traceFetchFlag) {
	TFormCode form = pId->pType->form;

	cout << ">>   " << pId->String();
	if      (form == fcArray)  cout << "[*]";
	else if (form == fcRecord) cout << ".*";
	cout << ": ";

	TraceDataValue(pDataValue, pDataType);
    }
}

//--------------------------------------------------------------
//  TraceDataValue      Trace a data value.
//
//      pDataValue : ptr to the data value
//      pDataType  : ptr to the data's type object
//--------------------------------------------------------------

void TExecutor::TraceDataValue(const void  *pDataValue,
			       const TType *pDataType)
{
    char text[maxInputBufferSize];  // text for value

    if (pDataType == pRealType) {
	sprintf(text, "%0.6g", ((TStackItem *) pDataValue)->real);
    }
    else if (pDataType == pCharType) {
	sprintf(text, "'%c'", ((TStackItem *) pDataValue)->character);
    }
    else if (pDataType == pBooleanType) {
	strcpy(text, ((TStackItem *) pDataValue)->integer == 0
						? "false" : "true");
    }
    else if (pDataType->form == fcArray) {
	if (pDataType->array.pElmtType == pCharType) {
	    int length = pDataType->array.elmtCount;
	    memcpy(text + 1, pDataValue, length);
	    text[0]          = '\'';
	    text[length + 1] = '\'';
	    text[length + 2] = '\0';
	}
	else strcpy(text, "<array>");
    }
    else if (pDataType->form == fcRecord) {
	strcpy(text, "<record>");
    }
    else if (pDataType->Base()->form == fcEnum) {
	int count = ((TStackItem *) pDataValue)->integer;
	TSymtabNode *pId = pDataType->Base()->enumeration.pConstIds;
	while (--count >= 0) pId = pId->next;
	strcpy(text, pId->String());
    }
    else {
	sprintf(text, "%d", ((TStackItem *) pDataValue)->integer);
    }

    cout << text << endl;
}
//endfig

//fig 8-28
//  *************************************************************
//  *                                                           *
//  *   P R E T T Y   P R I N T E R   (Routines)                *
//  *                                                           *
//  *   Pretty-print program, procedure, and functions.         *
//  *                                                           *
//  *   CLASSES: TPrettyPrinter                                 *
//  *                                                           *
//  *   FILE:    prog8-2/pprtn.cpp                              *
//  *                                                           *
//  *   MODULE:  Pretty printer                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "pprinter.h"

//--------------------------------------------------------------
//  PrintProgram        Pretty-print a program:
//
//                          PROGRAM <id> (<id-1>, <id-2>, ...);
//                              <block>.
//
//      pProgramId : ptr to the program id's symbol table node
//--------------------------------------------------------------

void TPrettyPrinter::PrintProgram(const TSymtabNode *pProgramId)
{
    Put("PROGRAM ");
    Put(pProgramId->String());  // program name

    //--Print the program parameter list.
    TSymtabNode *pParmId = pProgramId->defn.routine.locals.pParmIds;
    if (pParmId) {
	Put(" (");
	int saveMargin = SetMargin();

	//--Loop to print each program parameter.
	do {
	    Put(pParmId->String());
	    pParmId = pParmId->next;
	    if (pParmId) Put(", ");
	} while (pParmId);

	Put(")");
	ResetMargin(saveMargin);
    }
    PutLine(";");

    //--Print the program's block followed by a period.
    Indent();
    PrintBlock(pProgramId);
    PutLine(".");
    Dedent();
}

//--------------------------------------------------------------
//  PrintSubroutine     Pretty-print a procedure or function.
//
//                          <header>;
//                              <block>;
//
//      pRoutineId : ptr to the routine id's symbol table node
//--------------------------------------------------------------

void TPrettyPrinter::PrintSubroutine(const TSymtabNode *pRoutineId)
{
    PrintSubroutineHeader(pRoutineId);
    PutLine(";");

    //--Print the routine's block followed by a semicolon.
    Indent();
    PrintBlock(pRoutineId);
    PutLine(";");
    Dedent();
}

//--------------------------------------------------------------
//  PrintSubroutineHeader   Pretty-print a procedure or
//                          function header:
//
//                              PROCEDURE <id> <formals>
//                              FUNCTION <id> <formals> : <type>
//
//      pRoutineId : ptr to the routine id's symbol table node
//--------------------------------------------------------------

void TPrettyPrinter::PrintSubroutineHeader
				(const TSymtabNode *pRoutineId)
{
    PutLine(" ");
    Put(pRoutineId->defn.how == dcProcedure ? "PROCEDURE "
					    : "FUNCTION " );

    //--Print the procedure or function name
    //--followed by the formal parameter list.
    Put(pRoutineId->String());
    PrintSubroutineFormals(pRoutineId->defn.routine.locals.pParmIds);

    //--Print a function's return type.
    if (pRoutineId->defn.how == dcFunction) {
	Put(" : ");
	PrintTypeSpec(pRoutineId->pType, false);
    }
}

//--------------------------------------------------------------
//  PrintSubroutineFormals  Pretty-print a procedure or
//                          function formal parameter list:
//
//                              (<id-1>, <id-2>, ... : <type-1>;
//                               VAR <id-3>, ... : <type-2>)
//
//      pParmId : ptr to the head of the formal parm id list
//--------------------------------------------------------------

void TPrettyPrinter::PrintSubroutineFormals(const TSymtabNode *pParmId)
{
    if (!pParmId) return;

    Put(" (");
    int saveMargin = SetMargin();

    //--Loop to print each sublist of parameters with
    //--common definition and type.
    do {
	TDefnCode  commonDefn  = pParmId->defn.how;  // common defn
	TType     *pCommonType = pParmId->pType;     // common type
	int        doneFlag;  // true if sublist done, false if not

	if (commonDefn == dcVarParm) Put("VAR ");

	//--Loop to print the parms in the sublist.
	do {
	    Put(pParmId->String());

	    pParmId  = pParmId->next;
	    doneFlag = (!pParmId) || (commonDefn  != pParmId->defn.how)
				  || (pCommonType != pParmId->pType);
	    if (!doneFlag) Put(", ");
	} while (!doneFlag);

	//--Print the sublist's common type.
	Put(" : ");
	PrintTypeSpec(pCommonType, false);

	if (pParmId) PutLine(";");
    } while (pParmId);

    Put(")");
    ResetMargin(saveMargin);
}

//--------------------------------------------------------------
//  PrintBlock      Pretty-print a program, procedure, or
//                  function block.
//
//      pRoutineId : ptr to the routine id's symbol table node
//--------------------------------------------------------------

void TPrettyPrinter::PrintBlock(const TSymtabNode *pRoutineId)
{
    //--First print the definitions and declarations.
    PrintDeclarations(pRoutineId);
    PutLine(" ");

    //--Then print the statements in the icode.
    pIcode = pRoutineId->defn.routine.pIcode;
    PrintCompound();
}
//endfig

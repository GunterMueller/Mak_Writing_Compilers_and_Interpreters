//fig 8-10
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Routines #1)                             *
//  *                                                           *
//  *   Parse programs, procedures, and functions.              *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog8-1/parsrtn1.cpp                           *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include "common.h"
#include "parser.h"

//--------------------------------------------------------------
//  ParseProgram        Parse a program:
//
//                          <program-header> ; <block> .
//
//  Return: ptr to program id's symbol table node
//--------------------------------------------------------------

TSymtabNode *TParser::ParseProgram(void)
{
    //--<program-header>
    TSymtabNode *pProgramId = ParseProgramHeader();

    //-- ;
    Resync(tlHeaderFollow, tlDeclarationStart, tlStatementStart);
    if (token == tcSemicolon) GetToken();
    else if (TokenIn(token, tlDeclarationStart) ||
	     TokenIn(token, tlStatementStart)) {
	Error(errMissingSemicolon);
    }

    //--<block>
    ParseBlock(pProgramId);
    pProgramId->defn.routine.pSymtab = symtabStack.ExitScope();

    //-- .
    Resync(tlProgramEnd);
    CondGetTokenAppend(tcPeriod, errMissingPeriod);

    return pProgramId;
}

//--------------------------------------------------------------
//  ParseProgramHeader      Parse a program header:
//
//                              PROGRAM <id>
//
//                          or:
//
//                              PROGRAM <id> ( <id-list> )
//
//  Return: ptr to program id's symbol table node
//--------------------------------------------------------------

TSymtabNode *TParser::ParseProgramHeader(void)
{
    TSymtabNode *pProgramId;  // ptr to program id node

    //--PROGRAM
    CondGetToken(tcPROGRAM, errMissingPROGRAM);

    //--<id>
    if (token == tcIdentifier) {
	pProgramId = EnterNewLocal(pToken->String(), dcProgram);
	pProgramId->defn.routine.which               = rcDeclared;
	pProgramId->defn.routine.parmCount           = 0;
	pProgramId->defn.routine.totalParmSize       = 0;
	pProgramId->defn.routine.totalLocalSize      = 0;
	pProgramId->defn.routine.locals.pParmIds     = NULL;
	pProgramId->defn.routine.locals.pConstantIds = NULL;
	pProgramId->defn.routine.locals.pTypeIds     = NULL;
	pProgramId->defn.routine.locals.pVariableIds = NULL;
	pProgramId->defn.routine.locals.pRoutineIds  = NULL;
	pProgramId->defn.routine.pSymtab             = NULL;
	pProgramId->defn.routine.pIcode              = NULL;
	SetType(pProgramId->pType, pDummyType);
	GetToken();
    }
    else Error(errMissingIdentifier);

    //-- ( or ;
    Resync(tlProgProcIdFollow, tlDeclarationStart, tlStatementStart);

    //--Enter the nesting level 1 and open a new scope for the program.
    symtabStack.EnterScope();

    //--Optional ( <id-list> )
    if (token == tcLParen) {
	TSymtabNode *pPrevParmId = NULL;

	//--Loop to parse a comma-separated identifier list.
	do {
	    GetToken();
	    if (token == tcIdentifier) {
		TSymtabNode *pParmId = EnterNewLocal(pToken->String(),
						     dcVarParm);
		SetType(pParmId->pType, pDummyType);
		GetToken();

		//--Link program parm id nodes together.
		if (!pPrevParmId) {
		    pProgramId->defn.routine.locals.pParmIds
				      = pPrevParmId = pParmId;
		}
		else {
		    pPrevParmId->next = pParmId;
		    pPrevParmId       = pParmId;
		}
	    }
	    else Error(errMissingIdentifier);
	} while (token == tcComma);

	//-- )
	Resync(tlFormalParmsFollow,
	       tlDeclarationStart, tlStatementStart);
	CondGetToken(tcRParen, errMissingRightParen);
    }

    return pProgramId;
}

//--------------------------------------------------------------
//  ParseSubroutineDeclarations     Parse procedures and
//                                  function declarations.
//
//      pRoutineId : ptr to symbol table node of the name of the
//                   routine that contains the subroutines
//--------------------------------------------------------------

void TParser::ParseSubroutineDeclarations(TSymtabNode *pRoutineId)
{
    TSymtabNode *pLastId = NULL;  // ptr to last routine id node
				  //   in local list

    //--Loop to parse procedure and function definitions.
    while (TokenIn(token, tlProcFuncStart)) {
	TSymtabNode *pRtnId = ParseSubroutine();

	//--Link the routine's local (nested) routine id nodes together.
	if (!pRoutineId->defn.routine.locals.pRoutineIds) {
	    pRoutineId->defn.routine.locals.pRoutineIds = pRtnId;
	}
	else {
	    pLastId->next = pRtnId;
	}
	pLastId = pRtnId;

	//-- ;
	Resync(tlDeclarationFollow, tlProcFuncStart, tlStatementStart);
	if (token == tcSemicolon) GetToken();
	else if (TokenIn(token, tlProcFuncStart) ||
		 TokenIn(token, tlStatementStart)) {
	    Error(errMissingSemicolon);
	}
    }
}

//--------------------------------------------------------------
//  ParseSubroutine     Parse a subroutine:
//
//                          <subroutine-header> ; <block>
//
//                      or:
//
//                          <subroutine-header> ; forward
//
//  Return: ptr to subroutine id's symbol table node
//--------------------------------------------------------------

TSymtabNode *TParser::ParseSubroutine(void)
{
    //--<routine-header>
    TSymtabNode *pRoutineId = (token == tcPROCEDURE)
					? ParseProcedureHeader()
					: ParseFunctionHeader();

    //-- ;
    Resync(tlHeaderFollow, tlDeclarationStart, tlStatementStart);
    if (token == tcSemicolon) GetToken();
    else if (TokenIn(token, tlDeclarationStart) ||
	     TokenIn(token, tlStatementStart)) {
	Error(errMissingSemicolon);
    }

    //--<block> or forward
    if (stricmp(pToken->String(), "forward") != 0) {
	pRoutineId->defn.routine.which = rcDeclared;
	ParseBlock(pRoutineId);
    }
    else {
	GetToken();
	pRoutineId->defn.routine.which = rcForward;
    }

    pRoutineId->defn.routine.pSymtab = symtabStack.ExitScope();
    return pRoutineId;
}

//--------------------------------------------------------------
//  ParseProcedureHeader    Parse a procedure header:
//
//                              PROCEDURE <id>
//
//                          or:
//
//                              PROCEDURE <id> ( <parm-list> )
//
//  Return: ptr to procedure id's symbol table node
//--------------------------------------------------------------

TSymtabNode *TParser::ParseProcedureHeader(void)
{
    TSymtabNode *pProcId;     // ptr to procedure id node
    int forwardFlag = false;  // true if forwarded, false if not

    GetToken();

    //--<id> : If the procedure id has already been declared in
    //--       this scope, it must have been a forward declaration.
    if (token == tcIdentifier) {
	pProcId = SearchLocal(pToken->String());
	if (!pProcId) {

	    //--Not already declared.
	    pProcId = EnterLocal(pToken->String(), dcProcedure);
	    pProcId->defn.routine.totalLocalSize = 0;
	    SetType(pProcId->pType, pDummyType);
	}
	else if ((pProcId->defn.how == dcProcedure) &&
		 (pProcId->defn.routine.which == rcForward)) {

	    //--Forwarded.
	    forwardFlag = true;
	}
	else Error(errRedefinedIdentifier);

	GetToken();
    }
    else Error(errMissingIdentifier);

    //-- ( or ;
    Resync(tlProgProcIdFollow, tlDeclarationStart, tlStatementStart);

    //--Enter the next nesting level and open a new scope
    //--for the procedure.
    symtabStack.EnterScope();

    //--Optional ( <id-list> ) : If there was a forward declaration,
    //--                         there must not be a parameter list,
    //--                         but if there is, parse it anyway
    //--                         for error recovery.
    if (token == tcLParen) {
	int          parmCount;      // count of formal parms
	int          totalParmSize;  // total byte size of all parms
	TSymtabNode *pParmList = ParseFormalParmList(parmCount,
						     totalParmSize);

	if (forwardFlag) Error(errAlreadyForwarded);
	else {

	    //--Not forwarded.
	    pProcId->defn.routine.parmCount       = parmCount;
	    pProcId->defn.routine.totalParmSize   = totalParmSize;
	    pProcId->defn.routine.locals.pParmIds = pParmList;
	}
    }
    else if (!forwardFlag) {

	//--No parameters and no forward declaration.
	pProcId->defn.routine.parmCount       = 0;
	pProcId->defn.routine.totalParmSize   = 0;
	pProcId->defn.routine.locals.pParmIds = NULL;
    }

    pProcId->defn.routine.locals.pConstantIds = NULL;
    pProcId->defn.routine.locals.pTypeIds     = NULL;
    pProcId->defn.routine.locals.pVariableIds = NULL;
    pProcId->defn.routine.locals.pRoutineIds  = NULL;

    SetType(pProcId->pType, pDummyType);
    return pProcId;
}

//--------------------------------------------------------------
//  ParseFunctionHeader     Parse a function header:
//
//                              FUNCTION <id>
//
//                          or:
//
//                              FUNCTION <id> : <type-id>
//
//                          or:
//
//                              FUNCTION <id> ( <parm-list> )
//                                            : <type-id>
//
//  Return: ptr to function id's symbol table node
//--------------------------------------------------------------

TSymtabNode *TParser::ParseFunctionHeader(void)
{
    TSymtabNode *pFuncId;     // ptr to function id node
    int forwardFlag = false;  // true if forwarded, false if not

    GetToken();

    //--<id> : If the function id has already been declared in
    //--       this scope, it must have been a forward declaration.
    if (token == tcIdentifier) {
	pFuncId = SearchLocal(pToken->String());
	if (!pFuncId) {

	    //--Not already declared.
	    pFuncId = EnterLocal(pToken->String(), dcFunction);
	    pFuncId->defn.routine.totalLocalSize = 0;
	}
	else if ((pFuncId->defn.how == dcFunction) &&
		 (pFuncId->defn.routine.which == rcForward)) {

	    //--Forwarded.
	    forwardFlag = true;
	}
	else Error(errRedefinedIdentifier);

	GetToken();
    }
    else Error(errMissingIdentifier);

    //-- ( or : or ;
    Resync(tlFuncIdFollow, tlDeclarationStart, tlStatementStart);

    //--Enter the next nesting level and open a new scope
    //--for the function.
    symtabStack.EnterScope();

    //--Optional ( <id-list> ) : If there was a forward declaration,
    //--                         there must not be a parameter list,
    //--                         but if there is, parse it anyway
    //--                         for error recovery.
    if (token == tcLParen) {
	int          parmCount;      // count of formal parms
	int          totalParmSize;  // total byte size of all parms
	TSymtabNode *pParmList = ParseFormalParmList(parmCount,
						     totalParmSize);

	if (forwardFlag) Error(errAlreadyForwarded);
	else {

	    //--Not forwarded.
	    pFuncId->defn.routine.parmCount       = parmCount;
	    pFuncId->defn.routine.totalParmSize   = totalParmSize;
	    pFuncId->defn.routine.locals.pParmIds = pParmList;
	}
    }
    else if (!forwardFlag) {

	//--No parameters and no forward declaration.
	pFuncId->defn.routine.parmCount       = 0;
	pFuncId->defn.routine.totalParmSize   = 0;
	pFuncId->defn.routine.locals.pParmIds = NULL;
    }

    pFuncId->defn.routine.locals.pConstantIds = NULL;
    pFuncId->defn.routine.locals.pTypeIds     = NULL;
    pFuncId->defn.routine.locals.pVariableIds = NULL;
    pFuncId->defn.routine.locals.pRoutineIds  = NULL;

    //--Optional <type-id> : If there was a forward declaration,
    //--                     there must not be a type id, but if
    //--                     there is, parse it anyway for error
    //--                     recovery.
    if (!forwardFlag || (token == tcColon)) {
	CondGetToken(tcColon, errMissingColon);
	if (token == tcIdentifier) {
	    TSymtabNode *pTypeId = Find(pToken->String());
	    if (pTypeId->defn.how != dcType) Error(errInvalidType);

	    if (forwardFlag) Error(errAlreadyForwarded);
	    else             SetType(pFuncId->pType, pTypeId->pType);

	    GetToken();
	}
	else {
	    Error(errMissingIdentifier);
	    SetType(pFuncId->pType, pDummyType);
	}
    }

    return pFuncId;
}

//--------------------------------------------------------------
//  ParseBlock      Parse a routine's block:
//
//                      <declarations> <compound-statement>
//
//      pRoutineId : ptr to symbol table node of routine's id
//--------------------------------------------------------------

void TParser::ParseBlock(TSymtabNode *pRoutineId)
{
    //--<declarations>
    ParseDeclarations(pRoutineId);

    //--<compound-statement> : Reset the icode and append BEGIN to it,
    //--                       and then parse the compound statement.
    Resync(tlStatementStart);
    if (token != tcBEGIN) Error(errMissingBEGIN);
    icode.Reset();
    ParseCompound();

    //--Set the program's or routine's icode.
    pRoutineId->defn.routine.pIcode = new TIcode(icode);
}
//endfig

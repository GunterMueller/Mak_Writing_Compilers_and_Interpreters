//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Routines #2)                             *
//  *                                                           *
//  *   Parse formal parameters, procedure and function calls,  *
//  *   and actual parameters.                                  *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog13-1/parsrtn2.cpp                          *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "common.h"
#include "parser.h"

//--------------------------------------------------------------
//  ParseFormalParmList     Parse a formal parameter list:
//
//                              ( VAR <id-list> : <type-id> ;
//                                <id-list> : <type-id> ;
//                                ... )
//
//      count     : ref to count of parameters
//      totalSize : ref to total byte size of all parameters
//
//  Return: ptr to head of parm id symbol table node list
//--------------------------------------------------------------

//fig 12-12
TSymtabNode *TParser::ParseFormalParmList(int &count, int &totalSize)
{
    extern int execFlag;

    TSymtabNode *pParmId;             // ptrs to parm symtab nodes
    TSymtabNode *pFirstId, *pLastId;
    TSymtabNode *pPrevSublistLastId = NULL;
    TSymtabNode *pParmList  = NULL;   // ptr to list of parm nodes
    TDefnCode    parmDefn;            // how a parm is defined
    int          offset = parametersStackFrameOffset;

    count = totalSize = 0;
    GetToken();

    //--Loop to parse a parameter declarations separated by semicolons.
    while ((token == tcIdentifier) || (token == tcVAR)) {
	TType *pParmType;  // ptr to parm's type object

	pFirstId = NULL;

	//--VAR or value parameter?
	if (token == tcVAR) {
	    parmDefn = dcVarParm;
	    GetToken();
	}
	else parmDefn = dcValueParm;

	//--Loop to parse the comma-separated sublist of parameter ids.
	while (token == tcIdentifier) {
	    pParmId = EnterNewLocal(pToken->String(), parmDefn);
	    ++count;
	    if (!pParmList) pParmList = pParmId;

	    //--Link the parm id nodes together.
	    if (!pFirstId) pFirstId = pLastId = pParmId;
	    else {
		pLastId->next = pParmId;
		pLastId       = pParmId;
	    }

	    //-- ,
	    GetToken();
	    Resync(tlIdentifierFollow);
	    if (token == tcComma) {

		//--Saw comma.
		//--Skip extra commas and look for an identifier.
		do {
		    GetToken();
		    Resync(tlIdentifierStart, tlIdentifierFollow);
		    if (token == tcComma) {
			Error(errMissingIdentifier);
		    }
		} while (token == tcComma);
		if (token != tcIdentifier) {
		    Error(errMissingIdentifier);
		}
	    }
	    else if (token == tcIdentifier) Error(errMissingComma);
	}

	//-- :
	Resync(tlSublistFollow, tlDeclarationFollow);
	CondGetToken(tcColon, errMissingColon);

	//--<type-id>
	if (token == tcIdentifier) {
	    TSymtabNode *pTypeId = Find(pToken->String());
	    if (pTypeId->defn.how != dcType) Error(errInvalidType);
	    pParmType = pTypeId->pType;
	    GetToken();
	}
	else {
	    Error(errMissingIdentifier);
	    pParmType = pDummyType;
	}

	if (execFlag) {
	    //--Loop to assign the offset and type to each
	    //--parm id in the sublist.
	    for (pParmId = pFirstId; pParmId; pParmId = pParmId->next) {
		pParmId->defn.data.offset = totalSize++;
		SetType(pParmId->pType, pParmType);
	    }
	}
	else {
	    //--Loop to assign the type to each parm id in the sublist.
	    for (pParmId = pFirstId; pParmId; pParmId = pParmId->next) {
		SetType(pParmId->pType, pParmType);
	    }
	}

	//--Link this sublist to the previous sublist.
	if (pPrevSublistLastId) pPrevSublistLastId->next = pFirstId;
	pPrevSublistLastId = pLastId;

	//-- ; or )
	Resync(tlFormalParmsFollow, tlDeclarationFollow);
	if ((token == tcIdentifier) || (token == tcVAR)) {
	    Error(errMissingSemicolon);
	}
	else while (token == tcSemicolon) GetToken();
    }

    if (!execFlag) {

	//--Assign the offset to each parm id in the entire
	//--formal parameter list in reverse order.
	ReverseNodeList(pParmList);
	for (pParmId = pParmList;
	     pParmId; pParmId = pParmId->next) {
	    pParmId->defn.data.offset = offset;
	    offset += pParmId->defn.how == dcValueParm
			    ? pParmId->pType->size	// data value
			    : sizeof(void *);   	// VAR pointer
	    if (offset & 1) ++offset;	// round up to even
	}
	ReverseNodeList(pParmList);

	totalSize = offset - parametersStackFrameOffset;
    }

    //-- )
    CondGetToken(tcRParen, errMissingRightParen);

    return pParmList;
}
//endfig

//--------------------------------------------------------------
//  ParseSubroutineCall     Parse a call to a declared or a
//                          standard procedure or function.
//
//      pRoutineId    : ptr to routine id's symbol table node
//      parmCheckFlag : true to check parameter, false not to
//
//  Return: ptr to the subroutine's type object
//--------------------------------------------------------------

TType *TParser::ParseSubroutineCall(const TSymtabNode *pRoutineId,
				    int parmCheckFlag)
{
    GetTokenAppend();

    return (pRoutineId->defn.routine.which == rcDeclared) ||
	   (pRoutineId->defn.routine.which == rcForward)  ||
	   !parmCheckFlag
		? ParseDeclaredSubroutineCall(pRoutineId, parmCheckFlag)
		: ParseStandardSubroutineCall(pRoutineId);
}

//--------------------------------------------------------------
//  ParseDeclaredSubroutineCall Parse a call to a declared
//                              procedure or function.
//
//      pRoutineId    : ptr to subroutine id's symbol table node
//      parmCheckFlag : true to check parameter, false not to
//
//  Return: ptr to the subroutine's type object
//--------------------------------------------------------------

TType *TParser::ParseDeclaredSubroutineCall
				(const TSymtabNode *pRoutineId,
				 int parmCheckFlag)
{
    ParseActualParmList(pRoutineId, parmCheckFlag);
    return pRoutineId->pType;
}

//--------------------------------------------------------------
//  ParseActualParmList     Parse an actual parameter list:
//
//                              ( <expr-list> )
//
//      pRoutineId    : ptr to routine id's symbol table node
//      parmCheckFlag : true to check parameter, false not to
//--------------------------------------------------------------

void TParser::ParseActualParmList(const TSymtabNode *pRoutineId,
				  int parmCheckFlag)
{
    TSymtabNode *pFormalId = pRoutineId ? pRoutineId->defn.routine.
						locals.pParmIds
					: NULL;

    //--If there are no actual parameters, there better not be
    //--any formal parameters either.
    if (token != tcLParen) {
	if (parmCheckFlag && pFormalId) Error(errWrongNumberOfParms);
	return;
    }

    //--Loop to parse actual parameter expressions
    //--separated by commas.
    do {
	//-- ( or ,
	GetTokenAppend();

	ParseActualParm(pFormalId, parmCheckFlag);
	if (pFormalId) pFormalId = pFormalId->next;
    } while (token == tcComma);

    //-- )
    CondGetTokenAppend(tcRParen, errMissingRightParen);

    //--There better not be any more formal parameters.
    if (parmCheckFlag && pFormalId) Error(errWrongNumberOfParms);
}

//--------------------------------------------------------------
//  ParseActualParm     Parse an actual parameter.  Make sure it
//                      matches the corresponding formal parm.
//
//      pFormalId     : ptr to the corresponding formal parm
//                      id's symbol table node
//      parmCheckFlag : true to check parameter, false not to
//--------------------------------------------------------------

void TParser::ParseActualParm(const TSymtabNode *pFormalId,
			      int parmCheckFlag)
{
    //--If we're not checking the actual parameters against
    //--the corresponding formal parameters (as during error
    //--recovery), just parse the actual parameter.
    if (!parmCheckFlag) {
	ParseExpression();
	return;
    }

    //--If we've already run out of formal parameter,
    //--we have an error.  Go into error recovery mode and
    //--parse the actual parameter anyway.
    if (!pFormalId) {
	Error(errWrongNumberOfParms);
	ParseExpression();
	return;
    }

    //--Formal value parameter: The actual parameter can be an
    //--                        arbitrary expression that is
    //--                        assignment type compatible with
    //--                        the formal parameter.
    if (pFormalId->defn.how == dcValueParm) {
	CheckAssignmentTypeCompatible(pFormalId->pType,
				      ParseExpression(),
				      errIncompatibleTypes);
    }

    //--Formal VAR parameter: The actual parameter must be a
    //--                      variable of the same type as the
    //--                      formal parameter.
    else if (token == tcIdentifier) {
	TSymtabNode *pActualId = Find(pToken->String());
	icode.Put(pActualId);

	if (pFormalId->pType != ParseVariable(pActualId)) {
	    Error(errIncompatibleTypes);
	}
	Resync(tlExpressionFollow, tlStatementFollow, tlStatementStart);
    }

    //--Error: Parse the actual parameter anyway for error recovery.
    else {
	ParseExpression();
	Error(errInvalidVarParm);
    }
}

//fig 12-13
//--------------------------------------------------------------
//  ReverseNodeList 	Reverse a list of symbol table nodes.
//
//	head : ref to the ptr to the current head of the list
//--------------------------------------------------------------

void TParser::ReverseNodeList(TSymtabNode *&head)
{
	TSymtabNode *prev = NULL;
	TSymtabNode *curr = head;
	TSymtabNode *next;

	//--Reverse the list in place.
	while (curr) {
	    next = curr->next;
	    curr->next = prev;
	    prev = curr;
	    curr = next;
	}

	//--Now point to the new head of the list,
	//--which was formerly its tail.
	head = prev;
}
//endfig



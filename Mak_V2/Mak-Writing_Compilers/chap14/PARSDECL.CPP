//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Declarations)                            *
//  *                                                           *
//  *   Parse constant definitions and variable and record      *
//  *   field declarations.                                     *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog13-1/parsdecl.cpp                          *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "parser.h"

int execFlag = true;	// true for executor back end,
			//   false for code generator back end

//--------------------------------------------------------------
//  ParseDeclarations   Parse any constant definitions, type
//                      definitions, variable declarations, and
//                      procedure and function declarations, in
//                      that order.
//--------------------------------------------------------------

void TParser::ParseDeclarations(TSymtabNode *pRoutineId)
{
    if (token == tcCONST) {
	GetToken();
	ParseConstantDefinitions(pRoutineId);
    }

    if (token == tcTYPE) {
	GetToken();
	ParseTypeDefinitions(pRoutineId);
    }

    if (token == tcVAR) {
	GetToken();
	ParseVariableDeclarations(pRoutineId);
    }

    if (TokenIn(token, tlProcFuncStart)) {
	ParseSubroutineDeclarations(pRoutineId);
    }
}

//              **************************
//              *                        *
//              *  Constant Definitions  *
//              *                        *
//              **************************

//--------------------------------------------------------------
//  ParseConstDefinitions   Parse a list of constant definitions
//                          separated by semicolons:
//
//                              <id> = <constant>
//
//      pRoutineId : ptr to symbol table node of program,
//                   procedure, or function identifier
//--------------------------------------------------------------

void TParser::ParseConstantDefinitions(TSymtabNode *pRoutineId)
{
    TSymtabNode *pLastId = NULL;  // ptr to last constant id node
				  //   in local list

    //--Loop to parse a list of constant definitions
    //--seperated by semicolons.
    while (token == tcIdentifier) {

	//--<id>
	TSymtabNode *pConstId = EnterNewLocal(pToken->String());

	//--Link the routine's local constant id nodes together.
	if (!pRoutineId->defn.routine.locals.pConstantIds) {
	    pRoutineId->defn.routine.locals.pConstantIds = pConstId;
	}
	else {
	    pLastId->next = pConstId;
	}
	pLastId = pConstId;

	//-- =
	GetToken();
	CondGetToken(tcEqual, errMissingEqual);

	//--<constant>
	ParseConstant(pConstId);
	pConstId->defn.how = dcConstant;

	//-- ;
	Resync(tlDeclarationFollow, tlDeclarationStart,
	       tlStatementStart);
	CondGetToken(tcSemicolon, errMissingSemicolon);

	//--Skip extra semicolons.
	while (token == tcSemicolon) GetToken();
	Resync(tlDeclarationFollow, tlDeclarationStart,
	       tlStatementStart);
    }
}

//--------------------------------------------------------------
//  ParseConstant       Parse a constant.
//
//      pConstId : ptr to symbol table node of the identifier
//                 being defined
//--------------------------------------------------------------

void TParser::ParseConstant(TSymtabNode *pConstId)
{
    TTokenCode sign = tcDummy;  // unary + or - sign, or none

    //--Unary + or -
    if (TokenIn(token, tlUnaryOps)) {
	if (token == tcMinus) sign = tcMinus;
	GetToken();
    }

    switch (token) {

	//--Numeric constant:  Integer or real type.
	case tcNumber:
	    if (pToken->Type() == tyInteger) {
		pConstId->defn.constant.value.integer =
		    sign == tcMinus ? -pToken->Value().integer
				    :  pToken->Value().integer;
		SetType(pConstId->pType, pIntegerType);
	    }
	    else {
		pConstId->defn.constant.value.real =
		    sign == tcMinus ? -pToken->Value().real
				    :  pToken->Value().real;
		SetType(pConstId->pType, pRealType);
	    }

	    GetToken();
	    break;

	//--Identifier constant
	case tcIdentifier:
	    ParseIdentifierConstant(pConstId, sign);
	    break;

	//--String constant:  Character or string
	//--                  (character array) type.
	case tcString:
	    int length = strlen(pToken->String()) - 2;  // skip quotes

	    if (sign != tcDummy) Error(errInvalidConstant);

	    //--Single character
	    if (length == 1) {
		pConstId->defn.constant.value.character =
						pToken->String()[1];
		SetType(pConstId->pType, pCharType);
	    }

	    //--String (character array):  Create a new unnamed
	    //--                           string type.
	    else {
		char *pString = new char[length];
		CopyQuotedString(pString, pToken->String());
		pConstId->defn.constant.value.pString = pString;
		SetType(pConstId->pType, new TType(length));
	    }

	    GetToken();
	    break;
    }
}

//--------------------------------------------------------------
//  ParseIdentifierConstant     In a constant definition of the
//                              form
//
//                                      <id-1> = <id-2>
//
//                              parse <id-2>. The type can be
//                              integer, real, character,
//                              enumeration, or string
//                              (character array).
//
//      pId1 : ptr to symbol table node of <id-1>
//      sign : unary + or - sign, or none
//--------------------------------------------------------------

void TParser::ParseIdentifierConstant(TSymtabNode *pId1,
				      TTokenCode sign)
{
    TSymtabNode *pId2 = Find(pToken->String());  // ptr to <id-2>

    if (pId2->defn.how != dcConstant) {
	Error(errNotAConstantIdentifier);
	SetType(pId1->pType, pDummyType);
	GetToken();
	return;
    }

    //--Integer identifier
    if (pId2->pType == pIntegerType) {
	pId1->defn.constant.value.integer =
	    sign == tcMinus ? -pId2->defn.constant.value.integer
			    :  pId2->defn.constant.value.integer;
	SetType(pId1->pType, pIntegerType);
    }

    //--Real identifier
    else if (pId2->pType == pRealType) {
	pId1->defn.constant.value.real =
	    sign == tcMinus ? -pId2->defn.constant.value.real
			    :  pId2->defn.constant.value.real;
	SetType(pId1->pType, pRealType);
    }

    //--Character identifier:  No unary sign allowed.
    else if (pId2->pType == pCharType) {
	if (sign != tcDummy) Error(errInvalidConstant);

	pId1->defn.constant.value.character =
			pId2->defn.constant.value.character;
	SetType(pId1->pType, pCharType);
    }

    //--Enumeration identifier:  No unary sign allowed.
    else if (pId2->pType->form == fcEnum) {
	if (sign != tcDummy) Error(errInvalidConstant);

	pId1->defn.constant.value.integer =
			pId2->defn.constant.value.integer;
	SetType(pId1->pType, pId2->pType);
    }

    //--Array identifier:  Must be character array, and
    //                     no unary sign allowed.
    else if (pId2->pType->form == fcArray) {
	if ((sign != tcDummy) ||
	    (pId2->pType->array.pElmtType != pCharType)) {
	    Error(errInvalidConstant);
	}

	pId1->defn.constant.value.pString =
			pId2->defn.constant.value.pString;
	SetType(pId1->pType, pId2->pType);
    }

    GetToken();
}

//      ********************************************
//      *                                          *
//      *  Variable and Record Field Declarations  *
//      *                                          *
//      ********************************************

//--------------------------------------------------------------
//  ParseVariableDeclarations   Parse variable declarations.
//
//      pRoutineId : ptr to symbol table node of program,
//                   procedure, or function identifier
//--------------------------------------------------------------

//fig 12-10
void TParser::ParseVariableDeclarations(TSymtabNode *pRoutineId)
{
    if (execFlag) {
	ParseVarOrFieldDecls(pRoutineId, NULL,
			     pRoutineId->defn.routine.parmCount);
    }
    else {
	ParseVarOrFieldDecls(pRoutineId, NULL,
			     pRoutineId->defn.how == dcProcedure
				? procLocalsStackFrameOffset
				: funcLocalsStackFrameOffset);
    }
}
//endfig

//--------------------------------------------------------------
//  ParseFieldDeclarations      Parse a list record field
//                              declarations.
//
//      pRecordType : ptr to record type object
//      offet       : byte offset within record
//--------------------------------------------------------------

void TParser::ParseFieldDeclarations(TType *pRecordType, int offset)
{
    ParseVarOrFieldDecls(NULL, pRecordType, offset);
}

//--------------------------------------------------------------
//  ParseVarOrFieldDecls        Parse a list of variable or
//                              record field declarations
//                              separated by semicolons:
//
//                                  <id-sublist> : <type>
//
//      pRoutineId  : ptr to symbol table node of program,
//                    procedure, or function identifier, or NULL
//      pRecordType : ptr to record type object, or NULL
//      offset      : variable: runtime stack offset
//                    field: byte offset within record
//--------------------------------------------------------------

//fig 12-11
void TParser::ParseVarOrFieldDecls(TSymtabNode *pRoutineId,
				   TType       *pRecordType,
				   int offset)
{
    TSymtabNode *pId, *pFirstId, *pLastId;   // ptrs to symtab nodes
    TSymtabNode *pPrevSublistLastId = NULL;  // ptr to last node of
					     //   previous sublist
    int          totalSize = 0;              // total byte size of
					     //   local variables

    //--Loop to parse a list of variable or field declarations
    //--separated by semicolons.
    while (token == tcIdentifier) {

	//--<id-sublist>
	pFirstId = ParseIdSublist(pRoutineId, pRecordType, pLastId);

	//-- :
	Resync(tlSublistFollow, tlDeclarationFollow);
	CondGetToken(tcColon, errMissingColon);

	//--<type>
	TType *pType = ParseTypeSpec();

	//--Now loop to assign the type and offset to each
	//--identifier in the sublist.
	for (pId = pFirstId; pId; pId = pId->next) {
	    SetType(pId->pType, pType);

	    if (pRoutineId) {

		//--Variables
		if (execFlag) {
		    pId->defn.data.offset = offset++;
		}
		else {
		    offset -= pType->size;
		    pId->defn.data.offset = offset;
		}
		totalSize += pType->size;
	    }
	    else {

		//--Record fields
		pId->defn.data.offset = offset;
		offset += pType->size;
	    }
	}

	if (pFirstId) {

	    //--Set the first sublist into the routine id's symtab node.
	    if (pRoutineId &&
		(!pRoutineId->defn.routine.locals.pVariableIds)) {
		pRoutineId->defn.routine.locals.pVariableIds = pFirstId;
	    }

	    //--Link this list to the previous sublist.
	    if (pPrevSublistLastId) pPrevSublistLastId->next = pFirstId;
	    pPrevSublistLastId = pLastId;
	}

	//-- ;   for variable and record field declaration, or
	//-- END for record field declaration
	if (pRoutineId) {
	    Resync(tlDeclarationFollow, tlStatementStart);
	    CondGetToken(tcSemicolon, errMissingSemicolon);

	    //--Skip extra semicolons.
	    while (token == tcSemicolon) GetToken();
	    Resync(tlDeclarationFollow, tlDeclarationStart,
		   tlStatementStart);
	}
	else {
	    Resync(tlFieldDeclFollow);
	    if (token != tcEND) {
		CondGetToken(tcSemicolon, errMissingSemicolon);

		//--Skip extra semicolons.
		while (token == tcSemicolon) GetToken();
		Resync(tlFieldDeclFollow, tlDeclarationStart,
		       tlStatementStart);
	    }
	}
    }

    //--Set the routine identifier node or the record type object.
    if (pRoutineId) {
	pRoutineId->defn.routine.totalLocalSize = totalSize;
    }
    else {
	pRecordType->size = offset;
    }
}
//endfig

//--------------------------------------------------------------
//  ParseIdSublist      Parse a sublist of variable or record
//                      identifiers separated by commas.
//
//      pRoutineId  : ptr to symbol table node of program,
//                    procedure, or function identifier, or NULL
//      pRecordType : ptr to record type object, or NULL
//      pLastId     : ref to ptr that will be set to point to the
//                    last symbol table node of the sublist
//--------------------------------------------------------------

TSymtabNode *TParser::ParseIdSublist(const TSymtabNode *pRoutineId,
				     const TType       *pRecordType,
				     TSymtabNode       *&pLastId)
{
    TSymtabNode *pId;
    TSymtabNode *pFirstId = NULL;

    pLastId = NULL;

    //--Loop to parse each identifier in the sublist.
    while (token == tcIdentifier) {

	//--Variable:  Enter into local  symbol table.
	//--Field:     Enter into record symbol table.
	pId = pRoutineId ? EnterNewLocal(pToken->String())
			 : pRecordType->record.pSymtab->
					    EnterNew(pToken->String());

	//--Link newly-declared identifier nodes together
	//--into a sublist.
	if (pId->defn.how == dcUndefined) {
	    pId->defn.how = pRoutineId ? dcVariable : dcField;
	    if (!pFirstId) pFirstId = pLastId = pId;
	    else {
		pLastId->next = pId;
		pLastId       = pId;
	    }
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
		if (token == tcComma) Error(errMissingIdentifier);
	    } while (token == tcComma);
	    if (token != tcIdentifier) Error(errMissingIdentifier);
	}
	else if (token == tcIdentifier) Error(errMissingComma);
    }

    return pFirstId;
}


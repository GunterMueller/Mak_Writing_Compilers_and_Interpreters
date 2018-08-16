//fig 7-13
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Types #1)                                *
//  *                                                           *
//  *   Parse type definitions, and identifier, enumeration,    *
//  *   and subrange type specifications.                       *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog7-1/parstyp1.cpp                           *
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
//  ParseTypeDefinitions    Parse a list of type definitions
//                          separated by semicolons:
//
//                              <id> = <type>
//
//      pRoutineId : ptr to symbol table node of program,
//                   procedure, or function identifier
//--------------------------------------------------------------

void TParser::ParseTypeDefinitions(TSymtabNode *pRoutineId)
{
    TSymtabNode *pLastId = NULL;  // ptr to last type id node
				  //   in local list

    //--Loop to parse a list of type definitions
    //--seperated by semicolons.
    while (token == tcIdentifier) {

	//--<id>
	TSymtabNode *pTypeId = EnterNewLocal(pToken->String());

	//--Link the routine's local type id nodes together.
	if (!pRoutineId->defn.routine.locals.pTypeIds) {
	    pRoutineId->defn.routine.locals.pTypeIds = pTypeId;
	}
	else {
	    pLastId->next = pTypeId;
	}
	pLastId = pTypeId;

	//-- =
	GetToken();
	CondGetToken(tcEqual, errMissingEqual);

	//--<type>
	SetType(pTypeId->pType, ParseTypeSpec());
	pTypeId->defn.how = dcType;

	//--If the type object doesn't have a name yet,
	//--point it to the type id.
	if (! pTypeId->pType->pTypeId) {
	    pTypeId->pType->pTypeId = pTypeId;
	}

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
//  ParseTypeSpec       Parse a type specification.
//
//  Return: ptr to type object
//--------------------------------------------------------------

TType *TParser::ParseTypeSpec(void)
{
    switch (token) {

	//--Type identifier
	case tcIdentifier: {
	    TSymtabNode *pId = Find(pToken->String());

	    switch (pId->defn.how) {
		case dcType:      return ParseIdentifierType(pId);
		case dcConstant:  return ParseSubrangeType(pId);

		default:
		    Error(errNotATypeIdentifier);
		    GetToken();
		    return(pDummyType);
	    }
	}

	case tcLParen:  return ParseEnumerationType();
	case tcARRAY:   return ParseArrayType();
	case tcRECORD:  return ParseRecordType();

	case tcPlus:
	case tcMinus:
	case tcNumber:
	case tcString:  return ParseSubrangeType(NULL);

	default:
	    Error(errInvalidType);
	    return(pDummyType);
    }
}

//              *********************
//              *                   *
//              *  Identifier Type  *
//              *                   *
//              *********************

//--------------------------------------------------------------
//  ParseIdentifierType     In a type defintion of the form
//
//                               <id-1> = <id-2>
//
//                          parse <id-2>.
//
//      pId2 : ptr to symbol table node of <id-2>
//
//  Return: ptr to type object of <id-2>
//--------------------------------------------------------------

TType *TParser::ParseIdentifierType(const TSymtabNode *pId2)
{
    GetToken();
    return pId2->pType;
}

//              **********************
//              *                    *
//              *  Enumeration Type  *
//              *                    *
//              **********************

//--------------------------------------------------------------
//  ParseEnumerationType    Parse a enumeration type
//                          specification:
//
//                               ( <id-1>, <id-2>, ..., <id-n> )
//
//  Return: ptr to type object
//--------------------------------------------------------------

TType *TParser::ParseEnumerationType(void)
{
    TType       *pType      = new TType(fcEnum, sizeof(int), NULL);
    TSymtabNode *pLastId    = NULL;
    int          constValue = -1;  // enumeration constant value

    GetToken();
    Resync(tlEnumConstStart);

    //--Loop to parse list of constant identifiers separated by commas.
    while (token == tcIdentifier) {
	TSymtabNode *pConstId = EnterNewLocal(pToken->String());
	++constValue;

	if (pConstId->defn.how == dcUndefined) {
	    pConstId->defn.how = dcConstant;
	    pConstId->defn.constant.value.integer = constValue;
	    SetType(pConstId->pType, pType);

	    //--Link constant identifier symbol table nodes together.
	    if (!pLastId) {
		pType->enumeration.pConstIds = pLastId = pConstId;
	    }
	    else {
		pLastId->next = pConstId;
		pLastId       = pConstId;
	    }
	}

	//-- ,
	GetToken();
	Resync(tlEnumConstFollow);
	if (token == tcComma) {

	    //--Saw comma.  Skip extra commas and look for
	    //--            an identifier.
	    do {
		GetToken();
		Resync(tlEnumConstStart, tlEnumConstFollow);
		if (token == tcComma) Error(errMissingIdentifier);
	    } while (token == tcComma);
	    if (token != tcIdentifier) Error(errMissingIdentifier);
	}
	else if (token == tcIdentifier) Error(errMissingComma);
    }

    //-- )
    CondGetToken(tcRParen, errMissingRightParen);

    pType->enumeration.max = constValue;
    return pType;
}

//              *******************
//              *                 *
//              *  Subrange Type  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  ParseSubrangeType       Parse a subrange type specification:
//
//                               <min-const> .. <max-const>
//
//      pMinId : ptr to symbol table node of <min-const> if it
//               is an identifier, else NULL
//
//  Return: ptr to type object
//--------------------------------------------------------------

TType *TParser::ParseSubrangeType(TSymtabNode *pMinId)
{
    TType *pType = new TType(fcSubrange, 0, NULL);

    //--<min-const>
    SetType(pType->subrange.pBaseType,
	    ParseSubrangeLimit(pMinId, pType->subrange.min));

    //-- ..
    Resync(tlSubrangeLimitFollow, tlDeclarationStart);
    CondGetToken(tcDotDot, errMissingDotDot);

    //--<max-const>
    TType *pMaxType = ParseSubrangeLimit(NULL, pType->subrange.max);

    //--Check limits.
    if (pMaxType != pType->subrange.pBaseType) {
	Error(errIncompatibleTypes);
	pType->subrange.min = pType->subrange.max = 0;
    }
    else if (pType->subrange.min > pType->subrange.max) {
	Error(errMinGtMax);

	int temp = pType->subrange.min;
	pType->subrange.min = pType->subrange.max;
	pType->subrange.max = temp;
    }

    pType->size = pType->subrange.pBaseType->size;
    return pType;
}

//--------------------------------------------------------------
//  ParseSubrangeLimit      Parse a mininum or maximum limit
//                          constant of a subrange type.
//
//      pLimitId : ptr to symbol table node of limit constant if
//                 it is an identifier (already set for the
//                 minimum limit), else NULL
//      limit    : ref to limit value that will be set
//
//  Return: ptr to type object of limit constant
//--------------------------------------------------------------

TType *TParser::ParseSubrangeLimit(TSymtabNode *pLimitId, int &limit)
{
    TType      *pType = pDummyType;  // type to return
    TTokenCode  sign  = tcDummy;     // unary + or - sign, or none

    limit = 0;

    //--Unary + or -
    if (TokenIn(token, tlUnaryOps)) {
	if (token == tcMinus) sign = tcMinus;
	GetToken();
    }

    switch (token) {

	case tcNumber:

	    //--Numeric constant:  Integer type only.
	    if (pToken->Type() == tyInteger) {
		limit = sign == tcMinus ? -pToken->Value().integer
					:  pToken->Value().integer;
		pType = pIntegerType;
	    }
	    else Error(errInvalidSubrangeType);
	    break;

	case tcIdentifier:

	    //--Identifier limit:  Must be integer, character, or
	    //--                   enumeration type.
	    if (!pLimitId) pLimitId = Find(pToken->String());

	    if (pLimitId->defn.how == dcUndefined) {
		pLimitId->defn.how = dcConstant;
		pType = SetType(pLimitId->pType, pDummyType);
		break;
	    }

	    else if ((pLimitId->pType == pRealType)  ||
		     (pLimitId->pType == pDummyType) ||
		     (pLimitId->pType->form == fcArray)) {
		Error(errInvalidSubrangeType);
	    }
	    else if (pLimitId->defn.how == dcConstant) {

		//--Use the value of the constant identifier.
		if (pLimitId->pType == pIntegerType) {
		    limit = sign == tcMinus
			? -pLimitId->defn.constant.value.integer
			:  pLimitId->defn.constant.value.integer;
		}
		else if (pLimitId->pType == pCharType) {
		    if (sign != tcDummy) Error(errInvalidConstant);
		    limit = pLimitId->defn.constant.value.character;
		}
		else if (pLimitId->pType->form == fcEnum) {
		    if (sign != tcDummy) Error(errInvalidConstant);
		    limit = pLimitId->defn.constant.value.integer;
		}
		pType = pLimitId->pType;
	    }

	    else Error(errNotAConstantIdentifier);
	    break;

	case tcString:

	    //--String limit:  Character type only.
	    if (sign != tcDummy) Error(errInvalidConstant);

	    if (strlen(pToken->String()) != 3) { // len includes quotes
		Error(errInvalidSubrangeType);
	    }

	    limit = pToken->String()[1];
	    pType = pCharType;
	    break;

	default:
	    Error(errMissingConstant);
	    return pType;  // don't get another token
    }

    GetToken();
    return pType;
}
//endfig


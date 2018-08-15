//fig 7-14
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Types #2)                                *
//  *                                                           *
//  *   Parse array and record type specifications.             *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog7-1/parstyp2.cpp                           *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "common.h"
#include "parser.h"

//              ****************
//              *              *
//              *  Array Type  *
//              *              *
//              ****************

//--------------------------------------------------------------
//  ParseArrayType      Parse an array type specification:
//
//                          ARRAY [ <index-type-list> ]
//                              OF <elmt-type>
//
//  Return: ptr to type object
//--------------------------------------------------------------

TType *TParser::ParseArrayType(void)
{
    TType *pArrayType = new TType(fcArray, 0, NULL);
    TType *pElmtType  = pArrayType;
    int    indexFlag;  // true if another array index, false if done

    //-- [
    GetToken();
    CondGetToken(tcLBracket, errMissingLeftBracket);

    //--Loop to parse each type spec in the index type list,
    //--seperated by commas.
    do {
	ParseIndexType(pElmtType);

	//-- ,
	Resync(tlIndexFollow, tlIndexStart);
	if ((token == tcComma) || TokenIn(token, tlIndexStart)) {

	    //--For each type spec after the first, create an
	    //--element type object.
	    pElmtType = SetType(pElmtType->array.pElmtType,
				new TType(fcArray, 0, NULL));
	    CondGetToken(tcComma, errMissingComma);
	    indexFlag = true;
	}
	else indexFlag = false;

    } while (indexFlag);

    //-- ]
    CondGetToken(tcRBracket, errMissingRightBracket);

    //--OF
    Resync(tlIndexListFollow, tlDeclarationStart, tlStatementStart);
    CondGetToken(tcOF, errMissingOF);

    //--Final element type.
    SetType(pElmtType->array.pElmtType, ParseTypeSpec());

    //--Total byte size of the array.
    if (pArrayType->form != fcNone) {
	pArrayType->size = ArraySize(pArrayType);
    }
    
    return pArrayType;
}

//--------------------------------------------------------------
//  ParseIndexType      Parse an array index type.
//
//      pArrayType : ptr to array type object
//--------------------------------------------------------------

void TParser::ParseIndexType(TType *pArrayType)
{
    if (TokenIn(token, tlIndexStart)) {
	TType *pIndexType = ParseTypeSpec();
	SetType(pArrayType->array.pIndexType, pIndexType);

	switch (pIndexType->form) {

	    //--Subrange index type
	    case fcSubrange:
		pArrayType->array.elmtCount =
			pIndexType->subrange.max -
			pIndexType->subrange.min + 1;
		pArrayType->array.minIndex =
			pIndexType->subrange.min;
		pArrayType->array.maxIndex =
			pIndexType->subrange.max;
		return;

	    //--Enumeration index type
	    case fcEnum:
		pArrayType->array.elmtCount =
			pIndexType->enumeration.max + 1;
		pArrayType->array.minIndex = 0;
		pArrayType->array.maxIndex =
			pIndexType->enumeration.max;
		return;

	    //--Error
	    default:  goto BadIndexType;
	}
    }

BadIndexType:

    //--Error
    SetType(pArrayType->array.pIndexType, pDummyType);
    pArrayType->array.elmtCount = 0;
    pArrayType->array.minIndex  = pArrayType->array.maxIndex = 0;
    Error(errInvalidIndexType);
}

//--------------------------------------------------------------
//  ArraySize           Calculate the total byte size of an
//                      array type by recursively calculating
//                      the size of each dimension.
//
//      pArrayType : ptr to array type object
//
//  Return: byte size
//--------------------------------------------------------------

int TParser::ArraySize(TType *pArrayType)
{
    //--Calculate the size of the element type
    //--if it hasn't already been calculated.
    if (pArrayType->array.pElmtType->size == 0) {
	pArrayType->array.pElmtType->size =
				ArraySize(pArrayType->array.pElmtType);
    }

    return (pArrayType->array.elmtCount *
	    pArrayType->array.pElmtType->size);
}

//              *****************
//              *               *
//              *  Record Type  *
//              *               *
//              *****************

//--------------------------------------------------------------
//  ParseRecordType     Parse a record type specification:
//
//                          RECORD
//                              <id-list> : <type> ;
//                              ...
//                          END
//
//  Return: ptr to type object
//--------------------------------------------------------------

TType *TParser::ParseRecordType(void)
{
    TType *pType          = new TType(fcRecord, 0, NULL);
    pType->record.pSymtab = new TSymtab;

    //--Parse field declarations.
    GetToken();
    ParseFieldDeclarations(pType, 0);

    //--END
    CondGetToken(tcEND, errMissingEND);

    return pType;
}
//endfig

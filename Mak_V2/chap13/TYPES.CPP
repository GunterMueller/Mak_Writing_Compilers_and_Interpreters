//fig 7-8
//  *************************************************************
//  *                                                           *
//  *   T Y P E S                                               *
//  *                                                           *
//  *   CLASSES: TType                                          *
//  *                                                           *
//  *   FILE:    prog7-1/types.cpp                              *
//  *                                                           *
//  *   MODULE:  Symbol table                                   *
//  *                                                           *
//  *   Routines to manage type objects.                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "buffer.h"
#include "error.h"
#include "types.h"

static char *formStrings[] = {"*** Error ***",  "Scalar", "Enumeration",
			      "Subrange",       "Array",  "Record"};

//--Pointers to predefined types.
TType *pIntegerType = NULL;
TType *pRealType    = NULL;
TType *pBooleanType = NULL;
TType *pCharType    = NULL;
TType *pDummyType   = NULL;

//--------------------------------------------------------------
//  Constructors    General:
//
//      fc    : form code
//      s     : byte size of type
//      pNode : ptr to symbol table node of type identifier
//
//                  String: unnamed string type
//
//      length : string length
//--------------------------------------------------------------

//--General
TType::TType(TFormCode fc, int s, TSymtabNode *pId)
{
    form     = fc;
    size     = s;
    pTypeId  = pId;
    refCount = 0;

    switch (fc) {
	case fcSubrange:
	    subrange.pBaseType = NULL;
	    break;

	case fcArray:
	    array.pIndexType = array.pElmtType = NULL;
	    break;

	default:  break;
    }
}

//--String
TType::TType(int length)
{
    form     = fcArray;
    size     = length;
    pTypeId  = NULL;
    refCount = 0;

    array.pIndexType = array.pElmtType = NULL;
    SetType(array.pIndexType, new TType(fcSubrange, sizeof(int), NULL));
    SetType(array.pElmtType,  pCharType);
    array.elmtCount = length;

    //--Integer subrange index type, range 1..length
    SetType(array.pIndexType->subrange.pBaseType, pIntegerType);
    array.pIndexType->subrange.min = 1;
    array.pIndexType->subrange.max = length;
}

//--------------------------------------------------------------
//  Destructor      Delete the allocated objects according to
//                  the form code.  Note that the objects
//                  pointed to by enumeration.pConstIds and by
//                  subrange.pBaseType are deleted along with
//                  the symbol tables that contain their
//                  identifiers.
//--------------------------------------------------------------

TType::~TType(void)
{
    switch (form) {

	case fcSubrange:

	    //--Subrange:  Delete the base type object.
	    RemoveType(subrange.pBaseType);
	    break;

	case fcArray:

	    //--Array:  Delete the index and element type objects.
	    RemoveType(array.pIndexType);
	    RemoveType(array.pElmtType);
	    break;

	case fcRecord:

	    //--Record:  Delete the record fields symbol table.
	    delete record.pSymtab;
	    break;

	default:  break;
    }
}

//--------------------------------------------------------------
//  PrintTypeSpec       Print information about a type
//                      specification for the cross-reference.
//
//      vc : vcVerbose or vcTerse to control the output
//--------------------------------------------------------------

void TType::PrintTypeSpec(TVerbosityCode vc) const
{
    //--Type form and size
    sprintf(list.text, "%s, size %d bytes.  Type identifier: ",
		       formStrings[form], size);

    //--Type identifier
    if (pTypeId) strcat(list.text, pTypeId->String());
    else {
	strcat(list.text, "<unnamed>");
	vc = vcVerbose;  // verbose output for unnamed types
    }
    list.PutLine();

    //--Print the information for the particular type.
    switch (form) {
	case fcEnum:      PrintEnumType    (vc);  break;
	case fcSubrange:  PrintSubrangeType(vc);  break;
	case fcArray:     PrintArrayType   (vc);  break;
	case fcRecord:    PrintRecordType  (vc);  break;
    }
}

//--------------------------------------------------------------
//  PrintEnumType       Print information about an enumeration
//                      type for the cross-reference.
//
//      vc : vcVerbose or vcTerse to control the output
//--------------------------------------------------------------

void TType::PrintEnumType(TVerbosityCode vc) const
{
    if (vc == vcTerse) return;

    //--Print the names and values of the enumeration
    //--constant identifiers.
    list.PutLine("--- Enumeration Constant Identifiers "
		 "(value = name) ---");
    for (TSymtabNode *pConstId = enumeration.pConstIds;
	 pConstId; pConstId = pConstId->next) {
	sprintf(list.text, "    %d = %s",
			   pConstId->defn.constant.value.integer,
			   pConstId->String());
	list.PutLine();
    }
}

//--------------------------------------------------------------
//  PrintSubrangeType   Print information about a subrange
//                      type for the cross-reference.
//
//      vc : vcVerbose or vcTerse to control the output
//--------------------------------------------------------------

void TType::PrintSubrangeType(TVerbosityCode vc) const
{
    if (vc == vcTerse) return;

    //--Subrange minimum and maximum values
    sprintf(list.text, "Minimum value = %d, maximum value = %d",
		       subrange.min, subrange.max);
    list.PutLine();

    //--Base range type
    if (subrange.pBaseType) {
	list.PutLine("--- Base Type ---");
	subrange.pBaseType->PrintTypeSpec(vcTerse);
    }
}

//--------------------------------------------------------------
//  PrintArrayType      Print information about an array
//                      type for the cross-reference.
//
//      vc : vcVerbose or vcTerse to control the output
//--------------------------------------------------------------

void TType::PrintArrayType(TVerbosityCode vc) const
{
    if (vc == vcTerse) return;

    //--Element count
    sprintf(list.text, "%d elements", array.elmtCount);
    list.PutLine();

    //--Index type
    if (array.pIndexType) {
	list.PutLine("--- INDEX TYPE ---");
	array.pIndexType->PrintTypeSpec(vcTerse);
    }

    //--Element type
    if (array.pElmtType) {
	list.PutLine("--- ELEMENT TYPE ---");
	array.pElmtType->PrintTypeSpec(vcTerse);
    }
}

//--------------------------------------------------------------
//  PrintRecordType     Print information about a record
//                      type for the cross-reference.
//
//      vc : vcVerbose or vcTerse to control the output
//--------------------------------------------------------------

void TType::PrintRecordType(TVerbosityCode vc) const
{
    if (vc == vcTerse) return;

    //--Print the names and values of the record field identifiers.
    list.PutLine("--- Record Field Identifiers (offset : name) ---");
    list.PutLine();
    for (TSymtabNode *pFieldId = record.pSymtab->Root();
	 pFieldId; pFieldId = pFieldId->next) {
	sprintf(list.text, "    %d : %s",
			   pFieldId->defn.data.offset,
			   pFieldId->String());
	list.PutLine();
	pFieldId->PrintVarOrField();
    }
}

//--------------------------------------------------------------
//  InitializePredefinedTypes   Initialize the predefined
//                              types by entering their
//                              identifiers into the symbol
//                              table.
//
//      pSymtab : ptr to symbol table
//--------------------------------------------------------------

void InitializePredefinedTypes(TSymtab *pSymtab)
{
    //--Enter the names of the predefined types and of "false"
    //--and "true" into the symbol table.
    TSymtabNode *pIntegerId = pSymtab->Enter("integer", dcType);
    TSymtabNode *pRealId    = pSymtab->Enter("real",    dcType);
    TSymtabNode *pBooleanId = pSymtab->Enter("boolean", dcType);
    TSymtabNode *pCharId    = pSymtab->Enter("char",    dcType);
    TSymtabNode *pFalseId   = pSymtab->Enter("false",   dcConstant);
    TSymtabNode *pTrueId    = pSymtab->Enter("true",    dcConstant);

    //--Create the predefined type objects.
    if (!pIntegerType) {
	SetType(pIntegerType,
		new TType(fcScalar, sizeof(int), pIntegerId));
    }
    if (!pRealType) {
	SetType(pRealType,
		new TType(fcScalar, sizeof(float), pRealId));
    }
    if (!pBooleanType) {
	SetType(pBooleanType,
		new TType(fcEnum, sizeof(int), pBooleanId));
    }
    if (!pCharType) {
	SetType(pCharType,
		new TType(fcScalar, sizeof(char), pCharId));
    }

    //--Link each predefined type's id node to its type object.
    SetType(pIntegerId->pType, pIntegerType);
    SetType(pRealId   ->pType, pRealType);
    SetType(pBooleanId->pType, pBooleanType);
    SetType(pCharId   ->pType, pCharType);

    //--More initialization for the boolean type object.
    pBooleanType->enumeration.max       = 1;        // max value
    pBooleanType->enumeration.pConstIds = pFalseId; // first constant

    //--More initialization for the "false" and "true" id nodes.
    pFalseId->defn.constant.value.integer = 0;
    pTrueId ->defn.constant.value.integer = 1;
    SetType(pTrueId->pType,  pBooleanType);
    SetType(pFalseId->pType, pBooleanType);
    pFalseId->next = pTrueId;  // "false" node points to "true" node

    //--Initialize the dummy type object that will be used
    //--for erroneous type definitions and for typeless objects.
    SetType(pDummyType, new TType(fcNone, 1, NULL));
}

//--------------------------------------------------------------
//  RemovePredefinedTypes       Remove the predefined types.
//--------------------------------------------------------------

void RemovePredefinedTypes(void)
{
    RemoveType(pIntegerType);
    RemoveType(pRealType);
    RemoveType(pBooleanType);
    RemoveType(pCharType);
    RemoveType(pDummyType);
}

//--------------------------------------------------------------
//  SetType     Set the target type.  Increment the reference
//              count of the source type.
//
//      pTargetType : ref to ptr to target type object
//      pSourceType : ptr to source type object
//
//  Return: ptr to source type object
//--------------------------------------------------------------

TType *SetType(TType *&pTargetType, TType *pSourceType)
{
    if (!pTargetType) RemoveType(pTargetType);

    ++pSourceType->refCount;
    pTargetType = pSourceType;

    return pSourceType;
}

//--------------------------------------------------------------
//  RemoveType  Decrement a type object's reference count, and
//              delete the object and set its pointer to NULL
//              if the count becomes 0.
//
//      pType : ref to ptr to type object
//--------------------------------------------------------------

void RemoveType(TType *&pType)
{
    if (pType && (--pType->refCount == 0)) {
	delete pType;
	pType = NULL;
    }
}

//              ************************
//              *                      *
//              *  Type Compatibility  *
//              *                      *
//              ************************

//--------------------------------------------------------------
//  CheckRelOpOperands  Check that the types of the two operands
//                      of a relational operator are compatible.
//                      Flag an incompatible type error if not.
//
//      pType1 : ptr to the first  operand's type object
//      pType2 : ptr to the second operand's type object
//--------------------------------------------------------------

void CheckRelOpOperands(const TType *pType1, const TType *pType2)
{
    pType1 = pType1->Base();
    pType2 = pType2->Base();

    //--Two identical scalar or enumeration types.
    if (   (pType1 == pType2)
	&& ((pType1->form == fcScalar) || (pType1->form == fcEnum))) {
	return;
    }

    //--One integer operand and one real operand.
    if (   ((pType1 == pIntegerType) && (pType2 == pRealType))
	|| ((pType2 == pIntegerType) && (pType1 == pRealType))) {
	return;
    }

    //--Two strings of the same length.
    if (   (pType1->form == fcArray)
	&& (pType2->form == fcArray)
	&& (pType1->array.pElmtType == pCharType)
	&& (pType2->array.pElmtType == pCharType)
	&& (pType1->array.elmtCount == pType2->array.elmtCount)) {
	return;
    }

    //--Else:  Incompatible types.
    Error(errIncompatibleTypes);
}

//--------------------------------------------------------------
//  CheckIntegerOrReal  Check that the type of each operand is
//                      either integer or real.  Flag an
//                      incompatible type error if not.
//
//      pType1 : ptr to the first  operand's type object
//      pType2 : ptr to the second operand's type object or NULL
//--------------------------------------------------------------

void CheckIntegerOrReal(const TType *pType1, const TType *pType2)
{
    pType1 = pType1->Base();
    if ((pType1 != pIntegerType) && (pType1 != pRealType)) {
	Error(errIncompatibleTypes);
    }

    if (pType2) {
	pType2 = pType2->Base();
	if ((pType2 != pIntegerType) && (pType2 != pRealType)) {
	    Error(errIncompatibleTypes);
	}
    }
}

//--------------------------------------------------------------
//  CheckBoolean        Check that the type of each operand is
//                      boolean.  Flag an incompatible type
//                      error if not.
//
//      pType1 : ptr to the first  operand's type object
//      pType2 : ptr to the second operand's type object or NULL
//--------------------------------------------------------------

void CheckBoolean(const TType *pType1, const TType *pType2)
{
    if (   (pType1->Base() != pBooleanType)
	|| (pType2 && (pType2->Base() != pBooleanType))) {
	Error(errIncompatibleTypes);
    }
}

//--------------------------------------------------------------
//  CheckAssignmentTypeCompatible   Check that a value's type is
//                                  assignment compatible with
//                                  the target's type.  Flag an
//                                  error if not.
//
//      pTargetType : ptr to the target's type object
//      pValueType  : ptr to the value's  type object
//      ec          : error code
//--------------------------------------------------------------

void CheckAssignmentTypeCompatible(const TType *pTargetType,
				   const TType *pValueType,
				   TErrorCode ec)
{
    pTargetType = pTargetType->Base();
    pValueType  = pValueType ->Base();

    //--Two identical types.
    if (pTargetType == pValueType) return;

    //--real := integer
    if (   (pTargetType == pRealType)
	&& (pValueType  == pIntegerType)) return;


    //--Two strings of the same length.
    if (   (pTargetType->form == fcArray)
	&& (pValueType ->form == fcArray)
	&& (pTargetType->array.pElmtType == pCharType)
	&& (pValueType ->array.pElmtType == pCharType)
	&& (pTargetType->array.elmtCount ==
				pValueType->array.elmtCount)) {
	return;
    }

    Error(ec);
}

//--------------------------------------------------------------
//  IntegerOperands     Check that the types of both operands
//                      are integer.
//
//      pType1 : ptr to the first  operand's type object
//      pType2 : ptr to the second operand's type object
//
//  Return: true if yes, false if no
//--------------------------------------------------------------

int IntegerOperands(const TType *pType1, const TType *pType2)
{
    pType1 = pType1->Base();
    pType2 = pType2->Base();

    return (pType1 == pIntegerType) && (pType2 == pIntegerType);
}

//--------------------------------------------------------------
//  RealOperands        Check that the types of both operands
//                      are real, or that one is real and the
//                      other is integer.
//
//      pType1 : ptr to the first  operand's type object
//      pType2 : ptr to the second operand's type object
//
//  Return: true if yes, false if no
//--------------------------------------------------------------

int RealOperands(const TType *pType1, const TType *pType2)
{
    pType1 = pType1->Base();
    pType2 = pType2->Base();

    return    ((pType1 == pRealType) && (pType2 == pRealType))
	   || ((pType1 == pRealType) && (pType2 == pIntegerType))
	   || ((pType2 == pRealType) && (pType1 == pIntegerType));
}
//endfig


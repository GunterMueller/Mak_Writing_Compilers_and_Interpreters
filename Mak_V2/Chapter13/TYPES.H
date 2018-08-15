//fig 7-5
//  *************************************************************
//  *                                                           *
//  *   T Y P E S   (Header)                                    *
//  *                                                           *
//  *	CLASSES: TType                               		*
//  *                                                           *
//  *   FILE:    prog7-1/types.h                                *
//  *                                                           *
//  *   MODULE:  Symbol table                                   *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef types_h
#define types_h

#include "error.h"
#include "symtab.h"

extern TType *pIntegerType, *pRealType, *pBooleanType, *pCharType,
	     *pDummyType;

//--------------------------------------------------------------
//  TFormCode           Form code: What form of data structure.
//--------------------------------------------------------------

enum TFormCode {
    fcNone, fcScalar, fcEnum, fcSubrange, fcArray, fcRecord,
};

extern char *formStrings[];

//--------------------------------------------------------------
//  TType               Type class.
//--------------------------------------------------------------

class TType {
    int refCount;           // reference count

public:
    TFormCode    form;      // form code
    int          size;      // byte size of type
    TSymtabNode *pTypeId;   // ptr to symtab node of type identifier

    union {

	//--Enumeration
	struct {
	    TSymtabNode *pConstIds;  // ptr to list of const id nodes
	    int          max;        // max constant value
	} enumeration;

	//--Subrange
	struct {
	    TType *pBaseType;  // ptr to base type object
	    int    min, max;   // min and max subrange limit values
	} subrange;

	//--Array
	struct {
	    TType *pIndexType;          // ptr to index type object
	    TType *pElmtType;           // ptr to elmt  type object
	    int    minIndex, maxIndex;  // min and max index values
	    int    elmtCount;           // count of array elmts
	} array;

	//--Record
	struct {
	    TSymtab *pSymtab;  // ptr to record fields symtab
	} record;
    };

    //--General and string type constructors.
    TType(TFormCode fc, int s, TSymtabNode *pId);
    TType(int length);

   ~TType(void);

    int IsScalar(void) const { return (form != fcArray) &&
    				      (form != fcRecord); }

    TType *Base(void) const
    {
	return form == fcSubrange ? subrange.pBaseType : (TType *) this;
    }

    enum TVerbosityCode {vcVerbose, vcTerse};

    void PrintTypeSpec    (TVerbosityCode vc) const;
    void PrintEnumType    (TVerbosityCode vc) const;
    void PrintSubrangeType(TVerbosityCode vc) const;
    void PrintArrayType   (TVerbosityCode vc) const;
    void PrintRecordType  (TVerbosityCode vc) const;

    friend TType *SetType   (TType *&pTargetType, TType *pSourceType);
    friend void   RemoveType(TType *&pType);

    friend void CheckRelOpOperands(const TType *pType1,
				   const TType *pType2);
    friend void CheckIntegerOrReal(const TType *pType1,
				   const TType *pType2 = NULL);
    friend void CheckBoolean      (const TType *pType1,
				   const TType *pType2 = NULL);
    friend void CheckAssignmentTypeCompatible(const TType *pTargetType,
					      const TType *pValueType,
					      TErrorCode ec);

    friend int  IntegerOperands(const TType *pType1,
				const TType *pType2);
    friend int  RealOperands   (const TType *pType1,
    				const TType *pType2);
};

void InitializePredefinedTypes(TSymtab *pSymtab);
void RemovePredefinedTypes(void);

#endif

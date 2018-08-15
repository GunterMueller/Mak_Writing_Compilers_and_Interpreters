//fig 8-29
//  *************************************************************
//  *                                                           *
//  *   P R E T T Y   P R I N T E R   (Declarations)            *
//  *                                                           *
//  *   Pretty-print declarations.                              *
//  *                                                           *
//  *   CLASSES: TPrettyPrinter                                 *
//  *                                                           *
//  *   FILE:    prog8-2/ppdecl.cpp                             *
//  *                                                           *
//  *   MODULE:  Pretty printer                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "types.h"
#include "pprinter.h"

//--------------------------------------------------------------
//  PrintDeclarations   Pretty-print definitions and
//                      declarations.
//
//      pRoutineId : ptr to the routine id's symbol table node
//--------------------------------------------------------------

void TPrettyPrinter::PrintDeclarations(const TSymtabNode *pRoutineId)
{
    TSymtabNode *pConstId= pRoutineId->defn.routine.locals.pConstantIds;
    TSymtabNode *pTypeId = pRoutineId->defn.routine.locals.pTypeIds;
    TSymtabNode *pVarId  = pRoutineId->defn.routine.locals.pVariableIds;
    TSymtabNode *pRtnId  = pRoutineId->defn.routine.locals.pRoutineIds;

    if (pConstId) PrintConstantDefinitions   (pConstId);
    if (pTypeId)  PrintTypeDefinitions       (pTypeId);
    if (pVarId)   PrintVariableDeclarations  (pVarId);
    if (pRtnId)   PrintSubroutineDeclarations(pRtnId);
}

//--------------------------------------------------------------
//  PrintConstantDefinitions    Pretty-print constant
//                              definitions:
//
//                                  CONST
//                                      <id-1> = <value-1>;
//                                      <id-2> = <value-2>;
//                                      ...
//
//      pConstId : ptr to the head of the constant id list
//--------------------------------------------------------------

void TPrettyPrinter::PrintConstantDefinitions
				(const TSymtabNode *pConstId)
{
    PutLine(" ");
    PutLine("CONST");
    Indent();

    //--Loop to print constant definitions, one per line.
    do {
	char   text[maxInputBufferSize];
	TType *pConstType = pConstId->pType;

	//--Print the constant identifier followed by = .
	Put(pConstId->String());
	Put(" = ");
	int saveMargin = SetMargin();

	//--Print the constant value.
	if (pConstType->form == fcArray) {  // string
	    Put("'");
	    Put(pConstId->defn.constant.value.pString);
	    Put("'");
	}
	else if (pConstType == pIntegerType) {
	    sprintf(text, "%d", pConstId->defn.constant.value.integer);
	    Put(text);
	}
	else if (pConstType == pRealType) {
	    sprintf(text, "%g", pConstId->defn.constant.value.real);
	    Put(text);
	}
	else if (pConstType == pCharType) {
	    sprintf(text, "'%c'",
			  pConstId->defn.constant.value.character);
	    Put(text);
	}

	PutLine(";");
	ResetMargin(saveMargin);

	pConstId = pConstId->next;
    } while (pConstId);

    Dedent();
}

//--------------------------------------------------------------
//  PrintTypeDefinitions        Pretty-print type definitions:
//
//                                 TYPE
//                                      <id-1> = <type-spec-1>;
//                                      <id-2> = <type-spec-2>;
//                                      ...
//
//      pTypeId : ptr to the head of the type id list
//--------------------------------------------------------------

void TPrettyPrinter::PrintTypeDefinitions(const TSymtabNode *pTypeId)
{
    PutLine(" ");
    PutLine("TYPE");
    Indent();

    //--Loop to print type definitions, one per line.
    do {
	//--Print the type identifier followed by = .
	Put(pTypeId->String());
	Put(" = ");
	int saveMargin = SetMargin();

	//--Print the type specification.
	PrintTypeSpec(pTypeId->pType,
		      pTypeId == pTypeId->pType->pTypeId);

	PutLine(";");
	ResetMargin(saveMargin);

	pTypeId = pTypeId->next;
    } while (pTypeId);

    Dedent();
}

//--------------------------------------------------------------
//  PrintVariableDeclarations   Pretty-print variable
//                              declarations:
//
//                                  VAR
//                                      <var-or-fields>
//
//      pVarId : ptr to the head of the variable id list
//--------------------------------------------------------------

void TPrettyPrinter::PrintVariableDeclarations
				(const TSymtabNode *pVarId)
{
    PutLine(" ");
    PutLine("VAR");
    Indent();
    PrintVarsOrFields(pVarId);
    Dedent();
}

//--------------------------------------------------------------
//  PrintVarsOrFields           Pretty-print variable or record
//                              field declarations:
//
//                      <id-1>, <id-2>, ... : <type-spec-1>;
//                      ...
//
//      pId : ptr to the head of the variable or field id list
//--------------------------------------------------------------

void TPrettyPrinter::PrintVarsOrFields(const TSymtabNode *pId)
{
    TType *pCommonType = pId->pType;  // ptr to common type of sublist

    //--Loop to print sublists of variables with a common type.
    do {
	Put(pId->String());
	pId = pId->next;

	if (pId && (pId->pType == pCommonType)) Put(", ");
	else {

	    //--End of sublist:  Print the common type and begin
	    //--                 a new sublist.
	    Put(" : ");
	    int saveMargin = SetMargin();
	    PrintTypeSpec(pCommonType, false);

	    PutLine(";");
	    ResetMargin(saveMargin);

	    if (pId) pCommonType = pId->pType;
	}
    } while (pId);
}

//--------------------------------------------------------------
//  PrintSubroutineDeclarations     Pretty-print procedure or
//                                  function declarations.
//
//      pRtnId : ptr to the head of the routine id list
//--------------------------------------------------------------

void TPrettyPrinter::PrintSubroutineDeclarations
				(const TSymtabNode *pRtnId)
{
    do {
	PrintSubroutine(pRtnId);
	pRtnId = pRtnId->next;
    } while (pRtnId);
}

//--------------------------------------------------------------
//  PrintTypeSpec       Pretty-print a type specification.  If
//                      the type is being defined as a named
//                      type, or it is unnamed, print it out
//                      completely.  Otherwise, just print the
//                      type identifier.
//
//      pType    : ptr to the type object
//      defnFlag : true if being defined as a named type,
//                 false if not
//--------------------------------------------------------------

void TPrettyPrinter::PrintTypeSpec(const TType *pType, int defnFlag)
{
    //--Named type that is part of a type specification:
    //--Just print the type identifier.
    if (!defnFlag && pType->pTypeId) Put(pType->pTypeId->String());

    //--Otherwise, print the type spec completely.
    else switch (pType->form) {
	case fcEnum:        PrintEnumType    (pType);   break;
	case fcSubrange:    PrintSubrangeType(pType);   break;
	case fcArray:       PrintArrayType   (pType);   break;
	case fcRecord:      PrintRecordType  (pType);   break;
    }
}

//--------------------------------------------------------------
//  PrintEnumType       Pretty-print an enumeration type spec:
//
//                          (<id-1>, <id-2>,
//                           <id-3>, ...)
//
//      pType : ptr to the type object
//--------------------------------------------------------------

void TPrettyPrinter::PrintEnumType(const TType *pType)
{
    TSymtabNode *pConstId = pType->enumeration.pConstIds;

    Put("(");
    int saveMargin = SetMargin();

    //--Loop to print the enumeration constant identifiers.
    do {
	Put(pConstId->String());
	pConstId = pConstId->next;
	if (pConstId) Put(", ");
    } while (pConstId);

    Put(")");
    ResetMargin(saveMargin);
}

//--------------------------------------------------------------
//  PrintSubrangeType   Pretty-print a subrange type spec:
//
//                          <min>..<max>
//
//      pType : ptr to the type object
//--------------------------------------------------------------

void TPrettyPrinter::PrintSubrangeType(const TType *pType)
{
    PrintSubrangeLimit(pType->subrange.min, pType->subrange.pBaseType);
    Put("..");
    PrintSubrangeLimit(pType->subrange.max, pType->subrange.pBaseType);
}

//--------------------------------------------------------------
//  PrintSubrangeLimit  Pretty-print the minimum or maximum
//                      limit of a subrange type spec.
//
//      limit     : the min or max limit value
//      pBaseType : ptr to the base type object
//--------------------------------------------------------------

void TPrettyPrinter::PrintSubrangeLimit(int limit,
					const TType *pBaseType)
{
    char text[maxInputBufferSize];

    if (pBaseType == pIntegerType) {
	sprintf(text, "%d", limit);
	Put(text);
    }
    else if (pBaseType == pCharType) {
	sprintf(text, "'%c'", limit);
	Put(text);
    }

    //--Enumeration:  Find the appropriate enumeration constant id.
    else {
	TSymtabNode *pConstId = pBaseType->enumeration.pConstIds;
	while (limit-- > 0) pConstId = pConstId->next;
	Put(pConstId->String());
    }
}

//--------------------------------------------------------------
//  PrintArrayType      Pretty-print an array type spec:
//
//                  ARRAY [<type-spec-1>] OF
//                      ARRAY [<type-spec-2>] OF <type-spec-3>
//
//      pType : ptr to the type object
//--------------------------------------------------------------

void TPrettyPrinter::PrintArrayType(const TType *pType)
{
    TType *pIndexType = pType->array.pIndexType;
    TType *pElmtType  = pType->array.pElmtType;

    Put("ARRAY [");
    PrintTypeSpec(pIndexType, false);
    Put("] OF ");

    if ((pElmtType->pTypeId) || (pElmtType->IsScalar())) {
	PrintTypeSpec(pElmtType, false);
    }
    else {

	//--Cascade array of unnamed arrays or records
	//--over multiple lines.
	PutLine();
	Indent();
	PrintTypeSpec(pElmtType, false);
	Dedent();
    }
}

//--------------------------------------------------------------
//  PrintRecordType     Pretty-print a record type spec:
//
//                          RECORD
//                              <var-or-fields>
//                          END
//
//      pType : ptr to the type object
//--------------------------------------------------------------

void TPrettyPrinter::PrintRecordType(const TType *pType)
{
    PutLine("RECORD");

    Indent();
    PrintVarsOrFields(pType->record.pSymtab->Root());
    Dedent();

    Put("END");
}
//endfig

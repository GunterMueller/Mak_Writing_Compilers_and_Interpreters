//fig 8-26
//  *************************************************************
//  *                                                           *
//  *   P R E T T Y   P R I N T E R   (Header)                  *
//  *                                                           *
//  *   CLASSES: TPrettyPrinter                                 *
//  *                                                           *
//  *   FILE:    prog8-2/pprinter.h                             *
//  *                                                           *
//  *   MODULE:  Pretty printer                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef pprinter_h
#define pprinter_h

#include "misc.h"
#include "buffer.h"
#include "error.h"
#include "symtab.h"
#include "pretlist.h"
#include "backend.h"

//--------------------------------------------------------------
//  TPrettyPrinter      Pretty printer subclass of TBackend.
//--------------------------------------------------------------

class TPrettyPrinter : public TBackend {

    TPrettyListBuffer *const pPretty;  // ptr to pretty list buffer

    //--Routines.
    void PrintProgram          (const TSymtabNode *pProgramId);
    void PrintSubroutine       (const TSymtabNode *pRoutineId);
    void PrintSubroutineHeader (const TSymtabNode *pRoutineId);
    void PrintSubroutineFormals(const TSymtabNode *pParmId);
    void PrintBlock            (const TSymtabNode *pRoutineId);

    //--Declarations.
    void PrintDeclarations          (const TSymtabNode *pRoutineId);
    void PrintTypeSpec              (const TType *pType, int defnFlag);
    void PrintConstantDefinitions   (const TSymtabNode *pConstId);
    void PrintTypeDefinitions       (const TSymtabNode *pTypeId);
    void PrintVariableDeclarations  (const TSymtabNode *pVarId);
    void PrintVarsOrFields          (const TSymtabNode *pId);
    void PrintSubroutineDeclarations(const TSymtabNode *pRtnId);
    void PrintEnumType              (const TType *pType);
    void PrintSubrangeType          (const TType *pType);
    void PrintSubrangeLimit         (int limit, const TType *pBaseType);
    void PrintArrayType             (const TType *pType);
    void PrintRecordType            (const TType *pType);

    //--Statements.
    void PrintStatement       (void);
    void PrintStatementList   (TTokenCode terminator);
    void PrintAssignmentOrCall(void);
    void PrintREPEAT          (void);
    void PrintWHILE           (void);
    void PrintIF              (void);
    void PrintFOR             (void);
    void PrintCASE            (void);
    void PrintCompound        (void);

    //--Expressions.
    void PrintExpression(void);
    void PrintIdentifier(void);

    void Put        (const char *pString) { pPretty->Put(pString);     }
    void PutLine    (void)                { pPretty->PutLine();        }
    void PutLine    (const char *pString) { pPretty->PutLine(pString); }
    void Indent     (void)  { pPretty->Indent(); }
    void Dedent     (void)  { pPretty->Dedent(); }
    int  SetMargin  (void)  { return pPretty->SetMargin(); }
    void ResetMargin(int m) { pPretty->ResetMargin(m);     }

public:
    TPrettyPrinter(void)
	: pPretty(new TPrettyListBuffer) {}

    virtual ~TPrettyPrinter(void) { delete pPretty; }

    virtual void Go(const TSymtabNode *pProgramId)
    {
	PrintProgram(pProgramId);
    }
};

#endif
//endfig

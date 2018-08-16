//fig 12-9
//  *************************************************************
//  *                                                           *
//  *   E M I T   S O U R C E   L I N E S                       *
//  *                                                           *
//  *   Emit source lines as comments in the assembly listing.  *
//  *                                                           *
//  *   CLASSES: TCodeGenerator                                 *
//  *                                                           *
//  *   FILE:    prog13-1/emitsrc.cpp                           *
//  *                                                           *
//  *   MODULE:  Code generator                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "common.h"
#include "buffer.h"
#include "symtab.h"
#include "codegen.h"

//--------------------------------------------------------------
//  StartComment    Start a new comment with a line number.
//
//      pString : ptr to the string to append
//--------------------------------------------------------------

void TCodeGenerator::StartComment(int n)
{
    Reset();
    sprintf(AsmText(), "; {%d} ", n);
    Advance();
}

//--------------------------------------------------------------
//  PutComment      Append a string to the assembly comment if
//                  it fits.  If not, emit the current comemnt
//                  and append the string to the next comment.
//
//      pString : ptr to the string to append
//--------------------------------------------------------------

void TCodeGenerator::PutComment(const char *pString)
{
    int length = strlen(pString);

    //--Start a new comment if the current one is full.
    if (!pAsmBuffer->Fit(length)) {
	PutLine();
	StartComment();
    }

    strcpy(AsmText(), pString);
    Advance();
}

//              ******************
//              *                *
//              *  Declarations  *
//              *                *
//              ******************

//--------------------------------------------------------------
//  EmitProgramHeaderComment    Emit the program header as a
//                              comment.
//
//      pProgramId : ptr to the program id's symbol table node
//--------------------------------------------------------------

void TCodeGenerator::EmitProgramHeaderComment
					(const TSymtabNode *pProgramId)
{
    PutLine();
    StartComment("PROGRAM ");
    PutComment(pProgramId->String());  // program name

    //--Emit the program's parameter list.
    TSymtabNode *pParmId = pProgramId->defn.routine.locals.pParmIds;
    if (pParmId) {
	PutComment(" (");

	//--Loop to emit each parameter.
	do {
	    PutComment(pParmId->String());
	    pParmId = pParmId->next;
	    if (pParmId) PutComment(", ");
	} while (pParmId);

	PutComment(")");
    }

    PutLine();
}

//--------------------------------------------------------------
//  EmitSubroutineHeaderComment     Emit a subroutine header as
//                                  a comment.
//
//      pRoutineId : ptr to the subroutine id's symtab node
//--------------------------------------------------------------

void TCodeGenerator::EmitSubroutineHeaderComment
					(const TSymtabNode *pRoutineId)
{
    PutLine();
    StartComment(pRoutineId->defn.how == dcProcedure ? "PROCEDURE "
						     : "FUNCTION " );
    //--Emit the procedure or function name
    //--followed by the formal parameter list.
    PutComment(pRoutineId->String());
    EmitSubroutineFormalsComment
			(pRoutineId->defn.routine.locals.pParmIds);

    //--Emit a function's return type.
    if (pRoutineId->defn.how == dcFunction) {
	PutComment(" : ");
	PutComment(pRoutineId->pType->pTypeId->String());
    }

    PutLine();
}

//--------------------------------------------------------------
//  EmitSubroutineFormalsComment    Emit a formal parameter list
//                                  as a comment.
//
//      pParmId : ptr to the head of the formal parm id list
//--------------------------------------------------------------

void TCodeGenerator::EmitSubroutineFormalsComment
					(const TSymtabNode *pParmId)
{
    if (!pParmId) return;

    PutComment(" (");

    //--Loop to emit each sublist of parameters with
    //--common definition and type.
    do {
	TDefnCode  commonDefn  = pParmId->defn.how;  // common defn
	TType     *pCommonType = pParmId->pType;     // common type
	int        doneFlag;  // true if sublist done, false if not

	if (commonDefn == dcVarParm) PutComment("VAR ");

	//--Loop to emit the parms in the sublist.
	do {
	    PutComment(pParmId->String());

	    pParmId  = pParmId->next;
	    doneFlag = (!pParmId) || (commonDefn  != pParmId->defn.how)
				  || (pCommonType != pParmId->pType);
	    if (!doneFlag) PutComment(", ");
	} while (!doneFlag);

	//--Print the sublist's common type.
	PutComment(" : ");
	PutComment(pCommonType->pTypeId->String());

	if (pParmId) PutComment("; ");
    } while (pParmId);

    PutComment(")");
}

//--------------------------------------------------------------
//  EmitVarDeclComment      Emit variable declarations as
//                          comments.
//
//      pVarId : ptr to the head of the variable id list
//--------------------------------------------------------------

void TCodeGenerator::EmitVarDeclComment
				(const TSymtabNode *pVarId)
{
    TType *pCommonType;  // ptr to common type

    if (!pVarId) return;
    pCommonType = pVarId->pType;

    PutLine();
    StartComment("VAR");
    PutLine();
    StartComment();

    //--Loop to print sublists of variables with a common type.
    do {
	PutComment(pVarId->String());
	pVarId = pVarId->next;

	if (pVarId && (pVarId->pType == pCommonType)) PutComment(", ");
	else {

	    //--End of sublist:  Print the common type and begin
	    //--                 a new sublist.
	    PutComment(" : ");
	    EmitTypeSpecComment(pCommonType);
	    PutLine();

	    if (pVarId) {
		pCommonType = pVarId->pType;
		StartComment();
	    }
	}
    } while (pVarId);
}

//--------------------------------------------------------------
//  EmitTypeSpecComment     Emit a type specification as a
//                          comment.
//
//      pType : ptr to the type object
//--------------------------------------------------------------

void TCodeGenerator::EmitTypeSpecComment(const TType *pType)
{
    //--If named type, emit the name, else emit "..."
    PutComment(pType->pTypeId ? pType->pTypeId->String() : "...");
}

//              ****************
//              *              *
//              *  Statements  *
//              *              *
//              ****************

//--------------------------------------------------------------
//  EmitStmtComment         Emit a statement as a comment.
//--------------------------------------------------------------

void TCodeGenerator::EmitStmtComment(void)
{
    SaveState();     // save icode state
    StartComment(currentLineNumber);

    switch (token) {
	case tcIdentifier:  EmitAsgnOrCallComment();    break;
	case tcREPEAT:      EmitREPEATComment();        break;
	case tcUNTIL:       EmitUNTILComment();         break;
	case tcWHILE:       EmitWHILEComment();         break;
	case tcIF:          EmitIFComment();            break;
	case tcFOR:         EmitFORComment();           break;
	case tcCASE:        EmitCASEComment();          break;
    }

    RestoreState();  // restore icode state
}

//--------------------------------------------------------------
//  EmitAsgnOrCallComment   Emit an assignment statement or a
//                          procedure call as a comment.
//--------------------------------------------------------------

void TCodeGenerator::EmitAsgnOrCallComment(void)
{
    EmitIdComment();

    if (token == tcColonEqual) {
	PutComment(" := ");

	GetToken();
	EmitExprComment();
    }

    PutLine();
}

//--------------------------------------------------------------
//  EmitREPEATComment   Emit a REPEAT statement as a comment.
//  EmitUNTILComment
//--------------------------------------------------------------

void TCodeGenerator::EmitREPEATComment(void)
{
    PutComment("REPEAT");
    PutLine();
}

void TCodeGenerator::EmitUNTILComment(void)
{
    PutComment("UNTIL ");

    GetToken();
    EmitExprComment();

    PutLine();
}

//--------------------------------------------------------------
//  EmitWHILEComment    Emit a WHILE statement as a comment.
//--------------------------------------------------------------

void TCodeGenerator::EmitWHILEComment(void)
{
    PutComment("WHILE ");

    GetToken();
    GetLocationMarker();

    GetToken();
    EmitExprComment();

    PutComment(" DO");
    PutLine();
}

//--------------------------------------------------------------
//  EmitIFComment       Emit an IF statement as a comment.
//--------------------------------------------------------------

void TCodeGenerator::EmitIFComment(void)
{
    PutComment("IF ");

    GetToken();
    GetLocationMarker();

    GetToken();
    EmitExprComment();

    PutLine();
}

//--------------------------------------------------------------
//  EmitFORComment      Emit a FOR statement as a comment.
//--------------------------------------------------------------

void TCodeGenerator::EmitFORComment(void)
{
    PutComment("FOR ");

    GetToken();
    GetLocationMarker();

    GetToken();
    EmitIdComment();
    PutComment(" := ");

    GetToken();
    EmitExprComment();
    PutComment(token == tcTO ? " TO " : " DOWNTO ");

    GetToken();
    EmitExprComment();

    PutComment(" DO");
    PutLine();
}

//--------------------------------------------------------------
//  EmitCASEComment     Emit a CASE statement as a comment.
//--------------------------------------------------------------

void TCodeGenerator::EmitCASEComment(void)
{
    PutComment("CASE ");

    GetToken();
    GetLocationMarker();
    GetToken();
    GetLocationMarker();

    GetToken();
    EmitExprComment();

    PutComment(" OF ");
    PutLine();
}

//              ******************
//              *                *
//              *  Expresssions  *
//              *                *
//              ******************

//--------------------------------------------------------------
//  EmitExprComment         Emit an expression as a comment.
//--------------------------------------------------------------

void TCodeGenerator::EmitExprComment(void)
{
    int doneFlag = false;  // true if done with expression, false if not

    //--Loop over the entire expression.
    do {
	switch (token) {
	    case tcIdentifier:  EmitIdComment();  break;

	    case tcNumber:  PutComment(pToken->String());  GetToken();
			    break;

	    case tcString:  PutComment(pToken->String());  GetToken();
			    break;

	    case tcPlus:    PutComment(" + ");    GetToken();  break;
	    case tcMinus:   PutComment(" - ");    GetToken();  break;
	    case tcStar:    PutComment("*");      GetToken();  break;
	    case tcSlash:   PutComment("/");      GetToken();  break;
	    case tcDIV:     PutComment(" DIV ");  GetToken();  break;
	    case tcMOD:     PutComment(" MOD ");  GetToken();  break;
	    case tcAND:     PutComment(" AND ");  GetToken();  break;
	    case tcOR:      PutComment(" OR ");   GetToken();  break;
	    case tcEqual:   PutComment(" = ");    GetToken();  break;
	    case tcNe:      PutComment(" <> ");   GetToken();  break;
	    case tcLt:      PutComment(" < ");    GetToken();  break;
	    case tcLe:      PutComment(" <= ");   GetToken();  break;
	    case tcGt:      PutComment(" > ");    GetToken();  break;
	    case tcGe:      PutComment(" >= ");   GetToken();  break;
	    case tcNOT:     PutComment("NOT ");   GetToken();  break;

	    case tcLParen:
		PutComment("(");
		GetToken();
		EmitExprComment();
		PutComment(")");
		GetToken();
		break;

	    default:
		doneFlag = true;
		break;
	}
    } while (!doneFlag);
}

//--------------------------------------------------------------
//  EmitIdComment           Emit an identifier and its
//                          modifiers as a comment.
//--------------------------------------------------------------

//--Tokens that can start an identifier modifier.
TTokenCode tlIdModStart[] = {tcLBracket, tcLParen, tcPeriod, tcDummy};

//--Tokens that can end an identifier modifier.
TTokenCode tlIdModEnd[]   = {tcRBracket, tcRParen, tcDummy};

void TCodeGenerator::EmitIdComment(void)
{
    PutComment(pToken->String());
    GetToken();

    //--Loop to print any modifiers (subscripts, record fields,
    //--or actual parameter lists).
    while (TokenIn(token, tlIdModStart)) {

	//--Record field.
	if (token == tcPeriod) {
	    PutComment(".");
	    GetToken();
	    EmitIdComment();
	}

	//--Subscripts or actual parameters.
	else {

	    //--( or [
	    PutComment(token == tcLParen ? "(" : "[");
	    GetToken();

	    while (!TokenIn(token, tlIdModEnd)) {
		EmitExprComment();

		//--Write and writeln field width and precision.
		while (token == tcColon) {
		    PutComment(":");
		    GetToken();
		    EmitExprComment();
		}

		if (token == tcComma) {
		    PutComment(", ");
		    GetToken();
		}
	    }

	    //--) or ]
	    PutComment(token == tcRParen ? ")" : "]");
	    GetToken();
	}
    }
}
//endfig

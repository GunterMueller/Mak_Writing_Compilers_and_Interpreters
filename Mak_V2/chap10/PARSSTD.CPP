//fig 8-20
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Standard Routines)                       *
//  *                                                           *
//  *   Parse calls to standard procedures and functions.       *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog8-1/parsstd.cpp                            *
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
//  ParseStandardSubroutineCall     Parse a call to a standard
//                                  procedure or function.
//
//      pRoutineId : ptr to the subroutine id's
//                   symbol table node
//
//  Return: ptr to the type object of the call
//--------------------------------------------------------------

TType *TParser::ParseStandardSubroutineCall
				(const TSymtabNode *pRoutineId)
{
    switch (pRoutineId->defn.routine.which) {

	case rcRead:
	case rcReadln:  return(ParseReadReadlnCall(pRoutineId));

	case rcWrite:
	case rcWriteln: return(ParseWriteWritelnCall(pRoutineId));

	case rcEof:
	case rcEoln:    return(ParseEofEolnCall());

	case rcAbs:
	case rcSqr:     return(ParseAbsSqrCall());

	case rcArctan:
	case rcCos:
	case rcExp:
	case rcLn:
	case rcSin:
	case rcSqrt:    return(ParseArctanCosExpLnSinSqrtCall());

	case rcPred:
	case rcSucc:    return(ParsePredSuccCall());

	case rcChr:     return(ParseChrCall());
	case rcOdd:     return(ParseOddCall());
	case rcOrd:     return(ParseOrdCall());

	case rcRound:
	case rcTrunc:   return(ParseRoundTruncCall());

	default:        return(pDummyType);
    }
}

//--------------------------------------------------------------
//  ParseReadReadlnCall     Parse a call to read or readln.
//                          Each actual parameter must be a
//                          scalar variable.
//
//      pRoutineId : ptr to the routine id's symbol table node
//
//  Return: ptr to the dummy type object
//--------------------------------------------------------------

TType *TParser::ParseReadReadlnCall(const TSymtabNode *pRoutineId)
{
    //--Actual parameters are optional for readln.
    if (token != tcLParen) {
	if (pRoutineId->defn.routine.which == rcRead) {
	    Error(errWrongNumberOfParms);
	}
	return pDummyType;
    }

    //--Loop to parse comma-separated list of actual parameters.
    do {
	//-- ( or ,
	GetTokenAppend();

	//--Each actual parameter must be a scalar variable,
	//--but parse an expression anyway for error recovery.
	if (token == tcIdentifier) {
	    TSymtabNode *pParmId = Find(pToken->String());
	    icode.Put(pParmId);
	    
	    if (ParseVariable(pParmId)->Base()->form
			!= fcScalar) Error(errIncompatibleTypes);
	}
	else {
	    ParseExpression();
	    Error(errInvalidVarParm);
	}

	//-- , or )
	Resync(tlActualVarParmFollow,
	       tlStatementFollow, tlStatementStart);
    } while (token == tcComma);

    //-- )
    CondGetTokenAppend(tcRParen, errMissingRightParen);

    return pDummyType;
}

//--------------------------------------------------------------
//  ParseWriteWritelnCall   Parse a call to write or writeln.
//                          Each actual parameter can be in any
//                          one of the following forms:
//
//                              <expr>
//                              <expr> : <expr>
//                              <expr> : <expr> : <expr>
//
//      pRoutineId : ptr to the routine id's symbol table node
//
//  Return: ptr to the dummy type object
//--------------------------------------------------------------

TType *TParser::ParseWriteWritelnCall(const TSymtabNode *pRoutineId)
{
    //--Actual parameters are optional only for writeln.
    if (token != tcLParen) {
	if (pRoutineId->defn.routine.which == rcWrite) {
	    Error(errWrongNumberOfParms);
	}
	return pDummyType;
    }

    //--Loop to parse comma-separated list of actual parameters.
    do {
	//-- ( or ,
	GetTokenAppend();

	//--Value <expr> : The type must be either a non-Boolean
	//--               scalar or a string.
	TType *pActualType = ParseExpression()->Base();
	if (   ((pActualType->form != fcScalar) ||
		(pActualType == pBooleanType))
	    && ((pActualType->form != fcArray)  ||
		(pActualType->array.pElmtType != pCharType)) ) {
	    Error(errIncompatibleTypes);
	}

	//--Optional field width <expr>
	if (token == tcColon) {
	    GetTokenAppend();
	    if (ParseExpression()->Base() != pIntegerType) {
		Error(errIncompatibleTypes);
	    }

	    //--Optional precision <expr>
	    if (token == tcColon) {
		GetTokenAppend();
		if (ParseExpression()->Base() != pIntegerType) {
		    Error(errIncompatibleTypes);
		}
	    }
	}
    } while (token == tcComma);

    //-- )
    CondGetTokenAppend(tcRParen, errMissingRightParen);

    return pDummyType;
}

//--------------------------------------------------------------
//  ParseEofEolnCall    Parse a call to eof or eoln.
//                      No parameters => boolean result
//
//  Return: ptr to the boolean type object
//--------------------------------------------------------------

TType *TParser::ParseEofEolnCall(void)
{
    //--There should be no actual parameters, but parse
    //--them anyway for error recovery.
    if (token == tcLParen) {
	Error(errWrongNumberOfParms);
	ParseActualParmList(NULL, 0);
    }

    return pBooleanType;
}

//--------------------------------------------------------------
//  ParseAbsSqrCall     Parse a call to abs or sqr.
//                      Integer parm => integer result
//                      Real parm    => real result
//
//  Return: ptr to the result's type object
//--------------------------------------------------------------

TType *TParser::ParseAbsSqrCall(void)
{
    TType *pResultType;  // ptr to result type object

    //--There should be one integer or real parameter.
    if (token == tcLParen) {
	GetTokenAppend();

	TType *pParmType = ParseExpression()->Base();
	if ((pParmType != pIntegerType) && (pParmType != pRealType)) {
	    Error(errIncompatibleTypes);
	    pResultType = pIntegerType;
	}
	else pResultType = pParmType;

	//--There better not be any more parameters.
	if (token != tcRParen) SkipExtraParms();

	//-- )
	CondGetTokenAppend(tcRParen, errMissingRightParen);
    }
    else Error(errWrongNumberOfParms);

    return pResultType;
}

//--------------------------------------------------------------
//  ParseArctanCosExpLnSinSqrtCall  Parse a call to arctan, cos,
//                                  exp, ln, sin, or sqrt.
//                                  Integer parm => real result
//                                  Real parm    => real result
//
//  Return: ptr to the real type object
//--------------------------------------------------------------

TType *TParser::ParseArctanCosExpLnSinSqrtCall(void)
{
    //--There should be one integer or real parameter.
    if (token == tcLParen) {
	GetTokenAppend();

	TType *pParmType = ParseExpression()->Base();
	if ((pParmType != pIntegerType) && (pParmType != pRealType)) {
	    Error(errIncompatibleTypes);
	}

	//--There better not be any more parameters.
	if (token != tcRParen) SkipExtraParms();

	//-- )
	CondGetTokenAppend(tcRParen, errMissingRightParen);
    }
    else Error(errWrongNumberOfParms);

    return pRealType;
}

//--------------------------------------------------------------
//  ParsePredSuccCall   Parse a call to pred or succ.
//                      Integer parm => integer result
//                      Enum parm    => enum result
//
//  Return: ptr to the result's type object
//--------------------------------------------------------------

TType *TParser::ParsePredSuccCall(void)
{
    TType *pResultType;  // ptr to result type object

    //--There should be one integer or enumeration parameter.
    if (token == tcLParen) {
	GetTokenAppend();

	TType *pParmType = ParseExpression()->Base();
	if ((pParmType != pIntegerType) &&
	    (pParmType->form != fcEnum)) {
	    Error(errIncompatibleTypes);
	    pResultType = pIntegerType;
	}
	else pResultType = pParmType;

	//--There better not be any more parameters.
	if (token != tcRParen) SkipExtraParms();

	//-- )
	CondGetTokenAppend(tcRParen, errMissingRightParen);
    }
    else Error(errWrongNumberOfParms);

    return pResultType;
}

//--------------------------------------------------------------
//  ParseChrCall        Parse a call to chr.
//                      Integer parm => character result
//
//  Return: ptr to the character type object
//--------------------------------------------------------------

TType *TParser::ParseChrCall(void)
{
    //--There should be one character parameter.
    if (token == tcLParen) {
	GetTokenAppend();

	TType *pParmType = ParseExpression()->Base();
	if (pParmType != pIntegerType) Error(errIncompatibleTypes);

	//--There better not be any more parameters.
	if (token != tcRParen) SkipExtraParms();

	//-- )
	CondGetTokenAppend(tcRParen, errMissingRightParen);
    }
    else Error(errWrongNumberOfParms);

    return pCharType;
}

//--------------------------------------------------------------
//  ParseOddCall        Parse a call to odd.
//                      Integer parm => boolean result
//
//  Return: ptr to the boolean type object
//--------------------------------------------------------------

TType *TParser::ParseOddCall(void)
{
    //--There should be one integer parameter.
    if (token == tcLParen) {
	GetTokenAppend();

	TType *pParmType = ParseExpression()->Base();
	if (pParmType != pIntegerType) Error(errIncompatibleTypes);

	//--There better not be any more parameters.
	if (token != tcRParen) SkipExtraParms();

	//-- )
	CondGetTokenAppend(tcRParen, errMissingRightParen);
    }
    else Error(errWrongNumberOfParms);

    return pBooleanType;
}

//--------------------------------------------------------------
//  ParseOrdCall        Parse a call to ord.
//                      Character parm => integer result
//                      Enum parm      => integer result
//
//  Return: ptr to the integer type object
//--------------------------------------------------------------

TType *TParser::ParseOrdCall(void)
{
    //--There should be one character or enumeration parameter.
    if (token == tcLParen) {
	GetTokenAppend();

	TType *pParmType = ParseExpression()->Base();
	if ((pParmType != pCharType) && (pParmType->form != fcEnum)) {
	    Error(errIncompatibleTypes);
	}

	//--There better not be any more parameters.
	if (token != tcRParen) SkipExtraParms();

	//-- )
	CondGetTokenAppend(tcRParen, errMissingRightParen);
    }
    else Error(errWrongNumberOfParms);

    return pIntegerType;
}

//--------------------------------------------------------------
//  ParseRoundTruncCall     Parse a call to round or trunc.
//                          Real parm => integer result
//
//  Return: ptr to the integer type object
//--------------------------------------------------------------

TType *TParser::ParseRoundTruncCall(void)
{
    //--There should be one real parameter.
    if (token == tcLParen) {
	GetTokenAppend();

	TType *pParmType = ParseExpression()->Base();
	if (pParmType != pRealType) Error(errIncompatibleTypes);

	//--There better not be any more parameters.
	if (token != tcRParen) SkipExtraParms();

	//-- )
	CondGetTokenAppend(tcRParen, errMissingRightParen);
    }
    else Error(errWrongNumberOfParms);

    return pIntegerType;
}

//--------------------------------------------------------------
//  SkipExtraParms      Skip extra parameters in a call to a
//                      standard procedure or function.
//
//      pSymtab : ptr to symbol table
//--------------------------------------------------------------

void TParser::SkipExtraParms(void)
{
    Error(errWrongNumberOfParms);

    while (token == tcComma) {
	GetTokenAppend();
	ParseExpression();
    }
}

//--------------------------------------------------------------
//  InitializeStandardRoutines  Initialize the standard
//                              routines by entering their
//                              identifiers into the symbol
//                              table.
//
//      pSymtab : ptr to symbol table
//--------------------------------------------------------------

static struct TStdRtn {
    char         *pName;
    TRoutineCode  rc;
    TDefnCode     dc;
} stdRtnList[] = {
    {"read",    rcRead,    dcProcedure},
    {"readln",  rcReadln,  dcProcedure},
    {"write",   rcWrite,   dcProcedure},
    {"writeln", rcWriteln, dcProcedure},
    {"abs",     rcAbs,     dcFunction},
    {"arctan",  rcArctan,  dcFunction},
    {"chr",     rcChr,     dcFunction},
    {"cos",     rcCos,     dcFunction},
    {"eof",     rcEof,     dcFunction},
    {"eoln",    rcEoln,    dcFunction},
    {"exp",     rcExp,     dcFunction},
    {"ln",      rcLn,      dcFunction},
    {"odd",     rcOdd,     dcFunction},
    {"ord",     rcOrd,     dcFunction},
    {"pred",    rcPred,    dcFunction},
    {"round",   rcRound,   dcFunction},
    {"sin",     rcSin,     dcFunction},
    {"sqr",     rcSqr,     dcFunction},
    {"sqrt",    rcSqrt,    dcFunction},
    {"succ",    rcSucc,    dcFunction},
    {"trunc",   rcTrunc,   dcFunction},
    {NULL},
};

void InitializeStandardRoutines(TSymtab *pSymtab)
{
    int i = 0;

    do {
	TStdRtn     *pSR        = &stdRtnList[i];
	TSymtabNode *pRoutineId = pSymtab->Enter(pSR->pName, pSR->dc);

	pRoutineId->defn.routine.which               = pSR->rc;
	pRoutineId->defn.routine.parmCount           = 0;
	pRoutineId->defn.routine.totalParmSize       = 0;
	pRoutineId->defn.routine.totalLocalSize      = 0;
	pRoutineId->defn.routine.locals.pParmIds     = NULL;
	pRoutineId->defn.routine.locals.pConstantIds = NULL;
	pRoutineId->defn.routine.locals.pTypeIds     = NULL;
	pRoutineId->defn.routine.locals.pVariableIds = NULL;
	pRoutineId->defn.routine.locals.pRoutineIds  = NULL;
	pRoutineId->defn.routine.pSymtab             = NULL;
	pRoutineId->defn.routine.pIcode              = NULL;
	SetType(pRoutineId->pType, pDummyType);
    } while (stdRtnList[++i].pName);
}
//endfig

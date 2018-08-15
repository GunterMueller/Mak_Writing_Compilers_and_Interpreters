//  *************************************************************
//  *                                                           *
//  *   M I S C E L L A N E O U S   (Header)			*
//  *                                                           *
//  *   FILE:    prog13-1/misc.h				*
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef misc_h
#define misc_h

const int false = 0;
const int true  = 1;

//--------------------------------------------------------------
//  Runtime stack frame offsets
//--------------------------------------------------------------

const int procLocalsStackFrameOffset =  0;
const int funcLocalsStackFrameOffset = -4;
const int parametersStackFrameOffset = +6;

//--------------------------------------------------------------
//  TCharCode           Character codes.
//--------------------------------------------------------------

enum TCharCode {
    ccLetter, ccDigit, ccSpecial, ccQuote, ccWhiteSpace,
    ccEndOfFile, ccError,
};

//--------------------------------------------------------------
//  TTokenCode          Token codes.
//--------------------------------------------------------------

enum TTokenCode {
    tcDummy,
    tcIdentifier, tcNumber, tcString, tcEndOfFile, tcError,

    tcUpArrow, tcStar, tcLParen, tcRParen, tcMinus, tcPlus,
    tcEqual, tcLBracket, tcRBracket, tcColon, tcSemicolon, tcLt,
    tcGt, tcComma, tcPeriod, tcSlash, tcColonEqual, tcLe, tcGe,
    tcNe, tcDotDot,
    
    tcAND, tcARRAY, tcBEGIN, tcCASE, tcCONST, tcDIV,
    tcDO, tcDOWNTO, tcELSE, tcEND, tcFILE, tcFOR, tcFUNCTION,
    tcGOTO, tcIF, tcIN, tcLABEL, tcMOD, tcNIL, tcNOT, tcOF, tcOR,
    tcPACKED, tcPROCEDURE, tcPROGRAM, tcRECORD, tcREPEAT, tcSET,
    tcTHEN, tcTO, tcTYPE, tcUNTIL, tcVAR, tcWHILE, tcWITH,
};

//--------------------------------------------------------------
//  TDataType           Data type.
//--------------------------------------------------------------

enum TDataType {
    tyDummy, tyInteger, tyReal, tyCharacter, tyString,
};

//--------------------------------------------------------------
//  TDataValue          Data value.
//--------------------------------------------------------------

union TDataValue {
    int    integer;
    float  real;
    char   character;
    char  *pString;
};

#endif


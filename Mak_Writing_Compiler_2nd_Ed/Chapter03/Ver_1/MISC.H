//  *************************************************************
//  *                                                           *
//  *   M I S C E L L A N E O U S   (Header)                    *
//  *                                                           *
//  *   FILE:    prog3-1/misc.h                                 *
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

//fig 3-4
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
    tcDummy, tcWord, tcNumber, tcPeriod, tcEndOfFile, tcError,
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
//endfig

#endif

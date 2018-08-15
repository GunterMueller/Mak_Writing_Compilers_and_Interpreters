//fig 3-5
//  *************************************************************
//  *                                                           *
//  *   S C A N N E R   (Header)                                *
//  *                                                           *
//  *   CLASSES: TScanner, TTextScanner                         *
//  *                                                           *
//  *   FILE:    prog3-1/scanner.h                              *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef scanner_h
#define scanner_h

#include "misc.h"
#include "buffer.h"
#include "token.h"

//--------------------------------------------------------------
//  TScanner            Abstract scanner class.
//--------------------------------------------------------------

class TScanner {

protected:

    //--Tokens extracted and returned by the scanner.
    TWordToken    wordToken;
    TNumberToken  numberToken;
    TStringToken  stringToken;
    TSpecialToken specialToken;
    TEOFToken     eofToken;
    TErrorToken   errorToken;

public:
    virtual ~TScanner(void) {}

    virtual TToken *Get(void) = 0;
};

//--------------------------------------------------------------
//  TTextScanner        Text scanner subclass of TScanner.
//--------------------------------------------------------------

class TTextScanner : public TScanner {
    TTextInBuffer *const pTextInBuffer;  // ptr to input text buffer
					 //   to scan

    void SkipWhiteSpace(void);

public:
    TTextScanner(TTextInBuffer *pBuffer);
    virtual ~TTextScanner(void) { delete pTextInBuffer; }

    virtual TToken *Get(void);
};

#endif

//fig 3-15
//  *************************************************************
//  *                                                           *
//  *   T O K E N S   (Header)                                  *
//  *                                                           *
//  *	CLASSES: TToken, TWordToken, TNumberToken, 		*
//  *		 TStringToken, TSpecialToken, TEOFToken,	*
//  *		 TErrorToken                                    *
//  *                                                           *
//  *   FILE:    prog3-2/token.h                                *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef token_h
#define token_h

#include "misc.h"
#include "error.h"
#include "buffer.h"

extern TCharCode charCodeMap[];

//--------------------------------------------------------------
//  TToken              Abstract token class.
//--------------------------------------------------------------

class TToken {

protected:
    TTokenCode code;
    TDataType  type;
    TDataValue value;
    char       string[maxInputBufferSize];

public:
    TToken(void)
    {
	code = tcDummy;
	type = tyDummy;
	value.integer = 0;
	string[0]     = '\0';
    }

    TTokenCode  Code()   const { return code;   }
    TDataType   Type()   const { return type;   }
    TDataValue  Value()  const { return value;  }
    char       *String()       { return string; }

    virtual void Get(TTextInBuffer &buffer) = 0;
    virtual int  IsDelimiter(void) const = 0;
    virtual void Print      (void) const = 0;
};

//--------------------------------------------------------------
//  TWordToken          Word token subclass of TToken.
//--------------------------------------------------------------

class TWordToken : public TToken {
    void CheckForReservedWord(void);

public:
    virtual void Get(TTextInBuffer &buffer);
    virtual int  IsDelimiter(void) const { return false; }
    virtual void Print      (void) const;
};

//--------------------------------------------------------------
//  TNumberToken        Number token subclass of TToken.
//--------------------------------------------------------------

class TNumberToken : public TToken {
    char  ch;              // char fetched from input buffer
    char *ps;              // ptr into token string
    int   digitCount;      // total no. of digits in number
    int   countErrorFlag;  // true if too many digits, else false

    int AccumulateValue(TTextInBuffer &buffer,
			float &value, TErrorCode ec);

public:
    TNumberToken() { code = tcNumber; }

    virtual void Get(TTextInBuffer &buffer);
    virtual int  IsDelimiter(void) const { return false; }
    virtual void Print      (void) const;
};

//--------------------------------------------------------------
//  TStringToken        String token subclass of TToken.
//--------------------------------------------------------------

class TStringToken : public TToken {

public:
    TStringToken() { code = tcString; }

    virtual void Get(TTextInBuffer &buffer);
    virtual int  IsDelimiter(void) const { return true; }
    virtual void Print      (void) const;
};

//--------------------------------------------------------------
//  TSpecialToken       Special token subclass of TToken.
//--------------------------------------------------------------

class TSpecialToken : public TToken {

public:
    virtual void Get(TTextInBuffer &buffer);
    virtual int  IsDelimiter(void) const { return true; }
    virtual void Print      (void) const;
};

//--------------------------------------------------------------
//  TEOFToken           End-of-file token subclass of TToken.
//--------------------------------------------------------------

class TEOFToken : public TToken {

public:
    TEOFToken() { code = tcEndOfFile; }

    virtual void Get(TTextInBuffer &buffer) {}
    virtual int  IsDelimiter(void) const { return false; }
    virtual void Print      (void) const {}
};

//--------------------------------------------------------------
//  TErrorToken         Error token subclass of TToken.
//--------------------------------------------------------------

class TErrorToken : public TToken {

public:
    TErrorToken() { code = tcError; }

    virtual void Get(TTextInBuffer &buffer);
    virtual int  IsDelimiter(void) const { return false; }
    virtual void Print      (void) const {}
};

#endif

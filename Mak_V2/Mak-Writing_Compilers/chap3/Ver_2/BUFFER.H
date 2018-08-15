//fig 2-3
//  *************************************************************
//  *                                                           *
//  *   I / O   B U F F E R S   (Header)                        *
//  *                                                           *
//  *   CLASSES: TTextInBuffer,  TSourceBuffer                  *
//  *            TTextOutBuffer, TListBuffer                    *
//  *                                                           *
//  *   FILE:    prog2-1/buffer.h                               *
//  *                                                           *
//  *   MODULE:  Buffer                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef buffer_h
#define buffer_h

#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "error.h"

//              ***********
//              *         *
//              *  Input  *
//              *         *
//              ***********

extern char eofChar;
extern int  inputPosition;
extern int  listFlag;
extern int  level;

const int maxInputBufferSize = 256;

//--------------------------------------------------------------
//  TTextInBuffer       Abstract text input buffer class.
//--------------------------------------------------------------

class TTextInBuffer {

protected:
    fstream  file;                      // input text file
    char    *const pFileName;           // ptr to the file name
    char     text[maxInputBufferSize];  // input text buffer
    char    *pChar;                     // ptr to the current char
					//   in the text buffer

    virtual char GetLine(void) = 0;

public:
    TTextInBuffer(const char *pInputFileName, TAbortCode ac);

    virtual ~TTextInBuffer(void)
    {
	file.close();
	delete pFileName;
    }

    char Char       (void) const { return *pChar; }
    char GetChar    (void);
    char PutBackChar(void);
};

//--------------------------------------------------------------
//  TSourceBuffer       Source buffer subclass of TTextInBuffer.
//--------------------------------------------------------------

class TSourceBuffer : public TTextInBuffer {
    virtual char GetLine(void);

public:
    TSourceBuffer(const char *pSourceFileName);
};

//              ************
//              *          *
//              *  Output  *
//              *          *
//              ************

//--------------------------------------------------------------
//  TTextOutBuffer      Abstract text output buffer class.
//--------------------------------------------------------------

class TTextOutBuffer {

public:
    char text[maxInputBufferSize + 16];  // output text buffer

    virtual void PutLine(void) = 0;

    void PutLine(const char *pText)
    {
	strcpy(text, pText);
	PutLine();
    }
};

//--------------------------------------------------------------
//  TListBuffer         List buffer subclass of TTextOutBuffer.
//--------------------------------------------------------------

class TListBuffer : public TTextOutBuffer {
    char *pSourceFileName;  // ptr to source file name (for page header)
    char  date[26];         // date string for page header
    int   pageNumber;       // current page number
    int   lineCount;        // count of lines in the current page

    void PrintPageHeader(void);

public:
    virtual ~TListBuffer(void) { delete pSourceFileName; }

    void Initialize(const char *fileName);
    virtual void PutLine(void);

    void PutLine(const char *pText)
    {
	TTextOutBuffer::PutLine(pText);
    }

    void PutLine(const char *pText, int lineNumber, int nestingLevel)
    {
	sprintf(text, "%4d %d: %s", lineNumber, nestingLevel, pText);
	PutLine();
    }
};

extern TListBuffer list;

#endif
//endfig


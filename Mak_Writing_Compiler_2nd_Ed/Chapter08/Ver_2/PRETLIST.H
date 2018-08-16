//fig 8-24
//  *************************************************************
//  *                                                           *
//  *   P R E T T Y   P R I N T   L I S T I N G   (Header)      *
//  *                                                           *
//  *   CLASSES: TPrettyListBuffer                              *
//  *                                                           *
//  *   FILE:    prog8-2/pretlist.h                             *
//  *                                                           *
//  *   MODULE:  Buffer                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef pretlist_h
#define pretlist_h

#include "buffer.h"

//--------------------------------------------------------------
//  TPrettyListBuffer       Pretty print list buffer subclass
//                          of TTextOutBuffer.
//--------------------------------------------------------------

class TPrettyListBuffer : public TTextOutBuffer {
    enum {
	indentSize          =  4,
	maxMargin           = 60,
	maxPrettyTextLength = 80,
    };

    int   textLength;  // length of output text line
    char *pText;       // pointer to end of text
    int   margin;      // left margin (number of spaces)

public:
    TPrettyListBuffer(void)
    {
	pText      = text;
	*pText     = '\0';
	textLength = margin = 0;
    }

    void Put(const char *pString);
    virtual void PutLine(void);

    void PutLine(const char *pText)
    {
	Put(pText);
	PutLine();
    }

    void Indent(void) { margin += indentSize; }
    void Dedent(void) { if ((margin -= indentSize) < 0) margin = 0; }

    int SetMargin(void)
    {
	int m  = margin;      // save current margin
	margin = textLength;  // set margin at current position
	return m;             // return old margin
    }

    void ResetMargin(int m) { margin = m; }
};

#endif
//endfig

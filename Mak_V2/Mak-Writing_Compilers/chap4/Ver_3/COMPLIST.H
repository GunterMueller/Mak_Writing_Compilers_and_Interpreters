//fig 3-25
//  *************************************************************
//  *                                                           *
//  *   C O M P A C T   L I S T I N G   (Header)                *
//  *                                                           *
//  *	CLASSES: TCompactListBuffer				*
//  *                                                           *
//  *   FILE:    prog3-3/complist.h                             *
//  *                                                           *
//  *   MODULE:  Buffer                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef complist_h
#define complist_h

#include "buffer.h"

const int maxCompactTextLength = 72;

//--------------------------------------------------------------
//  TCompactListBuffer	    Compact list buffer subclass
//			    of TTextOutBuffer.
//--------------------------------------------------------------

class TCompactListBuffer : public TTextOutBuffer {
    int   textLength;  // length of output text line
    char *pText;       // pointer to end of text

public:
    TCompactListBuffer(void)
    {
	pText      = text;
	*pText     = '\0';
	textLength = 0;
    }

    void PutBlank(void);
    void Put     (const char *pString);
    void PutLine (void);
};

#endif
//endfig

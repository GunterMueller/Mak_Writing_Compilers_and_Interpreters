//fig 5-25
//  *************************************************************
//  *                                                           *
//  *   B A C K E N D  (Header)                                 *
//  *                                                           *
//  *   CLASSES: TBackend                                       *
//  *                                                           *
//  *   FILE:    prog5-2/backend.h                              *
//  *                                                           *
//  *   MODULE:  Back end                                       *
//  *                                                           *
//  *   Copyright (c) 1995 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef backend_h
#define backend_h

#include "misc.h"
#include "symtab.h"
#include "token.h"
#include "icode.h"

extern TIcode icode;

//--------------------------------------------------------------
//  TBackend            Abstract back end class.
//--------------------------------------------------------------

class TBackend {

protected:
    TToken      *pToken;  // ptr to the current token
    TTokenCode   token;   // code of current token
    TSymtabNode *pNode;   // ptr to symtab node

    void GetToken(void)
    {
	pToken = icode.Get();
	token  = pToken->Code();
	pNode  = icode.SymtabNode();
    }

    void GoTo(int location) { icode.GoTo(location); }

    int CurrentLocation(void) const {
	return icode.CurrentLocation();
    }

public:
    virtual ~TBackend(void) {}

    virtual void Go(void) = 0;
};

#endif
//endfig


//fig 5-15
//  *************************************************************
//  *                                                           *
//  *   I N T E R M E D I A T E   C O D E   (Header)            *
//  *                                                           *
//  *	CLASSES: TIcode						*
//  *                                                           *
//  *   FILE:    prog5-2/icode.h                                *
//  *                                                           *
//  *   MODULE:  Intermediate code                              *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef icode_h
#define icode_h

#include <fstream.h>
#include "misc.h"
#include "symtab.h"
#include "token.h"
#include "scanner.h"

const TTokenCode mcLineMarker = ((TTokenCode) 127);

//--------------------------------------------------------------
//  TIcode      Intermediate code subclass of TScanner.
//--------------------------------------------------------------

class TSymtabNode;

class TIcode : public TScanner {
    enum {codeSegmentSize = 4096};

    char        *pCode;   // ptr to the code segment
    char        *cursor;  // ptr to current code location
    TSymtabNode *pNode;   // ptr to extracted symbol table node

    void         CheckBounds  (int size);
    TSymtabNode *GetSymtabNode(void);

public:
    TIcode(void) { pCode = cursor = new char[codeSegmentSize]; }
    TIcode(const TIcode &icode);  // copy constructor
   ~TIcode(void) { delete[] pCode; }

    void Put(TTokenCode tc);
    void Put(const TSymtabNode *pNode);
    void InsertLineMarker(void);

    void Reset(void)	     { cursor = pCode;            }
    void GoTo (int location) { cursor = pCode + location; }

    int          CurrentLocation(void) const { return cursor - pCode; }
    TSymtabNode *SymtabNode     (void) const { return pNode;          }

    virtual TToken *Get(void);
};

#endif
//endfig

//  *************************************************************
//  *                                                           *
//  *   I N T E R M E D I A T E   C O D E   (Header)            *
//  *                                                           *
//  *   CLASSES: TIcode                                         *
//  *                                                           *
//  *   FILE:    prog10-1/icode.h                               *
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
#include "token.h"
#include "scanner.h"

const TTokenCode mcLineMarker     = ((TTokenCode) 127);
const TTokenCode mcLocationMarker = ((TTokenCode) 126);

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
    TIcode(const TIcode &icode);  // copy constructor
    TIcode(void) { pCode = cursor = new char[codeSegmentSize]; }
   ~TIcode(void) { delete[] pCode; }

    void Put(TTokenCode tc);
    void Put(const TSymtabNode *pNode);
    void InsertLineMarker   (void);
    int  PutLocationMarker  (void);
    void FixupLocationMarker(int location);
    int  GetLocationMarker  (void);
    void PutCaseItem(int value, int location);
    void GetCaseItem(int &value, int &location);

    void Reset(void)	     { cursor = pCode;            }
    void GoTo (int location) { cursor = pCode + location; }

    int          CurrentLocation(void) const { return cursor - pCode; }
    TSymtabNode *SymtabNode     (void) const { return pNode;          }

    virtual TToken *Get(void);
};

#endif

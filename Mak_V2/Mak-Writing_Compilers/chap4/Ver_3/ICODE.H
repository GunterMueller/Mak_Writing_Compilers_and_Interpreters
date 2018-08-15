//fig 4-10
//  *************************************************************
//  *                                                           *
//  *   I N T E R M E D I A T E   C O D E   (Header)            *
//  *                                                           *
//  *	CLASSES: TIcode						*
//  *                                                           *
//  *   FILE:    prog4-2/icode.h                                *
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

//--------------------------------------------------------------
//  TIcode	Intermediate code subclass of TScanner.
//--------------------------------------------------------------

class TSymtabNode;

class TIcode : public TScanner {
    fstream     file;             // intermediate code file
    const char *const pFileName;  // ptr to the file name
    int         size;             // byte size of file

    char **symtabStrings;  // vector of symbol table strings
    int    symtabCount;    // count of strings in the vector

public:
    enum TMode {input, output};

    TIcode(const char *pTIcodeFileName, TMode mode);
   ~TIcode(void);

    void GoTo(int location) { file.seekg(location, ios::beg); }
    int  Size(void) const   { return size; }

    void Put(int value);
    void Put(TTokenCode tc, short index = -1);
    void Put(const TSymtabNode *pNode);

    virtual TToken *Get	      (void);
    int             GetInteger(void);
    void            GetSymtabStrings(void);
};

#endif
//endfig

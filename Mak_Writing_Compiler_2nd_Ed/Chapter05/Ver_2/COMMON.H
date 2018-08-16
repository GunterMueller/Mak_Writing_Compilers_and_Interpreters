//  *************************************************************
//  *                                                           *
//  *   C O M M O N   (Header)                                  *
//  *                                                           *
//  *   FILE:    prog5-2/common.h                               *
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef common_h
#define common_h

#include "misc.h"
#include "symtab.h"
#include "icode.h"

extern int currentLineNumber;
extern int currentNestingLevel;

extern TSymtab   globalSymtab;
extern int       cntSymtabs;
extern TSymtab  *pSymtabList;
extern TSymtab **vpSymtabs;

extern TIcode icode;

#endif


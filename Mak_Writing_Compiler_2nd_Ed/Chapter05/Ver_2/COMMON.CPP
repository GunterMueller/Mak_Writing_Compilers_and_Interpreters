//  *************************************************************
//  *                                                           *
//  *   C O M M O N                                             *
//  *                                                           *
//  *   FILE:    prog5-2/common.cpp                             *
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Data and routines common to the front and back ends.    *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "common.h"

int currentNestingLevel = 0;
int currentLineNumber   = 0;

TSymtab   globalSymtab;        // the global symbol table
int       cntSymtabs  = 0;     // symbol table counter
TSymtab  *pSymtabList = NULL;  // ptr to head of symtab list
TSymtab **vpSymtabs;           // ptr to vector of symtab ptrs

TIcode icode;                  // the intermediate code


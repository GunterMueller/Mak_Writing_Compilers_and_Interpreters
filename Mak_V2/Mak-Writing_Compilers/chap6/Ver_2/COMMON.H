//  *************************************************************
//  *                                                           *
//  *   C O M M O N   (Header)                                  *
//  *                                                           *
//  *   FILE:    prog6-1/common.h                               *
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

//fig 6-3
//--------------------------------------------------------------
//  Token lists
//--------------------------------------------------------------

extern const TTokenCode tlStatementStart[], tlStatementFollow[];
extern const TTokenCode tlCaseLabelStart[];

extern const TTokenCode tlExpressionStart[], tlExpressionFollow[];
extern const TTokenCode tlRelOps[], tlUnaryOps[],
			tlAddOps[], tlMulOps[];

extern const TTokenCode tlProgramEnd[];

extern const TTokenCode tlColonEqual[];
extern const TTokenCode tlDO[];
extern const TTokenCode tlTHEN[];
extern const TTokenCode tlTODOWNTO[];
extern const TTokenCode tlOF[];
extern const TTokenCode tlColon[];
extern const TTokenCode tlEND[];

int TokenIn(TTokenCode tc, const TTokenCode *pList);
//endfig

#endif


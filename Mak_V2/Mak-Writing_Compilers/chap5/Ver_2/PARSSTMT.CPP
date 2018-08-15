//fig 5-21
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Statements)                              *
//  *                                                           *
//  *	Parse statements.					*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog5-2/parsstmt.cpp                           *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "parser.h"

//--------------------------------------------------------------
//  ParseStatement      Parse a statement.
//--------------------------------------------------------------

void TParser::ParseStatement(void)
{
    InsertLineMarker();

    //--Only assignment statements for now.
    if (token == tcIdentifier) ParseAssignment();
}

//--------------------------------------------------------------
//  ParseAssignment     Parse an assignment statement.
//
//                              <id> := <expr>
//--------------------------------------------------------------

void TParser::ParseAssignment(void)
{
    //--Search for the target variable's identifier and enter it
    //--if necessary.  Append the symbol table node handle
    //--to the icode.
    TSymtabNode       *pTargetNode = SearchAll (pToken->String());
    if (!pTargetNode)  pTargetNode = EnterLocal(pToken->String());

    icode.Put(pTargetNode);
    GetTokenAppend();

    //-- :=
    if (token == tcColonEqual) GetTokenAppend();
    else                       Error(errMissingColonEqual);

    //--<expr>
    ParseExpression();
}
//endfig


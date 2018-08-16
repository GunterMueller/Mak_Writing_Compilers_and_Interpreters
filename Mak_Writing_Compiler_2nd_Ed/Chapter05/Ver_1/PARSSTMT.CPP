//fig 5-11
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Statements)                              *
//  *                                                           *
//  *	Parse statements.					*
//  *                                                           *
//  *   CLASSES: TParser					*
//  *                                                           *
//  *   FILE:    prog5-1/parsstmt.cpp                           *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "parser.h"

//--------------------------------------------------------------
//  ParseStatement          Parse a statement.
//--------------------------------------------------------------

void TParser::ParseStatement(void)
{
    //--Only assignment statements for now.
    if (token == tcIdentifier) ParseAssignment();
}

//--------------------------------------------------------------
//  ParseAssignment         Parse an assignment statement.
//                          Print the assigned value of the
//                          target variable.
//--------------------------------------------------------------

void TParser::ParseAssignment(void)
{
    //--Search for the target variable's identifier and enter it
    //--if necessary.
    TSymtabNode       *pTargetNode = SearchAll (pToken->String());
    if (!pTargetNode)  pTargetNode = EnterLocal(pToken->String());

    //-- :=
    GetToken();
    if (token == tcColonEqual) GetToken();
    else                       Error(errMissingColonEqual);

    //--Parse the expression and pop its value into the
    //--target variable's symbol table node.
    ParseExpression();
    pTargetNode->value = runStack.Pop();

    //--Print the target variable's identifier and value.
    sprintf(list.text, "\t>> %s := %g", pTargetNode->String(),
					pTargetNode->value);
    list.PutLine();
}
//endfig

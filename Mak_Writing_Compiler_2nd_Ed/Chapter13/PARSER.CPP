//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *   Parse a Pascal source program.                          *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog11-1/parser.cpp                            *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "common.h"
#include "parser.h"

//--------------------------------------------------------------
//  Parse       Parse the source file.
//
//  Return: ptr to the program id's symbol table node
//--------------------------------------------------------------

TSymtabNode *TParser::Parse(void)
{
    //--Extract the first token and parse the program.
    GetToken();
    TSymtabNode *pProgramId = ParseProgram();

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.",  currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();

    return pProgramId;
}

//fig 11-12
//--------------------------------------------------------------
//  ParseCommandExpression      Parse the expression part of the
//                              debugger "show" command.
//
//      pCmdIcode : ref to ptr to command icode
//      cmdToken  : ref to current command token
//--------------------------------------------------------------

void TParser::ParseCommandExpression(TIcode     *&pCmdIcode,
				     TTokenCode  &cmdToken)
{
    icode.Reset();
    GetTokenAppend();  // first token of expression

    //--Parse the expression.
    ParseExpression();
    pCmdIcode = new TIcode(icode);  // copy of expression icode
    cmdToken  = token;              // transfer token to debugger

    //--Convert the current symbol table again in case new nodes
    //--were inserted by the expression.
    TSymtab *pSymtab = symtabStack.GetCurrentSymtab();
    delete pSymtab->NodeVector();
    pSymtab->Convert(vpSymtabs);
}

//--------------------------------------------------------------
//  ParseCommandAssignment      Parse the assignment statement
//                              part of the debugger "assign"
//                              command.
//
//      pCmdIcode : ref to ptr to command icode
//      cmdToken  : ref to current command token
//
//  Return: ptr to the symbol table node of the target variable
//--------------------------------------------------------------

TSymtabNode *TParser::ParseCommandAssignment(TIcode     *&pCmdIcode,
					     TTokenCode  &cmdToken)
{
    TSymtabNode *pTargetId = NULL;

    icode.Reset();
    GetTokenAppend();  // first token of target variable

    if (token == tcIdentifier) {
	pTargetId = Find(pToken->String());
	icode.Put(pTargetId);

	//--Parse the assignment.
	ParseAssignment(pTargetId);
	pCmdIcode = new TIcode(icode);  // copy of statement icode
	cmdToken  = token;              // transfer token to debugger

	//--Convert the current symbol table again in case new
	//--nodes were inserted by the assignment expression.
	TSymtab *pSymtab = symtabStack.GetCurrentSymtab();
	delete pSymtab->NodeVector();
	pSymtab->Convert(vpSymtabs);
    }
    else Error(errUnexpectedToken);

    return pTargetId;
}
//endfig

//--------------------------------------------------------------
//  Resync          Resynchronize the parser.  If the current
//                  token is not in one of the token lists,
//                  flag it as an error and then skip tokens
//                  up to one that is in a list or end of file.
//--------------------------------------------------------------

void TParser::Resync(const TTokenCode *pList1,
		     const TTokenCode *pList2,
		     const TTokenCode *pList3)
{
    //--Is the current token in one of the lists?
    int errorFlag = (! TokenIn(token, pList1)) &&
		    (! TokenIn(token, pList2)) &&
		    (! TokenIn(token, pList3));

    if (errorFlag) {

	//--Nope.  Flag it as an error.
	TErrorCode errorCode = token == tcEndOfFile
				    ? errUnexpectedEndOfFile
				    : errUnexpectedToken;
	Error(errorCode);

	//--Skip tokens.
	while ((! TokenIn(token, pList1)) &&
	       (! TokenIn(token, pList2)) &&
	       (! TokenIn(token, pList3)) &&
	       (token != tcPeriod)        &&
	       (token != tcEndOfFile)) {
	    GetToken();
	}

	//--Flag an unexpected end of file (if haven't already).
	if ((token == tcEndOfFile) &&
	    (errorCode != errUnexpectedEndOfFile)) {
	    Error(errUnexpectedEndOfFile);
	}
    }
}


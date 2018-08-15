//fig 8-30
//  *************************************************************
//  *                                                           *
//  *   P R E T T Y   P R I N T E R   (Statements)              *
//  *                                                           *
//  *   Pretty-print statements and expressions.                *
//  *                                                           *
//  *   CLASSES: TPrettyPrinter                                 *
//  *                                                           *
//  *   FILE:    prog8-2/ppstmt.cpp                             *
//  *                                                           *
//  *   MODULE:  Pretty printer                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "common.h"
#include "pprinter.h"

//              *****************************
//              *                           *
//              *  Pretty-Print Statements  *
//              *                           *
//              *****************************

//--------------------------------------------------------------
//  PrintStatement      Pretty-print a statement.
//--------------------------------------------------------------

void TPrettyPrinter::PrintStatement(void)
{
    switch (token) {
	case tcIdentifier:  PrintAssignmentOrCall();    break;
	case tcBEGIN:       PrintCompound();            break;
	case tcREPEAT:      PrintREPEAT();              break;
	case tcWHILE:       PrintWHILE();               break;
	case tcIF:          PrintIF();                  break;
	case tcFOR:         PrintFOR();                 break;
	case tcCASE:        PrintCASE();                break;
    }

    while (token == tcSemicolon) {
	Put(";");
	GetToken();
    }
    PutLine();
}

//--------------------------------------------------------------
//  PrintStatementList      Pretty-print a statement list:
//
//                              <stmt-1>;
//                              <stmt-2>;
//                              ...
//--------------------------------------------------------------

void TPrettyPrinter::PrintStatementList(TTokenCode terminator)
{
    while (token != terminator) PrintStatement();
}

//--------------------------------------------------------------
//  PrintAssignmentOrCall       Pretty-print an assignment
//                              statement or a procedure call:
//
//                                  <id>
//                                  <id> := <expr>
//--------------------------------------------------------------

void TPrettyPrinter::PrintAssignmentOrCall(void)
{
    PrintIdentifier();

    if (token == tcColonEqual) {
	Put(" := ");

	GetToken();
	PrintExpression();
    }
}

//--------------------------------------------------------------
//  PrintREPEAT         Pretty-print a REPEAT statement:
//
//                          REPEAT
//                              <stmt-list>
//                          UNTIL <expr>
//--------------------------------------------------------------

void TPrettyPrinter::PrintREPEAT(void)
{
    PutLine("REPEAT");
    Indent();

    GetToken();
    PrintStatementList(tcUNTIL);

    Dedent();
    Put("UNTIL ");
    int saveMargin = SetMargin();

    GetToken();
    PrintExpression();
    ResetMargin(saveMargin);
}

//--------------------------------------------------------------
//  PrintWHILE          Pretty-print a WHILE statement:
//
//                          WHILE <expr> DO
//                              <stmt>
//--------------------------------------------------------------

void TPrettyPrinter::PrintWHILE(void)
{
    Put("WHILE ");
    int saveMargin = SetMargin();

    GetToken();
    PrintExpression();
    ResetMargin(saveMargin);
    PutLine(" DO");

    Indent();
    GetToken();
    PrintStatement();
    Dedent();
}

//--------------------------------------------------------------
//  PrintIF             Pretty-print an IF statement:
//
//                          IF <expr> THEN
//                              <stmt-1>
//                          ELSE
//                              <stmt-2>
//--------------------------------------------------------------

void TPrettyPrinter::PrintIF(void)
{
    Put("IF ");

    GetToken();
    PrintExpression();
    PutLine(" THEN");

    Indent();
    GetToken();
    PrintStatement();
    Dedent();

    if (token == tcELSE) {
	PutLine("ELSE");

	Indent();
	GetToken();
	PrintStatement();
	Dedent();
    }
}

//--------------------------------------------------------------
//  PrintFOR            Pretty-print a FOR statement:
//
//                          FOR <id> := <expr-1> TO <expr-2> DO
//                              <stmt>
//--------------------------------------------------------------

void TPrettyPrinter::PrintFOR(void)
{
    Put("FOR ");

    GetToken();
    PrintIdentifier();
    Put(" := ");

    GetToken();
    PrintExpression();
    Put(token == tcTO ? " TO " : " DOWNTO ");

    GetToken();
    PrintExpression();
    PutLine(" DO");

    Indent();
    GetToken();
    PrintStatement();
    Dedent();
}

//--------------------------------------------------------------
//  PrintCASE           Pretty-print a CASE statement:
//
//                          CASE <expr> OF
//                              <const-1>, <const-2>:
//                                  <stmt-1>;
//                              ...
//                          END
//--------------------------------------------------------------

void TPrettyPrinter::PrintCASE(void)
{
    Put("CASE ");

    GetToken();
    PrintExpression();
    PutLine(" OF ");

    Indent();
    GetToken();

    //--Loop to print CASE branches.
    while (token != tcEND) {

	//--Loop to print the CASE labels of a branch.
	do {
	    PrintExpression();
	    if (token == tcComma) {
		Put(", ");
		GetToken();
	    }
	} while (token != tcColon);

	PutLine(":");

	Indent();
	GetToken();
	PrintStatement();
	Dedent();
    }

    Dedent();
    Put("END");

    GetToken();
}

//--------------------------------------------------------------
//  PrintCompound       Pretty-print a compund statement:
//
//                          BEGIN
//                              <stmt-list>
//                          END
//--------------------------------------------------------------

void TPrettyPrinter::PrintCompound(void)
{
    PutLine("BEGIN");
    Indent();

    GetToken();
    PrintStatementList(tcEND);

    Dedent();
    Put("END");

    GetToken();
}

//              ******************************
//              *                            *
//              *  Pretty-Print Expressions  *
//              *                            *
//              ******************************

//--------------------------------------------------------------
//  PrintExpression     Pretty-print an expression.
//--------------------------------------------------------------

void TPrettyPrinter::PrintExpression(void)
{
    int doneFlag = false;  // true if done with expression, false if not

    //--Loop over the entire expression.
    do {
	switch (token) {
	    case tcIdentifier:  PrintIdentifier();  break;

	    case tcNumber:  Put(pToken->String());  GetToken();  break;
	    case tcString:  Put(pToken->String());  GetToken();  break;

	    case tcPlus:        Put(" + ");     GetToken();     break;
	    case tcMinus:       Put(" - ");     GetToken();     break;
	    case tcStar:        Put("*");       GetToken();     break;
	    case tcSlash:       Put("/");       GetToken();     break;
	    case tcDIV:         Put(" DIV ");   GetToken();     break;
	    case tcMOD:         Put(" MOD ");   GetToken();     break;
	    case tcAND:         Put(" AND ");   GetToken();     break;
	    case tcOR:          Put(" OR ");    GetToken();     break;
	    case tcEqual:       Put(" = ");     GetToken();     break;
	    case tcNe:          Put(" <> ");    GetToken();     break;
	    case tcLt:          Put(" < ");     GetToken();     break;
	    case tcLe:          Put(" <= ");    GetToken();     break;
	    case tcGt:          Put(" > ");     GetToken();     break;
	    case tcGe:          Put(" >= ");    GetToken();     break;
	    case tcNOT:         Put("NOT ");    GetToken();     break;

	    case tcLParen:
		Put("(");
		GetToken();
		PrintExpression();
		Put(")");
		GetToken();
		break;

	    default:
		doneFlag = true;
		break;
	}
    } while (!doneFlag);
}

//--------------------------------------------------------------
//  PrintIdentifier     Pretty-print an identifier, possibly
//                      followed by modifiers (subscripts,
//                      actual parameters, or fields):
//
//                          <id>
//                          <id>[<expr-1>, <expr-2>, ...]
//                          <id>(<expr-1>, <expr-2>, ...)
//                          <id-1>.<id-2>
//--------------------------------------------------------------

//--Tokens that can start an identifier modifier.
TTokenCode tlIdModStart[] = {tcLBracket, tcLParen, tcPeriod, tcDummy};

//--Tokens that can end an identifier modifier.
TTokenCode tlIdModEnd[]   = {tcRBracket, tcRParen, tcDummy};

void TPrettyPrinter::PrintIdentifier(void)
{
    Put(pToken->String());
    GetToken();

    //--Loop to print any modifiers (subscripts, record fields,
    //--or actual parameter lists).
    while (TokenIn(token, tlIdModStart)) {

	//--Record field.
	if (token == tcPeriod) {
	    Put(".");
	    GetToken();
	    PrintIdentifier();
	}

	//--Subscripts or actual parameters.
	else {
	    int saveMargin;

	    //--Set margin for actual parameters.
	    if (token == tcLParen) {
		Put("(");
		saveMargin = SetMargin();
	    }
	    else Put("[");

	    GetToken();

	    while (!TokenIn(token, tlIdModEnd)) {
		PrintExpression();

		//--Write and writeln field width and precision.
		while (token == tcColon) {
		    Put(":");
		    GetToken();
		    PrintExpression();
		}

		if (token == tcComma) {
		    Put(", ");
		    GetToken();
		}
	    }

	    //--Reset actual parameter margin.
	    if (token == tcRParen) {
		Put(")");
		ResetMargin(saveMargin);
	    }
	    else Put("]");

	    GetToken();
	}
    }
}
//endfig

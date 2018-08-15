//fig 5-27
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R                                         *
//  *                                                           *
//  *   Execute the intermediate code.             		*
//  *                                                           *
//  *	CLASSES: TExecutor					*
//  *                                                           *
//  *   FILE:    prog5-2/exec.cpp				*
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "exec.h"

//--------------------------------------------------------------
//  Go                  Start the executor.
//--------------------------------------------------------------

void TExecutor::Go(void)
{
    //--Reset the icode to the beginning
    //--and extract the first token.
    icode.Reset();
    GetToken();

    //--Loop to execute statements until the end of the program.
    do {
	ExecuteStatement();

	//--Skip semicolons.
	while (token == tcSemicolon) GetToken();

    } while (token != tcPeriod);

    //--Print the executor's summary.
    cout << endl;
    cout << "Successful completion.  " << stmtCount
	 << " statements executed." << endl;
}
//endfig


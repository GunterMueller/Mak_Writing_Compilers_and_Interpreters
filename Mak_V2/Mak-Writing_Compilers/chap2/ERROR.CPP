//fig 2-6
//  *************************************************************
//  *                                                           *
//  *   E R R O R S                                             *
//  *                                                           *
//  *   Routines to handle translation-time and runtime errors. *
//  *                                                           *
//  *   FILE:    prog2-1/error.cpp                              *
//  *                                                           *
//  *   MODULE:  Error                                          *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdlib.h>
#include <iostream.h>
#include "error.h"

int errorCount = 0;     // count of syntax errors

//--------------------------------------------------------------
//  Abort messages      Keyed to enumeration type TAbortCode.
//--------------------------------------------------------------

static char *abortMsg[] = {
    NULL,
    "Invalid command line arguments",
    "Failed to open source file",
    "Failed to open intermediate form file",
    "Failed to open assembly file",
    "Too many syntax errors",
    "Stack overflow",
    "Code segment overflow",
    "Nesting too deep",
    "Runtime error",
    "Unimplemented feature",
};

//--------------------------------------------------------------
//  AbortTranslation    A fatal error occurred during the
//                      translation.  Print the abort code
//                      to the error file and then exit.
//
//      ac : abort code
//--------------------------------------------------------------

void AbortTranslation(TAbortCode ac)
{
    cerr << "*** Fatal translator error: " << abortMsg[-ac] << endl;
    exit(ac);
}
//endfig

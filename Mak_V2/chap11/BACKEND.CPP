//fig 11-6
//  *************************************************************
//  *                                                           *
//  *   B A C K E N D                                           *
//  *                                                           *
//  *   Common backend routines.                                *
//  *                                                           *
//  *   CLASSES: TBackend                                       *
//  *                                                           *
//  *   FILE:    prog11-1/backend.cpp                           *
//  *                                                           *
//  *   MODULE:  Back end                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include "backend.h"

//--------------------------------------------------------------
//  SaveState           Save the current state of the
//                      intermediate code.
//--------------------------------------------------------------

void TBackend::SaveState(void)
{
    pSaveToken     = pToken;
    saveToken      = token;
    pSaveIcode     = pIcode;
    pSaveNode      = pNode;
    saveLocation   = CurrentLocation();
    saveLineNumber = currentLineNumber;

    pSaveTokenString = new char[strlen(pToken->String()) + 1];
    strcpy(pSaveTokenString, pToken->String());
}

//--------------------------------------------------------------
//  RestoreState        Restore the current state of the
//                      intermediate code.
//--------------------------------------------------------------

void TBackend::RestoreState(void)
{
    pToken = pSaveToken;
    token  = saveToken;
    pIcode = pSaveIcode;
    pNode  = pSaveNode;
    GoTo(saveLocation);
    currentLineNumber = saveLineNumber;

    strcpy(pToken->String(), pSaveTokenString);
    delete[] pSaveTokenString;
}
//endfig

//fig 2-5
//  *************************************************************
//  *                                                           *
//  *   E R R O R S   (Header)                                  *
//  *                                                           *
//  *   FILE:    prog2-1/error.h                                *
//  *                                                           *
//  *   MODULE:  Error                                          *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef error_h
#define error_h

extern int errorCount;

//--------------------------------------------------------------
//  Abort codes for fatal translator errors.
//--------------------------------------------------------------

enum TAbortCode {
    abortInvalidCommandLineArgs     = -1,
    abortSourceFileOpenFailed       = -2,
    abortIFormFileOpenFailed        = -3,
    abortAssemblyFileOpenFailed     = -4,
    abortTooManySyntaxErrors        = -5,
    abortStackOverflow              = -6,
    abortCodeSegmentOverflow        = -7,
    abortNestingTooDeep             = -8,
    abortRuntimeError               = -9,
    abortUnimplementedFeature       = -10,
};

void AbortTranslation(TAbortCode ac);

#endif

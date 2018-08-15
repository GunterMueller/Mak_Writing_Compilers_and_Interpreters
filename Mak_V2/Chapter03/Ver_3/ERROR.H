//fig 3-20
//  *************************************************************
//  *                                                           *
//  *   E R R O R S   (Header)                                  *
//  *                                                           *
//  *   FILE:    prog3-2/error.h                                *
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
extern int errorArrowFlag;
extern int errorArrowOffset;

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

//--------------------------------------------------------------
//  Error codes for syntax errors.
//--------------------------------------------------------------

enum TErrorCode {
    errNone,
    errUnrecognizable,
    errTooMany,
    errUnexpectedEndOfFile,
    errInvalidNumber,
    errInvalidFraction,
    errInvalidExponent,
    errTooManyDigits,
    errRealOutOfRange,
    errIntegerOutOfRange,
    errMissingRightParen,
    errInvalidExpression,
    errInvalidAssignment,
    errMissingIdentifier,
    errMissingColonEqual,
    errUndefinedIdentifier,
    errStackOverflow,
    errInvalidStatement,
    errUnexpectedToken,
    errMissingSemicolon,
    errMissingComma,
    errMissingDO,
    errMissingUNTIL,
    errMissingTHEN,
    errInvalidFORControl,
    errMissingOF,
    errInvalidConstant,
    errMissingConstant,
    errMissingColon,
    errMissingEND,
    errMissingTOorDOWNTO,
    errRedefinedIdentifier,
    errMissingEqual,
    errInvalidType,
    errNotATypeIdentifier,
    errInvalidSubrangeType,
    errNotAConstantIdentifier,
    errMissingDotDot,
    errIncompatibleTypes,
    errInvalidTarget,
    errInvalidIdentifierUsage,
    errIncompatibleAssignment,
    errMinGtMax,
    errMissingLeftBracket,
    errMissingRightBracket,
    errInvalidIndexType,
    errMissingBEGIN,
    errMissingPeriod,
    errTooManySubscripts,
    errInvalidField,
    errNestingTooDeep,
    errMissingPROGRAM,
    errAlreadyForwarded,
    errWrongNumberOfParms,
    errInvalidVarParm,
    errNotARecordVariable,
    errMissingVariable,
    errCodeSegmentOverflow,
    errUnimplementedFeature,
};

void Error(TErrorCode ec);

#endif
//endfig

//fig 3-18
//  *************************************************************
//  *                                                           *
//  *   T O K E N S   (Numbers)                                 *
//  *                                                           *
//  *   Extract number tokens from the source file.             *
//  *                                                           *
//  *   CLASSES: TNumberToken,                                  *
//  *                                                           *
//  *   FILE:    prog3-2/tknnum.cpp                             *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "token.h"

//              *******************
//              *                 *
//              *  Number Tokens  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  Get         Extract a number token from the source and set
//              its value.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TNumberToken::Get(TTextInBuffer &buffer)
{
    const int maxInteger  = 32767;
    const int maxExponent = 37;

    float  numValue      = 0.0;    // value of number ignoring
				   //    the decimal point
    int    wholePlaces   = 0;      // no. digits before the decimal point
    int    decimalPlaces = 0;      // no. digits after  the decimal point
    char   exponentSign  = '+';
    float  eValue        = 0.0;    // value of number after 'E'
    int    exponent      = 0;      // final value of exponent
    int    sawDotDotFlag = false;  // true if encountered '..',
				   //   else false

    ch             = buffer.Char();
    ps             = string;
    digitCount     = 0;
    countErrorFlag = false;
    code           = tcError;    // we don't know what it is yet, but
    type           = tyInteger;  //    assume it'll be an integer

    //--Get the whole part of the number by accumulating
    //--the values of its digits into numValue.  wholePlaces keeps
    //--track of the number of digits in this part.
    if (! AccumulateValue(buffer, numValue, errInvalidNumber)) return;
    wholePlaces = digitCount;

    //--If the current character is a dot, then either we have a
    //--fraction part or we are seeing the first character of a '..'
    //--token.  To find out, we must fetch the next character.
    if (ch == '.') {
	ch = buffer.GetChar();

	if (ch == '.') {

	    //--We have a .. token.  Back up bufferp so that the
	    //--token can be extracted next.
	    sawDotDotFlag = true;
	    buffer.PutBackChar();
	}
	else {
	    type  = tyReal;
	    *ps++ = '.';

	    //--We have a fraction part.  Accumulate it into numValue.
	    if (! AccumulateValue(buffer, numValue,
				  errInvalidFraction)) return;
	    decimalPlaces = digitCount - wholePlaces;
	}
    }

    //--Get the exponent part, if any. There cannot be an
    //--exponent part if we already saw the '..' token.
    if (!sawDotDotFlag && ((ch == 'E') || (ch == 'e'))) {
	type  = tyReal;
	*ps++ = ch;
	ch    = buffer.GetChar();

	//--Fetch the exponent's sign, if any.
	if ((ch == '+') || (ch == '-')) {
	    *ps++ = exponentSign = ch;
	    ch    = buffer.GetChar();
	}

	//--Accumulate the value of the number after 'E' into eValue.
	digitCount = 0;
	if (! AccumulateValue(buffer, eValue,
			      errInvalidExponent)) return;
	if (exponentSign == '-') eValue = -eValue;
    }

    //--Were there too many digits?
    if (countErrorFlag) {
	Error(errTooManyDigits);
	return;
    }

    //--Calculate and check the final exponent value,
    //--and then use it to adjust the number's value.
    exponent = int(eValue) - decimalPlaces;
    if ((exponent + wholePlaces < -maxExponent) ||
	(exponent + wholePlaces >  maxExponent)) {
	Error(errRealOutOfRange);
	return;
    }
    if (exponent != 0) numValue *= float(pow(10, exponent));

    //--Check and set the numeric value.
    if (type == tyInteger) {
	if ((numValue < -maxInteger) || (numValue > maxInteger)) {
	    Error(errIntegerOutOfRange);
	    return;
	}
	value.integer = int(numValue);
    }
    else value.real = numValue;

    *ps  = '\0';
    code = tcNumber;
}

//--------------------------------------------------------------
//  AccumulateValue     Extract a number part from the source
//                      and set its value.
//
//      pBuffer : ptr to text input buffer
//      value   : accumulated value (from one or more calls)
//      ec      : error code if failure
//
//  Return: true  if success
//          false if failure
//--------------------------------------------------------------

int TNumberToken::AccumulateValue(TTextInBuffer &buffer,
				  float &value, TErrorCode ec)
{
    const int maxDigitCount = 20;

    //--Error if the first character is not a digit.
    if (charCodeMap[ch] != ccDigit) {
	Error(ec);
	return false;           // failure
    }

    //--Accumulate the value as long as the total allowable
    //--number of digits has not been exceeded.
    do {
	*ps++ = ch;

	if (++digitCount <= maxDigitCount) {
	    value = 10*value + (ch - '0');  // shift left and add
	}
	else countErrorFlag = true;         // too many digits

	ch = buffer.GetChar();
    } while (charCodeMap[ch] == ccDigit);

    return true;               // success
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TNumberToken::Print(void) const
{
    if (type == tyInteger) {
	sprintf(list.text, "\t%-18s =%d", ">> integer:",
			   value.integer);
    }
    else {
	sprintf(list.text, "\t%-18s =%g", ">> real:",
			   value.real);
    }

    list.PutLine();
}
//endfig

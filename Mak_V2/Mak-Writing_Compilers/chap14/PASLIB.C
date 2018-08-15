//fig 12-5
//  *************************************************************
//  *                                                           *
//  *   P A S C A L   R U N T I M E   L I B R A R Y             *
//  *                                                           *
//  *   Note that all formal parameters are reversed to         *
//  *   accomodate the Pascal calling convention of the         *
//  *   compiled code.                                          *
//  *                                                           *
//  *   All floating point parameters are passed in as longs    *
//  *   to bypass unwanted type conversions.  Floating point    *
//  *   function values are also returned as longs.             *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_SOURCE_LINE_LENGTH  256

typedef enum {
    FALSE, TRUE
} BOOLEAN;

union {
    float real;
    long  dword;
} value;

//--------------------------------------------------------------
//  main                The main routine, which calls           
//                      _PascalMain, the "main" of the compiled
//                      program.                                
//--------------------------------------------------------------

void main(void)
{
    extern PascalMain(void);

    PascalMain();
    exit(0);
}

//              *******************
//              *                 *
//              *  Read Routines  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  ReadInteger         Read an integer value.
//--------------------------------------------------------------

int ReadInteger(void)
{
    int i;

    scanf("%d", &i);
    return feof(stdin) ? 0 : i;
}

//--------------------------------------------------------------
//  ReadReal            Read a real value.
//--------------------------------------------------------------

long ReadReal(void)
{
    scanf("%g", &value.real);
    return feof(stdin) ? 0.0 : value.dword;
}

//--------------------------------------------------------------
//  ReadChar           Read a character value.
//--------------------------------------------------------------

char ReadChar(void)
{
    char ch;

    scanf("%c", &ch);
    return (feof(stdin) || (ch == '\n')) ? ' ' : ch;
}

//--------------------------------------------------------------
//  ReadLine           Skip the rest of the input record.
//--------------------------------------------------------------

void ReadLine(void)
{
    char ch;

    do {
	ch = getchar();
    } while(!feof(stdin) && (ch != '\n'));
}

//              ********************
//              *                  *
//              *  Write Routines  *
//              *                  *
//              ********************

//--------------------------------------------------------------
//  WriteInteger        Write an integer value.
//--------------------------------------------------------------

void WriteInteger(int fieldWidth, int i)
{
    printf("%*d", fieldWidth, i);
}

//--------------------------------------------------------------
//  WriteReal           Write an real value.
//--------------------------------------------------------------

void WriteReal(int precision, int fieldWidth, long i)
{
    value.dword = i;
    printf("%*.*f", fieldWidth, precision, value.real);
}

//--------------------------------------------------------------
//  WriteBoolean        Write a boolean value.
//--------------------------------------------------------------

void WriteBoolean(int fieldWidth, int b)
{
    printf("%*s", fieldWidth, b == 0 ? "FALSE" : "TRUE");
}

//--------------------------------------------------------------
//  WriteChar           Write a character value.
//--------------------------------------------------------------

void WriteChar(int fieldWidth, int ch)
{
    printf("%*c", fieldWidth, ch);
}

//--------------------------------------------------------------
//  WriteString         Write a string value.
//--------------------------------------------------------------

void WriteString(int length, int fieldWidth, char *value)
{
    char *pCh       = value;
    int   fillCount = fieldWidth - length;

    //--Write the characters of the string.
    while (length--) putchar(*pCh++);

    //--Pad out the field on the right with blanks if necessary.
    if (fillCount > 0) while (fillCount--) putchar(' ');
}

//--------------------------------------------------------------
//  WriteLine           Write a carriage return.
//--------------------------------------------------------------

void WriteLine(void)
{
    putchar('\n');
}

//              ************************
//              *                      *
//              *  Other I/O Routines  *
//              *                      *
//              ************************

//--------------------------------------------------------------
//  StdEof              Return 1 if at end of file, else 0.
//--------------------------------------------------------------

BOOLEAN StdEof(void)
{
    char ch = getchar();

    if (feof(stdin)) return TRUE;
    else {
	ungetc(ch, stdin);
	return FALSE;
    }
}

//--------------------------------------------------------------
//  StdEoln             Return 1 if at end of line, else 0.
//--------------------------------------------------------------

BOOLEAN StdEoln(void)
{
    char ch = getchar();

    if (feof(stdin)) return TRUE;
    else {
	ungetc(ch, stdin);
	return ch == '\n';
    }
}

//              ****************************************
//              *                                      *
//              *  Floating Point Arithmetic Routines  *
//              *                                      *
//              ****************************************

//--------------------------------------------------------------
//  FloatNegate         Return the negated value.
//--------------------------------------------------------------

long FloatNegate(long i)
{
    value.dword = i;

    value.real = -value.real;
    return value.dword;
}

//--------------------------------------------------------------
//  FloatAdd            Return the sum x + y.
//--------------------------------------------------------------

long FloatAdd(long j, long i)
{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x + y;
    return value.dword;
}

//--------------------------------------------------------------
//  FloatSubtract       Return the difference x - y.
//--------------------------------------------------------------

long FloatSubtract(long j, long i)
{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x - y;
    return value.dword;
}

//--------------------------------------------------------------
//  FloatMultiply       Return the product x*y.
//--------------------------------------------------------------

long FloatMultiply(long j, long i)
{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x*y;
    return value.dword;
}

//--------------------------------------------------------------
//  FloatDivide         Return the quotient x/y.
//--------------------------------------------------------------

long FloatDivide(long j, long i)
{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x/y;
    return value.dword;
}

//--------------------------------------------------------------
//  FloatConvert        Convert an integer value to real and
//                      return the converted value.             
//--------------------------------------------------------------

long FloatConvert(int i)
{
    value.real = i;
    return value.dword;
}

//--------------------------------------------------------------
//  FloatCompare        Return -1 if x <  y
//                              0 if x == y                     
//                             +1 if x >  y                     
//--------------------------------------------------------------

int FloatCompare(long j, long i)
{
    int   comp;
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    if (x < y)          comp = -1;
    else if (x == y)    comp =  0;
    else                comp = +1;

    return comp;
}

//              ***************************************
//              *                                     *
//              *  Standard Floating Point Functions  *
//              *                                     *
//              ***************************************

//--------------------------------------------------------------
//  StdAbs              Return abs of parameter.
//--------------------------------------------------------------

long StdAbs(long i)
{
    value.dword = i;

    value.real = fabs(value.real);
    return value.dword;
}

//--------------------------------------------------------------
//  StdArctan           Return arctan of parameter.
//--------------------------------------------------------------

long StdArctan(long i)
{
    value.dword = i;

    value.real = atan(value.real);
    return value.dword;
}

//--------------------------------------------------------------
//  StdCos              Return cos of parameter.
//--------------------------------------------------------------

long StdCos(long i)
{
    value.dword = i;

    value.real = cos(value.real);
    return value.dword;
}

//--------------------------------------------------------------
//  StdExp              Return exp of parameter.
//--------------------------------------------------------------

long StdExp(long i)
{
    value.dword = i;

    value.real = exp(value.real);
    return value.dword;
}

//--------------------------------------------------------------
//  StdLn               Return ln of parameter.
//--------------------------------------------------------------

long StdLn(long i)
{
    value.dword = i;

    value.real = log(value.real);
    return value.dword;
}

//--------------------------------------------------------------
//  StdSin              Return sin of parameter.
//--------------------------------------------------------------

long StdSin(long i)
{
    value.dword = i;

    value.real = sin(value.real);
    return value.dword;
}

//--------------------------------------------------------------
//  StdSqrt             Return sqrt of parameter.
//--------------------------------------------------------------

long StdSqrt(long i)
{
    value.dword = i;

    value.real = sqrt(value.real);
    return value.dword;
}

//--------------------------------------------------------------
//  StdRound            Return round of parameter.
//--------------------------------------------------------------

int StdRound(long i)
{
    value.dword = i;

    value.dword = (int) (value.real + 0.5);
    return (int) value.dword;
}

//--------------------------------------------------------------
//  StdTrunc            Return trunc of parameter.
//--------------------------------------------------------------

int StdTrunc(long i)
{
    value.dword = i;

    value.dword = (int) value.real;
    return (int) value.dword;
}
//endfig

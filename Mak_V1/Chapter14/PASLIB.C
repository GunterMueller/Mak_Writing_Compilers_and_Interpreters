/****************************************************************/
/*                                                              */
/*      P A S C A L   R U N T I M E   L I B R A R Y             */
/*                                                              */
/*      Note that all formal parameters are reversed to         */
/*      accomodate the Pascal calling convention of the         */
/*      compiled code.                                          */
/*                                                              */
/*      All floating point parameters are passed in as longs    */
/*      to bypass unwanted type conversions.  Floating point    */
/*      function values are also returned as longs.             */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include <math.h>

#define MAX_SOURCE_LINE_LENGTH  256

typedef enum {
    FALSE, TRUE
} BOOLEAN;

union {
    float real;
    long  dword;
} value;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

BOOLEAN eof_flag  = FALSE;
BOOLEAN eoln_flag = FALSE;

/*--------------------------------------------------------------*/
/*  main                The main routine, which calls           */
/*                      pascal_main, the "main" of the compiled */
/*                      program.                                */
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    pascal_main();
    exit(0);
}

		/********************************/
		/*                              */
		/*      Read routines           */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  read_integer        Read an integer value.                  */
/*--------------------------------------------------------------*/

    int
read_integer()

{
    int i;

    scanf("%d", &i);
    return(i);
}

/*--------------------------------------------------------------*/
/*  read_real           Read a real value.                      */
/*--------------------------------------------------------------*/

    long
read_real()

{
    scanf("%g", &value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  read_char          Read a character value.                  */
/*--------------------------------------------------------------*/

    char
read_char()

{
    char ch;

    scanf("%c", &ch);
    if (eof_flag || (ch == '\n')) ch = ' ';

    return(ch);
}

/*--------------------------------------------------------------*/
/*  read_line          Skip the rest of the input record.       */
/*--------------------------------------------------------------*/

read_line()

{
    char ch;

    do {
	ch = getchar();
    } while(!eof_flag && (ch != '\n'));
}

		/********************************/
		/*                              */
		/*      Write routines          */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  write_integer       Write an integer value.                 */
/*--------------------------------------------------------------*/

write_integer(field_width, i)

    int i;
    int field_width;

{
    printf("%*d", field_width, i);
}

/*--------------------------------------------------------------*/
/*  write_real          Write an real value.                    */
/*--------------------------------------------------------------*/

write_real(precision, field_width, i)

    long i;
    int  field_width;
    int  precision;

{
    value.dword = i;
    printf("%*.*f", field_width, precision, value.real);
}

/*--------------------------------------------------------------*/
/*  write_boolean       Write a boolean value.                  */
/*--------------------------------------------------------------*/

write_boolean(field_width, b)

    int b;
    int field_width;

{
    printf("%*s", field_width, b == 0 ? "FALSE" : "TRUE");
}

/*--------------------------------------------------------------*/
/*  write_char          Write a character value.                */
/*--------------------------------------------------------------*/

write_char(field_width, ch)

    int ch;
    int field_width;

{
    printf("%*c", field_width, ch);
}

/*--------------------------------------------------------------*/
/*  write_string        Write a string value.                   */
/*--------------------------------------------------------------*/

write_string(length, field_width, value)

    char *value;
    int  field_width;
    int  length;

{
    char buffer[MAX_SOURCE_LINE_LENGTH];

    strncpy(buffer, value, length);
    buffer[length] = '\0';

    printf("%*s", -field_width, buffer);
}

/*--------------------------------------------------------------*/
/*  write_line          Write a carriage return.                */
/*--------------------------------------------------------------*/

write_line()

{
    putchar('\n');
}

		/********************************/
		/*                              */
		/*      Other I/O routines      */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  std_end_of_file     Return 1 if at end of file, else 0.     */
/*--------------------------------------------------------------*/

    BOOLEAN
std_end_of_file()

{
    char ch = getchar();

    if (eof_flag || feof(stdin)) eof_flag = TRUE;
    else                         ungetc(ch, stdin);

    return(eof_flag);
}

/*--------------------------------------------------------------*/
/*  std_end_of_line     Return 1 if at end of line, else 0.     */
/*--------------------------------------------------------------*/

    BOOLEAN
std_end_of_line()

{
    char ch = getchar();

    if (eof_flag || feof(stdin))
	eoln_flag = eof_flag = TRUE;
    else {
	eoln_flag = ch == '\n';
	ungetc(ch, stdin);
    }

    return(eoln_flag);
}

	    /************************************************/
	    /*                                              */
	    /*      Floating point arithmetic routines      */
	    /*                                              */
	    /************************************************/

/*--------------------------------------------------------------*/
/*  float_negate        Return the negated value.               */
/*--------------------------------------------------------------*/

    long
float_negate(i)

    long i;

{
    value.dword = i;

    value.real = -value.real;
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  float_add           Return the sum x + y.                   */
/*--------------------------------------------------------------*/

    long
float_add(j, i)

    long i, j;

{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x + y;
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  float_subtract      Return the difference x - y.            */
/*--------------------------------------------------------------*/

    long
float_subtract(j, i)

    long i, j;

{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x - y;
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  float_multiply      Return the product x*y.                 */
/*--------------------------------------------------------------*/

    long
float_multiply(j, i)

    long i, j;

{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x*y;
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  float_divide        Return the quotient x/y.                */
/*--------------------------------------------------------------*/

    long
float_divide(j, i)

    long i, j;

{
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    value.real = x/y;
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  float_convert       Convert an integer value to real and    */
/*                      return the converted value.             */
/*--------------------------------------------------------------*/

    long
float_convert(i)

    int i;

{
    value.real = i;
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  float_compare       Return -1 if x <  y                     */
/*                              0 if x == y                     */
/*                             +1 if x >  y                     */
/*--------------------------------------------------------------*/

float_compare(j, i)

    long i, j;

{
    int   comp;
    float x, y;

    value.dword = i;  x = value.real;
    value.dword = j;  y = value.real;

    if (x < y)          comp = -1;
    else if (x == y)    comp =  0;
    else                comp = +1;

    return(comp);
}

	    /************************************************/
	    /*                                              */
	    /*      Standard floating point functions       */
	    /*                                              */
	    /************************************************/

/*--------------------------------------------------------------*/
/*  std_abs             Return abs of parameter.                */
/*--------------------------------------------------------------*/

    long
std_abs(i)

    long i;

{
    value.dword = i;

    value.real = fabs(value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  std_arctan          Return arctan of parameter.             */
/*--------------------------------------------------------------*/

    long
std_arctan(i)

    long i;

{
    value.dword = i;

    value.real = atan(value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  std_cos             Return cos of parameter.                */
/*--------------------------------------------------------------*/

    long
std_cos(i)

    long i;

{
    value.dword = i;

    value.real = cos(value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  std_exp             Return exp of parameter.                */
/*--------------------------------------------------------------*/

    long
std_exp(i)

    long i;

{
    value.dword = i;

    value.real = exp(value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  std_ln              Return ln of parameter.                 */
/*--------------------------------------------------------------*/

    long
std_ln(i)

    long i;

{
    value.dword = i;

    value.real = log(value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  std_sin             Return sin of parameter.                */
/*--------------------------------------------------------------*/

    long
std_sin(i)

    long i;

{
    value.dword = i;

    value.real = sin(value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  std_sqrt            Return sqrt of parameter.               */
/*--------------------------------------------------------------*/

    long
std_sqrt(i)

    long i;

{
    value.dword = i;

    value.real = sqrt(value.real);
    return(value.dword);
}

/*--------------------------------------------------------------*/
/*  std_round           Return round of parameter.              */
/*--------------------------------------------------------------*/

    int
std_round(i)

    long i;

{
    value.dword = i;

    value.dword = (int) (value.real + 0.5);
    return((int) value.dword);
}

/*--------------------------------------------------------------*/
/*  std_trunc           Return trunc of parameter.              */
/*--------------------------------------------------------------*/

    int
std_trunc(i)

    long i;

{
    value.dword = i;

    value.dword = (int) value.real;
    return((int) value.dword);
}

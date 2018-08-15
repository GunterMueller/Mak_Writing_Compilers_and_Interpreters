/****************************************************************/
/*                                                              */
/*      Program 4-1:  Infix to Postfix Converter                */
/*                                                              */
/*	Convert simple Pascal expressions from infix to		*/
/*      postfix notation.                                       */
/*                                                              */
/*      FILE:       postfix.c                                   */
/*                                                              */
/*      REQUIRES:   Modules scanner, error                      */
/*								*/
/*      USAGE:      postfix sourcefile                          */
/*								*/
/*	    sourcefile	    name of source file containing	*/
/*                          expressions to convert to postfix   */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE token;
extern char       token_string[];
extern BOOLEAN    print_flag;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

char postfix[MAX_PRINT_LINE_LENGTH];    /* buffer for postfix */
char *pp;                               /* ptr into postfix */

/*--------------------------------------------------------------*/
/*  main		Contains the main loop that drives	*/
/*			the conversion by calling expression	*/
/*			each time through the loop.		*/
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    /*
    --  Initialize the scanner.
    */
    init_scanner(argv[1]);

    /*
    --	Repeatedly call expression until a period
    --	or the end of file.
    */
    do {
	strcpy(postfix, ">> ");
	pp = postfix + strlen(postfix);

	get_token();
	expression();

	output_postfix("\n");
	print_line(postfix);

	/*
	--  After an expression, there should be a semicolon,
	--  a period, or the end of file.  If not, skip tokens
	--  until there is such a token.
	*/
	while ((token != SEMICOLON) && (token != PERIOD) &&
	       (token != END_OF_FILE)) {
	    error(INVALID_EXPRESSION);
	    get_token();
	}
    } while ((token != PERIOD) && (token != END_OF_FILE));
}

/*--------------------------------------------------------------*/
/*  expression          Process an expression, which is just a	*/
/*			simple expression.			*/
/*--------------------------------------------------------------*/

expression()

{
    simple_expression();
}

/*--------------------------------------------------------------*/
/*  simple_expression	Process a simple expression consisting	*/
/*			of terms separated by + or - operators.	*/
/*--------------------------------------------------------------*/

simple_expression()

{
    TOKEN_CODE op;		/* an operator token */
    char       *op_string;	/* an operator token string */

    term();

    /*
    --	Loop to process subsequent terms
    --	separated by operators.
    */
    while ((token == PLUS) || (token == MINUS)) {
	op = token;	/* remember operator */

	get_token();
	term();		/* subsequent term */

	switch (op) {
	    case PLUS:	op_string = "+";	break;
	    case MINUS: op_string = "-";	break;
	}
	output_postfix(op_string);      /* output operator */
    }
}

/*--------------------------------------------------------------*/
/*  term                Process a term consisting of factors	*/
/*			separated by * or / operators.		*/
/*--------------------------------------------------------------*/

term()

{
    TOKEN_CODE op;		/* an operator token */
    char       *op_string;	/* an operator token string */

    factor();

    /*
    --	Loop to process subsequent factors
    --	separated by operators.
    */
    while ((token == STAR) || (token == SLASH)) {
	op = token;	/* remember operator */

	get_token();
	factor();	/* subsequent factor */

	switch (op) {
	    case STAR:	op_string = "*";	break;
	    case SLASH:	op_string = "/";	break;
	}
	output_postfix(op_string);      /* output operator */
    }
}

/*--------------------------------------------------------------*/
/*  factor		Process a factor, which is an identi-	*/
/*			fier, a number, or a parenthesized	*/
/*			subexpression.				*/
/*--------------------------------------------------------------*/

factor()

{
    if ((token == IDENTIFIER) || (token == NUMBER)) {
	output_postfix(token_string);
	get_token();
    }
    else if (token == LPAREN) {
	get_token();
	expression();	/* recursive call for subexpression */

	if (token == RPAREN) get_token();
	else                 error(MISSING_RPAREN);
    }
    else error(INVALID_EXPRESSION);
}

/*--------------------------------------------------------------*/
/*  output_postfix      Append the string preceded by a blank   */
/*                      to the postfix buffer.                  */
/*--------------------------------------------------------------*/

output_postfix(string)

    char *string;

{
    *pp++ = ' ';
    *pp   = '\0';
    strcat(pp, string);
    pp += strlen(string);
}






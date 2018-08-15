/****************************************************************/
/*                                                              */
/*      E X P R E S S I O N   P A R S E R                       */
/*                                                              */
/*      Parsing routines for expressions.                       */
/*                                                              */
/*      FILE:       expr.c                                      */
/*                                                              */
/*      MODULE:     parser                                      */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"
#include "parser.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE token;
extern char       word_string[];

/*--------------------------------------------------------------*/
/*  expression          Process an expression consisting of a	*/
/*			simple expression optionally followed	*/
/*			by a relational operator and a second	*/
/*                      simple expression.
/*--------------------------------------------------------------*/

TOKEN_CODE rel_op_list[] = {LT, LE, EQUAL, NE, GE, GT, 0};

expression()

{
    TOKEN_CODE op;              /* an operator token */

    simple_expression();        /* first simple expr */

    /*
    --	If there is a relational operator, remember it and
    --	process the second simple expression.
    */
    if (token_in(rel_op_list)) {
	op = token;             /* remember operator */

	get_token();
	simple_expression();    /* 2nd simple expr */
    }
}

/*--------------------------------------------------------------*/
/*  simple_expression	Process a simple expression consisting	*/
/*			of terms separated by +, -, or OR	*/
/*			operators.  There may be a unary + or -	*/
/*                      before the first term.                  */
/*--------------------------------------------------------------*/

TOKEN_CODE add_op_list[] = {PLUS, MINUS, OR, 0};

simple_expression()

{
    TOKEN_CODE op;                      /* an operator token */
    TOKEN_CODE unary_op = PLUS;		/* a unary operator token */

    /*
    --	If there is a unary + or -, remember it.
    */
    if ((token == PLUS) || (token == MINUS)) {
	unary_op = token;
	get_token();
    }

    term();             /* first term */

    /*
    --  Loop to process subsequent terms separated by operators.
    */
    while (token_in(add_op_list)) {
	op = token;     /* remember operator */

	get_token();
	term();         /* subsequent term */
    }
}

/*--------------------------------------------------------------*/
/*  term                Process a term consisting of factors	*/
/*			separated by *, /, DIV, MOD, or AND	*/
/*                      operators.                              */
/*--------------------------------------------------------------*/

TOKEN_CODE mult_op_list[] = {STAR, SLASH, DIV, MOD, AND, 0};

term()

{
    TOKEN_CODE op;      /* an operator token */

    factor();           /* first factor */

    /*
    --	Loop to process subsequent factors
    --	separated by operators.
    */
    while (token_in(mult_op_list)) {
	op = token;     /* remember operator */

	get_token();
	factor();       /* subsequent factor */
    }
}

/*--------------------------------------------------------------*/
/*  factor              Process a factor, which is an variable, */
/*                      a number, NOT followed by a factor, or  */
/*                      a parenthesized subexpression.          */
/*--------------------------------------------------------------*/

factor()

{
    switch (token) {

	case IDENTIFIER:
	    get_token();
	    break;

	case NUMBER:
	    get_token();
	    break;

	case STRING:
	    get_token();
	    break;

	case NOT:
	    get_token();
	    factor();
            break;

        case LPAREN:
	    get_token();
	    expression();

	    if_token_get_else_error(RPAREN, MISSING_RPAREN);
            break;

	default:
	    error(INVALID_EXPRESSION);
	    break;
    }
}

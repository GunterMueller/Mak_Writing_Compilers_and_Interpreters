/****************************************************************/
/*                                                              */
/*      P A R S I N G   R O U T I N E S   (Header)              */
/*                                                              */
/*      FILE:       parser.h                                    */
/*                                                              */
/*      MODULE:     parser                                      */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#ifndef parser_h
#define parser_h

#include "common.h"
#include "symtab.h"

		/********************************/
		/*                              */
		/*      Macros for parsing      */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  if_token_get                If token equals token_code, get */
/*                              the next token.                 */
/*--------------------------------------------------------------*/

#define if_token_get(token_code)		\
    if (token == token_code) get_token()

/*--------------------------------------------------------------*/
/*  if_token_get_else_error     If token equals token_code, get */
/*                              the next token, else error.     */
/*--------------------------------------------------------------*/

#define if_token_get_else_error(token_code, error_code)	\
    if (token == token_code) get_token(); 		\
    else                     error(error_code)

#endif


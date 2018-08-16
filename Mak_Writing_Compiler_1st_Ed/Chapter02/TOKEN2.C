/****************************************************************/
/*                                                              */
/*      Program 2-2:  Pascal Source Tokenizer                   */
/*                                                              */
/*      Recognize Pascal tokens.                                */
/*                                                              */
/*      FILE:       token2.c                                    */
/*                                                              */
/*      REQUIRES:   Modules error, scanner                      */
/*                                                              */
/*      USAGE:      token2 sourcefile                           */
/*                                                              */
/*          sourcefile      name of source file to tokenize     */
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
/*  Token name strings                                          */
/*--------------------------------------------------------------*/

char *symbol_strings[] = {
    "<no token>", "<IDENTIFIER>", "<NUMBER>", "<STRING>",
    "^", "*", "(", ")", "-", "+", "=", "[", "]", ":", ";",
    "<", ">", ",", ".", "/", ":=", "<=", ">=", "<>", "..",
    "<END OF FILE>", "<ERROR>",
    "AND", "ARRAY", "BEGIN", "CASE", "CONST", "DIV", "DO", "DOWNTO",
    "ELSE", "END", "FILE", "FOR", "FUNCTION", "GOTO", "IF", "IN",
    "LABEL", "MOD", "NIL", "NOT", "OF", "OR", "PACKED", "PROCEDURE",
    "PROGRAM", "RECORD", "REPEAT", "SET", "THEN", "TO", "TYPE",
    "UNTIL", "VAR", "WHILE", "WITH",
};

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE token;
extern char       token_string[];
extern LITERAL    literal;

/*--------------------------------------------------------------*/
/*  main                Loop to tokenize source file.           */
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
    --  Repeatedly fetch tokens until a period
    --  or the end of file.
    */
    do {
	get_token();
	if (token == END_OF_FILE) {
	    error(UNEXPECTED_END_OF_FILE);
	    break;
	}

	print_token();
    } while (token != PERIOD);

    quit_scanner();
}

/*--------------------------------------------------------------*/
/*  print_token         Print a line describing the current     */
/*                      token.                                  */
/*--------------------------------------------------------------*/

print_token()

{
    char line[MAX_SOURCE_LINE_LENGTH + 32];
    char *symbol_string = symbol_strings[token];

    switch (token) {

	case NUMBER:
	    if (literal.type == INTEGER_LIT)
		sprintf(line, "     >> %-16s %d (integer)\n",
			      symbol_string, literal.value.integer);
	    else
		sprintf(line, "     >> %-16s %g (real)\n",
			      symbol_string, literal.value.real);
            break;

	case STRING:
	    sprintf(line, "     >> %-16s '%-s'\n",
			  symbol_string, literal.value.string);
	    break;

	default:
	    sprintf(line, "     >> %-16s %-s\n",
			  symbol_string, token_string);
	    break;
    }
    print_line(line);
}

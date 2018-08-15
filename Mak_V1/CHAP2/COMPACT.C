/****************************************************************/
/*                                                              */
/*      Program 2-3:  Pascal Source Compactor                   */
/*                                                              */
/*      Compact a Pascal source file by removing                */
/*      all comments and unnecessary blanks.                    */
/*                                                              */
/*      FILE:       compact.c                                   */
/*                                                              */
/*      REQUIRES:   Modules error, scanner                      */
/*                                                              */
/*      USAGE:      compact sourcefile                          */
/*                                                              */
/*          sourcefile      name of source file to compact      */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "scanner.h"

#define MAX_OUTPUT_RECORD_LENGTH	80

/*--------------------------------------------------------------*/
/*  Token classes						*/
/*--------------------------------------------------------------*/

typedef enum {
    DELIMITER, NONDELIMITER,
} TOKEN_CLASS;

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE token;
extern char       token_string[];
extern BOOLEAN    print_flag;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

int  record_length;             /* length of output record */
char *recp;                     /* pointer into output record */

char output_record[MAX_OUTPUT_RECORD_LENGTH];

/*--------------------------------------------------------------*/
/*  main                Loop to process tokens.                 */
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    TOKEN_CLASS class;		/* current token class */
    TOKEN_CLASS prev_class;	/* previous token class */
    TOKEN_CLASS token_class();
    
    /*
    --  Initialize the scanner.
    */
    print_flag = FALSE;
    init_scanner(argv[1]);

    /*
    --  Initialize the compactor.
    */
    prev_class = DELIMITER;
    recp  = output_record;
    *recp = '\0';
    record_length = 0;

    /*
    --  Repeatedly process tokens until a period
    --  or the end of file.
    */
    do {
	get_token();
	if (token == END_OF_FILE) break;
	class = token_class();

        /*
        --  Append a blank only if two adjacent nondelimiters.
        --  Then append the token string.
        */
	if ((prev_class == NONDELIMITER) && (class == NONDELIMITER))
	    append_blank();
	append_token();

	prev_class = class;
    } while (token != PERIOD);

    /*
    --  Flush the last output record if it is partially filled.
    */
    if (record_length > 0) flush_output_record();

    quit_scanner();
}

/*--------------------------------------------------------------*/
/*  token_class		Return the class of the current token.	*/
/*--------------------------------------------------------------*/

    TOKEN_CLASS
token_class()

{
    /*
    --  Nondelimiters:	identifiers, numbers, and reserved words
    --  Delimiters:	strings and special symbols
    */
    switch (token) {

	case IDENTIFIER:
	case NUMBER:
	    return(NONDELIMITER);

	default:
	    return(token < AND ? DELIMITER : NONDELIMITER);
    }
}

/*--------------------------------------------------------------*/
/*  append_blank	Append a blank to the output record,	*/
/*			or flush the record if it is full.	*/
/*--------------------------------------------------------------*/

append_blank()

{
    if (++record_length == MAX_OUTPUT_RECORD_LENGTH - 1)
	flush_output_record();
    else strcat(output_record, " ");
}

/*--------------------------------------------------------------*/
/*  append_token	Append the token string to the output	*/
/*			record if it fits.  If not, flush the	*/
/*			current record and append the string	*/
/*			to append to the new record.		*/
/*--------------------------------------------------------------*/

append_token()

{
    int	token_length;		/* length of token string */
    
    token_length = strlen(token_string);
    if (record_length + token_length
				>= MAX_OUTPUT_RECORD_LENGTH - 1)
	flush_output_record();

    strcat(output_record, token_string);
    record_length += token_length;
}

/*--------------------------------------------------------------*/
/*  flush_output_record		Flush the current output	*/
/*				record.				*/
/*--------------------------------------------------------------*/

flush_output_record()

{
     printf("%s\n", output_record);
     recp  = output_record;
     *recp = '\0';
     record_length = 0;
}

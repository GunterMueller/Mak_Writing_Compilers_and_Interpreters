/****************************************************************/
/*                                                              */
/*      Program 2-1:  Simple Tokenizer                          */
/*                                                              */
/*      Recognize words, small integers, and the period.        */
/*                                                              */
/*      FILE:       token1.c                                    */
/*                                                              */
/*      USAGE:      token1 sourcefile                           */
/*                                                              */
/*          sourcefile      name of source file to tokenize     */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define FORM_FEED_CHAR          '\f'
#define EOF_CHAR                '\x7f'

#define MAX_FILE_NAME_LENGTH    32
#define MAX_SOURCE_LINE_LENGTH  256
#define MAX_PRINT_LINE_LENGTH   80
#define MAX_LINES_PER_PAGE	50
#define DATE_STRING_LENGTH      26
#define MAX_TOKEN_STRING_LENGTH MAX_SOURCE_LINE_LENGTH
#define MAX_CODE_BUFFER_SIZE    4096

#define MAX_INTEGER             32767
#define MAX_DIGIT_COUNT         20

typedef enum {
    FALSE, TRUE,
} BOOLEAN;

/*--------------------------------------------------------------*/
/*  Character codes                                             */
/*--------------------------------------------------------------*/

typedef enum {
    LETTER, DIGIT, SPECIAL, EOF_CODE,
} CHAR_CODE;

/*--------------------------------------------------------------*/
/*  Token codes							*/
/*--------------------------------------------------------------*/

typedef enum {
    NO_TOKEN, WORD, NUMBER, PERIOD,
    END_OF_FILE, ERROR,
} TOKEN_CODE;

/*--------------------------------------------------------------*/
/*  Token name strings                                          */
/*--------------------------------------------------------------*/

char *symbol_strings[] = {
    "<no token>", "<WORD>", "<NUMBER>", "<PERIOD>",
    "<END OF FILE>", "<ERROR>",
};

/*--------------------------------------------------------------*/
/*  Literal structure                                           */
/*--------------------------------------------------------------*/

typedef enum {
    INTEGER_LIT, STRING_LIT,
} LITERAL_TYPE;

typedef struct {
    LITERAL_TYPE type;
    union {
	int  integer;
	char string[MAX_SOURCE_LINE_LENGTH];
    } value;
} LITERAL;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

char        ch;                 /* current input character */
TOKEN_CODE  token;              /* code of current token */
LITERAL     literal;            /* value of literal */
int         buffer_offset;      /* char offset into source buffer */
int         level = 0;          /* current nesting level */
int         line_number = 0;    /* current line number */

char source_buffer[MAX_SOURCE_LINE_LENGTH]; /* source file buffer */
char token_string[MAX_TOKEN_STRING_LENGTH]; /* token string */
char *bufferp = source_buffer;              /* source buffer ptr */
char *tokenp  = token_string;               /* token string ptr */

int     digit_count;            /* total no. of digits in number */
BOOLEAN count_error;            /* too many digits in number? */

int page_number = 0;
int line_count  = MAX_LINES_PER_PAGE;   /* no. lines on current pg */

char source_name[MAX_FILE_NAME_LENGTH]; /* name of source file */
char date[DATE_STRING_LENGTH];          /* current date and time */

FILE *source_file;

CHAR_CODE char_table[256];

/*--------------------------------------------------------------*/
/*  char_code           Return the character code of ch.        */
/*--------------------------------------------------------------*/

#define char_code(ch)   char_table[ch]

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
	    print_line("*** ERROR: Unexpected end of file.\n");
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
    char line[MAX_PRINT_LINE_LENGTH];
    char *symbol_string = symbol_strings[token];

    switch (token) {

	case NUMBER:
	    sprintf(line, "     >> %-16s %d\n",
			  symbol_string, literal.value.integer);
            break;

	default:
	    sprintf(line, "     >> %-16s %-s\n",
			  symbol_string, token_string);
	    break;
    }
    print_line(line);
}

		/********************************/
		/*				*/
		/*	Initialization		*/
		/*				*/
		/********************************/

/*--------------------------------------------------------------*/
/*  init_scanner	Initialize the scanner globals		*/
/*			and open the source file.		*/
/*--------------------------------------------------------------*/

init_scanner(name)

    char *name;		/* name of source file */

{
    int ch;

    /*
    --  Initialize character table.
    */
    for (ch = 0;   ch < 256;  ++ch) char_table[ch] = SPECIAL;
    for (ch = '0'; ch <= '9'; ++ch) char_table[ch] = DIGIT;
    for (ch = 'A'; ch <= 'Z'; ++ch) char_table[ch] = LETTER;
    for (ch = 'a'; ch <= 'z'; ++ch) char_table[ch] = LETTER;
    char_table[EOF_CHAR] = EOF_CODE;

    init_page_header(name);
    open_source_file(name);
}

/*--------------------------------------------------------------*/
/*  quit_scanner	Terminate the scanner.			*/
/*--------------------------------------------------------------*/

quit_scanner()

{
    close_source_file();
}

		/********************************/
		/*				*/
		/*	Character routines	*/
		/*				*/
		/********************************/

/*--------------------------------------------------------------*/
/*  get_char		Set ch to the next character from the	*/
/*			source buffer.				*/
/*--------------------------------------------------------------*/

get_char()

{
    BOOLEAN get_source_line();

    /*
    --  If at end of current source line, read another line.
    --  If at end of file, set ch to the EOF character and return.
    */
    if (*bufferp == '\0') {
	if (! get_source_line()) {
	    ch = EOF_CHAR;
	    return;
	}
	bufferp = source_buffer;
	buffer_offset = 0;
    }

    ch = *bufferp++;    /* next character in the buffer */

    if ((ch == '\n') || (ch == '\t')) ch = ' ';
}

/*--------------------------------------------------------------*/
/*  skip_blanks		Skip past any blanks at the current	*/
/*			location in the source buffer.  Set	*/
/*			ch to the next nonblank character.	*/
/*--------------------------------------------------------------*/

skip_blanks()

{
    while (ch == ' ') get_char();
}

		/********************************/
		/*				*/
		/*	Token routines		*/
		/*				*/
		/********************************/

	/*  Note that after a token has been extracted, */
	/*  ch is the first character after the token.  */

/*--------------------------------------------------------------*/
/*  get_token		Extract the next token from the	source	*/
/*			buffer.					*/
/*--------------------------------------------------------------*/

get_token()

{
    skip_blanks();
    tokenp = token_string;

    switch (char_code(ch)) {
	case LETTER:    get_word();             break;
	case DIGIT:     get_number();           break;
	case EOF_CODE:  token = END_OF_FILE;    break;
	default:        get_special();          break;
    }
}

/*--------------------------------------------------------------*/
/*  get_word            Extract a word token and set token to   */
/*                      IDENTIFIER.                             */
/*--------------------------------------------------------------*/

get_word()

{
    BOOLEAN is_reserved_word();

    /*
    --  Extract the word.
    */
    while ((char_code(ch) == LETTER) || (char_code(ch) == DIGIT)) {
	*tokenp++ = ch;
	get_char();
    }

    *tokenp = '\0';
    token   = WORD;
}

/*--------------------------------------------------------------*/
/*  get_number		Extract a number token and set literal	*/
/*			to its value.  Set token to NUMBER.	*/
/*--------------------------------------------------------------*/

get_number()

{
    int     nvalue      = 0;      /* value of number */
    int     digit_count = 0;      /* total no. of digits in number */
    BOOLEAN count_error = FALSE;  /* too many digits in number? */

    do {
	*tokenp++ = ch;

	if (++digit_count <= MAX_DIGIT_COUNT)
	    nvalue = 10*nvalue + (ch - '0');
	else count_error = TRUE;

	get_char();
    } while (char_code(ch) == DIGIT);

    if (count_error) {
	token = ERROR;
	return;
    }

    literal.type          = INTEGER_LIT;
    literal.value.integer = nvalue;
    *tokenp = '\0';
    token   = NUMBER;
}

/*--------------------------------------------------------------*/
/*  get_special         Extract a special token.  The only      */
/*                      special token we recognize so far is    */
/*                      PERIOD.  All others are ERRORs.         */
/*--------------------------------------------------------------*/

get_special()

{
    *tokenp++ = ch;
    token = (ch == '.') ? PERIOD : ERROR;
    get_char();
    *tokenp = '\0';
}

		/********************************/
		/*                              */
		/*      Source file routines	*/
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  open_source_file	Open the source file and fetch its	*/
/*			first character.			*/
/*--------------------------------------------------------------*/

open_source_file(name)

    char *name;		/* name of source file */

{
    if ((name == NULL) ||
	((source_file = fopen(name, "r")) == NULL)) {
	printf("*** Error:  Failed to open source file.\n");
	exit(-1);
    }

    /*
    --  Fetch the first character.
    */
    bufferp = ""        ;
    get_char();
}

/*--------------------------------------------------------------*/
/*  close_source_file	Close the source file.			*/
/*--------------------------------------------------------------*/

close_source_file()

{
    fclose(source_file);
}

/*--------------------------------------------------------------*/
/*  get_source_line	Read the next line from the source	*/
/*                      file.  If there is one, print it out    */
/*                      and return TRUE.  Else return FALSE     */
/*                      for the end of file.                    */
/*--------------------------------------------------------------*/

    BOOLEAN
get_source_line()

{
    char print_buffer[MAX_SOURCE_LINE_LENGTH + 9];

    if ((fgets(source_buffer, MAX_SOURCE_LINE_LENGTH,
				    source_file)) != NULL) {
	++line_number;

	sprintf(print_buffer, "%4d %d: %s",
			      line_number, level, source_buffer);
	print_line(print_buffer);

	return(TRUE);
    }
    else return(FALSE);
}

		/********************************/
		/*                              */
		/*      Printout routines       */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  print_line          Print out a line.  Start a new page if  */
/*                      the current page is full.               */
/*--------------------------------------------------------------*/

print_line(line)

    char line[];        /* line to be printed */

{
    char save_ch;
    char *save_chp = NULL;

    if (++line_count > MAX_LINES_PER_PAGE) {
	print_page_header();
	line_count = 1;
    };

    if (strlen(line) > MAX_PRINT_LINE_LENGTH)
	save_chp = &line[MAX_PRINT_LINE_LENGTH];

    if (save_chp) {
	save_ch   = *save_chp;
	*save_chp = '\0';
    }

    printf(line);

    if (save_chp) *save_chp = save_ch;
}

/*--------------------------------------------------------------*/
/*  init_page_header    Initialize the fields of the page       */
/*                      header.                                 */
/*--------------------------------------------------------------*/

init_page_header(name)

    char *name;         /* name of source file */

{
    time_t timer;

    strncpy(source_name, name, MAX_FILE_NAME_LENGTH - 1);

    /*
    --  Set the current date and time in the date string.
    */
    time(&timer);
    strcpy(date, asctime(localtime(&timer)));
}

/*--------------------------------------------------------------*/
/*  print_page_header   Print the page header at the top of     */
/*                      the next page.                          */
/*--------------------------------------------------------------*/

print_page_header()

{
    putchar(FORM_FEED_CHAR);
    printf("Page %d   %s   %s\n\n", ++page_number, source_name, date);
}


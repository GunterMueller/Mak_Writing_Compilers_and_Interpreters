/****************************************************************/
/*                                                              */
/*      Program 1-1:  Source File Lister                        */
/*                                                              */
/*      Print the contents of a source file			*/
/*      with line numbers and page headings.                    */
/*                                                              */
/*      FILE:       list.c                                      */
/*                                                              */
/*      USAGE:      list sourcefile                             */
/*								*/
/*	    sourcefile	    name of source file to list		*/
/*                                                              */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define FORM_FEED_CHAR		'\f'

#define MAX_FILE_NAME_LENGTH    32
#define MAX_SOURCE_LINE_LENGTH  256
#define MAX_PRINT_LINE_LENGTH   80
#define MAX_LINES_PER_PAGE      50
#define DATE_STRING_LENGTH      26

typedef enum {
    FALSE, TRUE,
} BOOLEAN;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

int line_number = 0;                    /* current line number */
int page_number = 0;                    /* current page number */
int level       = 0;                    /* current nesting level */
int line_count  = MAX_LINES_PER_PAGE;   /* no. lines on current pg */

char source_buffer[MAX_SOURCE_LINE_LENGTH]; /* source file buffer */

char source_name[MAX_FILE_NAME_LENGTH]; /* name of source file */
char date[DATE_STRING_LENGTH];          /* current date and time */

FILE *source_file;

/*--------------------------------------------------------------*/
/*  main		Contains the main loop that drives	*/
/*			the lister.				*/
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    BOOLEAN get_source_line();

    init_lister(argv[1]);

    /*
    --  Repeatedly call get_source_line to read and print
    --  the next source line until the end of file.
    */
    while (get_source_line());
}

/*--------------------------------------------------------------*/
/*  init_lister         Initialize the lister globals.          */
/*--------------------------------------------------------------*/

init_lister(name)

    char *name;			/* name of source file */

{
    time_t timer;

    /*
    --  Copy the source file name and open the source file.
    */
    strcpy(source_name, name);
    source_file = fopen(source_name, "r");

    /*
    --  Set the current date and time in the date string.
    */
    time(&timer);
    strcpy(date, asctime(localtime(&timer)));
}

/*--------------------------------------------------------------*/
/*  get_source_line	Read the next line from the source	*/
/*			file.  If there was one, print it out	*/
/*			and return TRUE.  Else at end of file,	*/
/*			so return FALSE.			*/
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

/*--------------------------------------------------------------*/
/*  print_line		Print out a line.  Start a new page if  */
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

    printf("%s", line);

    if (save_chp) *save_chp = save_ch;
}

/*--------------------------------------------------------------*/
/*  print_page_header	Print the page header at the top of	*/
/*			the next page.				*/
/*--------------------------------------------------------------*/

print_page_header()

{
    putchar(FORM_FEED_CHAR);
    printf("Page %d   %s   %s\n\n", ++page_number, source_name, date);
}

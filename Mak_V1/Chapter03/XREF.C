/****************************************************************/
/*                                                              */
/*      Program 3-1:  Pascal Cross-Referencer                   */
/*                                                              */
/*      List all identifiers alphabetically each with the line  */
/*      numbers of the lines that reference it.                 */
/*                                                              */
/*      FILE:       xref.c                                      */
/*                                                              */
/*      REQUIRES:   Modules symbol table, scanner, error        */
/*                                                              */
/*      USAGE:      xref sourcefile                             */
/*								*/
/*	    sourcefile	    name of source file to cross-ref	*/
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "scanner.h"
#include "symtab.h"

#define MAX_LINENUMS_PER_LINE   10

/*--------------------------------------------------------------*/
/*  Line number item and list header                            */
/*--------------------------------------------------------------*/

typedef struct linenum_item {
    struct linenum_item *next;          /* ptr to next item */
    int                 line_number;
} LINENUM_ITEM, *LINENUM_ITEM_PTR;

typedef struct {
    LINENUM_ITEM_PTR first_linenum, last_linenum;
} LINENUM_HEADER, *LINENUM_HEADER_PTR;

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern int        line_number;
extern TOKEN_CODE token;
extern char       word_string[];

extern SYMTAB_NODE_PTR symtab_root;

/*--------------------------------------------------------------*/
/*  main                Loop to process identifiers.  Then      */
/*                      print the cross-reference listing.      */
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    SYMTAB_NODE_PTR np;         /* ptr to symtab entry */
    LINENUM_HEADER_PTR hp;      /* ptr to line item list header */

    init_scanner(argv[1]);

    /*
    --  Repeatedly process tokens until a period
    --  or the end of file.
    */
    do {
	get_token();
	if (token == END_OF_FILE) break;

	if (token == IDENTIFIER) {
	    /*
	    --  Enter each identifier into the symbol table
	    --  if it isn't already in there, and record the
	    --  current line number in the symbol table entry.
	    */
	    np = search_symtab(word_string, symtab_root);
	    if (np == NULL) {
		np = enter_symtab(word_string, &symtab_root);
		hp = alloc_struct(LINENUM_HEADER);
		hp->first_linenum = hp->last_linenum = NULL;
		np->info = (char *) hp;
	    }
	    record_line_number(np, line_number);
	}

    } while (token != PERIOD);

    /*
    --  Print out the cross-reference listing.
    */
    printf("\n\nCross-Reference");
    printf(  "\n---------------\n");
    print_xref(symtab_root);

    quit_scanner();
}

/*--------------------------------------------------------------*/
/*  record_line_number  Record a line number into the symbol    */
/*                      table entry.                            */
/*--------------------------------------------------------------*/

record_line_number(np, number)

    SYMTAB_NODE_PTR np;         /* ptr to symtab entry */
    int             number;     /* line number */

{
    LINENUM_ITEM_PTR   ip;      /* ptr to line item */
    LINENUM_HEADER_PTR hp;      /* ptr to line item list header */

    /*
    --  Create a new line number item ...
    */
    ip = alloc_struct(LINENUM_ITEM);
    ip->line_number = number;
    ip->next = NULL;

    /*
    --  ... and link it to the end of the list
    --  for this symbol table entry.
    */
    hp = (LINENUM_HEADER_PTR) np->info;
    if (hp->first_linenum == NULL)
	hp->first_linenum = hp->last_linenum = ip;
    else {
	(hp->last_linenum)->next = ip;
	hp->last_linenum = ip;
    }
}

/*--------------------------------------------------------------*/
/*  print_xref          Print the names and line numbers in     */
/*                      alphabetical order.                     */
/*--------------------------------------------------------------*/

print_xref(np)

    SYMTAB_NODE_PTR np;         /* ptr to subtree */

{
    int n;
    LINENUM_ITEM_PTR ip;        /* ptr to line item */
    LINENUM_HEADER_PTR hp;      /* ptr to line item list header */

    if (np == NULL) return;

    /*
    --  First, print the left subtree.
    */
    print_xref(np->left);

    /*
    --  Then, print the root of the subtree
    --  with at most MAX_LINENUMS_PER_LINE.
    */
    printf("\n%-16s   ", np->name);
    n = strlen(np->name) > 16 ? 0 : MAX_LINENUMS_PER_LINE;
    hp = (LINENUM_HEADER_PTR) np->info;
    for (ip = hp->first_linenum; ip != NULL; ip = ip->next) {
	if (n == 0) {
	    printf("\n%-16s   ", " ");
	    n = MAX_LINENUMS_PER_LINE;
	}
	printf(" %4d", ip->line_number);
	--n;
    };
    printf("\n");

    /*
    --  Finally, print the right subtree.
    */
    print_xref(np->right);
}

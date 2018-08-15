/****************************************************************/
/*                                                              */
/*      S Y M B O L   T A B L E                                 */
/*                                                              */
/*      Symbol table routines.                                  */
/*                                                              */
/*      FILE:       symtab.c                                    */
/*                                                              */
/*      MODULE:     symbol table                                */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"
#include "symtab.h"

/*--------------------------------------------------------------*/
/*  Globals							*/
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR symtab_root = NULL;     /* symbol table root */

/*--------------------------------------------------------------*/
/*  search_symtab       Search for a name in the symbol table.  */
/*                      Return a pointer of the entry if found, */
/*                      or NULL if not.                         */
/*--------------------------------------------------------------*/

    SYMTAB_NODE_PTR
search_symtab(name, np)

    char            *name;	/* name to search for */
    SYMTAB_NODE_PTR np;         /* ptr to symtab root */

{
    int cmp;

    /*
    --  Loop to check each node.  Return if the node matches,
    --  else continue search down the left or right subtree.
    */
    while (np != NULL) {
	cmp = strcmp(name, np->name);
	if (cmp == 0) return(np);               /* found */
	np = cmp < 0 ? np->left : np->right;    /* continue search */
    }

    return(NULL);                               /* not found */
}

/*--------------------------------------------------------------*/
/*  enter_symtab        Enter a name into the symbol table,     */
/*                      and return a pointer to the new entry.  */
/*--------------------------------------------------------------*/

    SYMTAB_NODE_PTR
enter_symtab(name, npp)

    char            *name;	/* name to enter */
    SYMTAB_NODE_PTR *npp;       /* ptr to ptr to symtab root */

{
    int             cmp;	/* result of strcmp */
    SYMTAB_NODE_PTR new_nodep;	/* ptr to new entry */
    SYMTAB_NODE_PTR np;         /* ptr to node to test */

    /*
    --  Create the new node for the name.
    */
    new_nodep = alloc_struct(SYMTAB_NODE);
    new_nodep->name = alloc_bytes(strlen(name) + 1);
    strcpy(new_nodep->name, name);
    new_nodep->left = new_nodep->right = new_nodep->next = NULL;
    new_nodep->info = NULL;
    new_nodep->defn.key = UNDEFINED;
    new_nodep->level = new_nodep->label_index = 0;

    /*
    --  Loop to search for the insertion point.
    */
    while ((np = *npp) != NULL) {
	cmp = strcmp(name, np->name);
	npp = cmp < 0 ? &(np->left) : &(np->right);
    }

    *npp = new_nodep;                   /* replace */
    return(new_nodep);
}

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

TYPE_STRUCT_PTR integer_typep, real_typep,      /* predefined types */
		boolean_typep, char_typep;

TYPE_STRUCT dummy_type = {      /* for erroneous type definitions */
    NO_FORM,      /* form */
    0,            /* size */
    NULL          /* type_idp */
};

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
    new_nodep->typep = NULL;
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

/*--------------------------------------------------------------*/
/*  init_symtab		Initialize the symbol table with 	*/
/*                      predefined identifiers and types,       */
/*                      and routines.                           */
/*--------------------------------------------------------------*/

init_symtab()

{
    SYMTAB_NODE_PTR integer_idp, real_idp, boolean_idp, char_idp,
		    false_idp, true_idp;

    enter_name_local_symtab(integer_idp, "integer");
    enter_name_local_symtab(real_idp,    "real");
    enter_name_local_symtab(boolean_idp, "boolean");
    enter_name_local_symtab(char_idp,    "char");
    enter_name_local_symtab(false_idp,   "false");
    enter_name_local_symtab(true_idp,    "true");

    integer_typep = alloc_struct(TYPE_STRUCT);
    real_typep    = alloc_struct(TYPE_STRUCT);
    boolean_typep = alloc_struct(TYPE_STRUCT);
    char_typep    = alloc_struct(TYPE_STRUCT);

    integer_idp->defn.key   = TYPE_DEFN;
    integer_idp->typep      = integer_typep;
    integer_typep->form     = SCALAR_FORM;
    integer_typep->size     = sizeof(int);
    integer_typep->type_idp = integer_idp;

    real_idp->defn.key      = TYPE_DEFN;
    real_idp->typep         = real_typep;
    real_typep->form        = SCALAR_FORM;
    real_typep->size        = sizeof(float);
    real_typep->type_idp    = real_idp;

    boolean_idp->defn.key   = TYPE_DEFN;
    boolean_idp->typep      = boolean_typep;
    boolean_typep->form     = ENUM_FORM;
    boolean_typep->size     = sizeof(int);
    boolean_typep->type_idp = boolean_idp;

    boolean_typep->info.enumeration.max = 1;
    boolean_idp->typep->info.enumeration.const_idp = false_idp;
    false_idp->defn.key = CONST_DEFN;
    false_idp->defn.info.constant.value.integer = 0;
    false_idp->typep = boolean_typep;

    false_idp->next = true_idp;
    true_idp->defn.key = CONST_DEFN;
    true_idp->defn.info.constant.value.integer = 1;
    true_idp->typep = boolean_typep;

    char_idp->defn.key   = TYPE_DEFN;
    char_idp->typep      = char_typep;
    char_typep->form     = SCALAR_FORM;
    char_typep->size     = sizeof(char);
    char_typep->type_idp = char_idp;
}

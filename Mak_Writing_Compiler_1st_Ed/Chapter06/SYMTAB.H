/****************************************************************/
/*                                                              */
/*      S Y M B O L   T A B L E   (Header)                      */
/*                                                              */
/*      FILE:       symtab.h                                    */
/*                                                              */
/*      MODULE:     symbol table                                */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#ifndef symtab_h
#define symtab_h

#include "common.h"

/*--------------------------------------------------------------*/
/*  Value structure                                             */
/*--------------------------------------------------------------*/

typedef union {
    int   integer;
    float real;
    char  character;
    char  *stringp;
} VALUE;

/*--------------------------------------------------------------*/
/*  Definition structure                                        */
/*--------------------------------------------------------------*/

typedef enum {
    UNDEFINED,
    CONST_DEFN, TYPE_DEFN, VAR_DEFN, FIELD_DEFN,
    VALPARM_DEFN, VARPARM_DEFN,
    PROG_DEFN, PROC_DEFN, FUNC_DEFN,
} DEFN_KEY;

typedef enum {
    DECLARED, FORWARD,
    READ, READLN, WRITE, WRITELN,
    ABS, ARCTAN, CHR, COS, EOFF, EOLN, EXP, LN, ODD, ORD,
    PRED, ROUND, SIN, SQR, SQRT, SUCC, TRUNC,
} ROUTINE_KEY;

typedef struct {
    DEFN_KEY key;
    union {
	struct {
	    VALUE value;
	} constant;

	struct {
	    ROUTINE_KEY        key;
	    int                parm_count;
	    int                total_parm_size;
	    int                total_local_size;
	    struct symtab_node *parms;
	    struct symtab_node *locals;
	    struct symtab_node *local_symtab;
	    char               *code_segment;
	} routine;

	struct {
	    int                offset;
	    struct symtab_node *record_idp;
	} data;
    } info;
} DEFN_STRUCT;

/*--------------------------------------------------------------*/
/*  Type structure                                              */
/*--------------------------------------------------------------*/

typedef enum {
    NO_FORM,
    SCALAR_FORM, ENUM_FORM, SUBRANGE_FORM,
    ARRAY_FORM, RECORD_FORM,
} TYPE_FORM;

typedef struct type_struct {
    TYPE_FORM          form;
    int                size;
    struct symtab_node *type_idp;
    union {
	struct {
	    struct symtab_node *const_idp;
	    int                max;
	} enumeration;

	struct {
	    struct type_struct *range_typep;
	    int                min, max;
	} subrange;

	struct {
	    struct type_struct *index_typep, *elmt_typep;
	    int                min_index, max_index;
	    int                elmt_count;
	} array;

	struct {
	    struct symtab_node *field_symtab;
	} record;
    } info;
} TYPE_STRUCT, *TYPE_STRUCT_PTR;

/*--------------------------------------------------------------*/
/*  Symbol table node						*/
/*--------------------------------------------------------------*/

typedef struct symtab_node {
    struct symtab_node *left, *right;	/* ptrs to subtrees */
    struct symtab_node *next;		/* for chaining nodes */
    char               *name;		/* name string */
    char               *info;           /* ptr to generic info */
    DEFN_STRUCT        defn;            /* definition struct */
    TYPE_STRUCT_PTR    typep;           /* ptr to type struct */
    int                level;           /* nesting level */
    int                label_index;     /* index for code label */
} SYMTAB_NODE, *SYMTAB_NODE_PTR;

/*--------------------------------------------------------------*/
/*  Functions                                                   */
/*--------------------------------------------------------------*/

SYMTAB_NODE_PTR search_symtab();
SYMTAB_NODE_PTR enter_symtab();
TYPE_STRUCT_PTR make_string_typep();

	/****************************************/
	/*                                      */
	/*      Macros to search symbol tables  */
	/*                                      */
	/****************************************/

/*--------------------------------------------------------------*/
/*  search_this_symtab              Search the given symbol     */
/*                                  table for the current id    */
/*                                  name.  Set a pointer to the */
/*                                  entry if found, else to     */
/*                                  NULL.                       */
/*--------------------------------------------------------------*/

#define search_this_symtab(idp, this_symtab)                    \
    idp = search_symtab(word_string, this_symtab)

/*--------------------------------------------------------------*/
/*  search_all_symtab               Search the local symbol     */
/*                                  table for the current id    */
/*                                  name.  Set a pointer to the */
/*                                  entry if found, else to     */
/*                                  NULL.                       */
/*--------------------------------------------------------------*/

#define search_all_symtab(idp)                                  \
    idp = search_symtab(word_string, symtab_root)

/*--------------------------------------------------------------*/
/*  enter_local_symtab              Enter the current id name   */
/*                                  into the local symbol       */
/*                                  table, and set a pointer    */
/*                                  to the entry.               */
/*--------------------------------------------------------------*/

#define enter_local_symtab(idp)                                 \
    idp = enter_symtab(word_string, &symtab_root)

/*--------------------------------------------------------------*/
/*  enter_name_local_symtab         Enter the given name into   */
/*                                  the local symbol table, and */
/*                                  set a pointer to the entry. */
/*--------------------------------------------------------------*/

#define enter_name_local_symtab(idp, name)                      \
    idp = enter_symtab(name, &symtab_root)

/*--------------------------------------------------------------*/
/*  search_and_find_all_symtab      Search the local symbol     */
/*                                  table for the current id    */
/*                                  name.  If not found, ID     */
/*                                  UNDEFINED error, and enter  */
/*                                  into the local symbol table.*/
/*                                  Set a pointer to the entry. */
/*--------------------------------------------------------------*/

#define search_and_find_all_symtab(idp)                         \
    if ((idp = search_symtab(word_string,                       \
			     symtab_root)) == NULL) {           \
	error(UNDEFINED_IDENTIFIER);                            \
	idp = enter_symtab(word_string, &symtab_root);          \
	idp->defn.key = UNDEFINED;                              \
	idp->typep = &dummy_type;                               \
    }

/*--------------------------------------------------------------*/
/*  search_and_enter_local_symtab   Search the local symbol     */
/*                                  table for the current id    */
/*                                  name.  Enter the name if    */
/*                                  it is not already in there, */
/*                                  else ID REDEFINED error.    */
/*                                  Set a pointer to the entry. */
/*--------------------------------------------------------------*/

#define search_and_enter_local_symtab(idp)                      \
    if ((idp = search_symtab(word_string,                       \
			     symtab_root)) == NULL) {           \
	idp = enter_symtab(word_string, &symtab_root);          \
    }                                                           \
    else error(REDEFINED_IDENTIFIER)

/*--------------------------------------------------------------*/
/*  search_and_enter_this_symtab    Search the given symbol     */
/*                                  table for the current id    */
/*                                  name.  Enter the name if    */
/*                                  it is not already in there, */
/*                                  else ID REDEFINED error.    */
/*                                  Set a pointer to the entry. */
/*--------------------------------------------------------------*/

#define search_and_enter_this_symtab(idp, this_symtab)          \
    if ((idp = search_symtab(word_string,                       \
			     this_symtab)) == NULL) {           \
	idp = enter_symtab(word_string, &this_symtab);          \
    }                                                           \
    else error(REDEFINED_IDENTIFIER)

#endif

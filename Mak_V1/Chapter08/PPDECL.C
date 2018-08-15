/****************************************************************/
/*                                                              */
/*      Pretty-print declarations and program, procedure, and   */
/*      function headers.                                       */
/*                                                              */
/*      FILE:       ppdecl.c                                    */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "symtab.h"
#include "pprint.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TYPE_STRUCT_PTR integer_typep, real_typep,
		       boolean_typep, char_typep;

extern char pprint_buffer[];
extern int  left_margin;
extern int  error_count;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

char string[MAX_PRINT_LINE_LENGTH];       /* buffer for literals */
BOOLEAN const_flag, type_flag, var_flag;  /* TRUE if keywords
					     already printed */

/*--------------------------------------------------------------*/
/*  analyze_routine_header  Pretty-print a program, procedure,  */
/*                          or function header:                 */
/*                                                              */
/*                              PROGRAM <id> (<parms>);         */
/*                              PROCEDURE <id> (<parms>);       */
/*                              FUNCTION <id> (<parms>) : <id>  */
/*--------------------------------------------------------------*/

analyze_routine_header(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;

{
    int             save_left_margin;   /* current left margin */
    DEFN_KEY        common_key;         /* defn of parm sublist */
    TYPE_STRUCT_PTR common_tp;          /* type of parm sublist */
    SYMTAB_NODE_PTR parm_idp = rtn_idp->defn.info.routine.parms;

    if (error_count > 0) return;
    const_flag = type_flag = var_flag = FALSE;

    emit(" ");
    flush();
    indent();

    switch (rtn_idp->defn.key) {
	case PROG_DEFN:  emit("PROGRAM ");   break;
	case PROC_DEFN:  emit("PROCEDURE "); break;
	case FUNC_DEFN:  emit("FUNCTION ");  break;
    }

    emit(rtn_idp->name);

    /*
    --  Print the formal parameters if there are any
    --  and the routine was not previously forwarded.
    */
    if ((parm_idp != NULL) &&
	(! (BOOLEAN) rtn_idp->info)) {
	BOOLEAN sublist_done;

	emit(" (");
	set_left_margin(save_left_margin);

	/*
	--  Loop to print the parameter sublists.
	*/
	do {
	    common_key = parm_idp->defn.key;
	    common_tp = parm_idp->typep;
	    if ((rtn_idp->defn.key != PROG_DEFN) &&
		(common_key == VARPARM_DEFN)) emit("VAR ");
	    emit(parm_idp->name);

	    /*
	    --  Loop to print the parameters in a sublist.
	    */
	    do {
	       parm_idp = parm_idp->next;
	       sublist_done = ((parm_idp == NULL) ||
			       (parm_idp->defn.key != common_key) ||
			       (parm_idp->typep != common_tp));
	       if (!sublist_done) {
		   emit(", ");
		   emit(parm_idp->name);
	       }
	    } while (!sublist_done);

	    if (rtn_idp->defn.key != PROG_DEFN) {
		emit(" : ");
		print_type(common_tp, FALSE);
	    }

	    if (parm_idp != NULL) {
		emit(";");
		flush();
		indent();
	    }
	} while (parm_idp != NULL);

	emit(")");

	if (rtn_idp->defn.key == FUNC_DEFN) {
	    emit(" : ");
	    print_type(rtn_idp->typep, FALSE);
	}

	reset_left_margin(save_left_margin);
    }

    emit(";");
    flush();

    /*
    --  Print a forward declaration.
    */
    if (rtn_idp->defn.info.routine.key == FORWARD) {
	rtn_idp->info = (char *) TRUE;

	advance_left_margin();
	indent();
	emit("FORWARD;");
	flush();
	retreat_left_margin();
    }

    else advance_left_margin();
}

/*--------------------------------------------------------------*/
/*  declaration_keyword     Print the keyword CONST, TYPE, or   */
/*                          VAR in string if flag is FALSE.     */
/*                          Each keyword is printed at most     */
/*                          once per block.                     */
/*--------------------------------------------------------------*/

declaration_keyword(flag, string)

    BOOLEAN *flag;      /* TRUE if keyword already printed */
    char    *string;    /* keyword */

{
    if (! *flag) {
	emit(" ");
	flush();

	indent();
	emit(string);
	flush();

	*flag = TRUE;
    }
}

/*--------------------------------------------------------------*/
/*  analyze_const_defn      Pretty-print a constant definition: */
/*                                                              */
/*                              CONST                           */
/*                                  <id> = <literal>;           */
/*--------------------------------------------------------------*/

analyze_const_defn(const_idp)

    SYMTAB_NODE_PTR const_idp;                    /* constant id */

{
    TYPE_STRUCT_PTR const_tp = const_idp->typep;  /* constant type */

    if (error_count > 0) return;

    declaration_keyword(&const_flag, "CONST");
    advance_left_margin();

    indent();
    emit(const_idp->name);
    emit(" = ");

    if (const_tp->form == ARRAY_FORM) {             /* string */
	emit("'");
	emit(const_idp->defn.info.constant.value.stringp);
	emit("'");
    }
    else if (const_tp == integer_typep) {
	sprintf(string, "%d",
		const_idp->defn.info.constant.value.integer);
	emit(string);
    }
    else if (const_tp == real_typep) {
	sprintf(string, "%g",
		const_idp->defn.info.constant.value.real);
	emit(string);
    }
    else if (const_tp == char_typep) {
	sprintf(string, "'%c'",
		const_idp->defn.info.constant.value.character);
	emit(string);
    }

    emit(";");
    flush();
    retreat_left_margin();
}

/*--------------------------------------------------------------*/
/*  analyze_type_defn       Pretty-print a type definition:     */
/*                                                              */
/*                              TYPE                            */
/*                                  <id> = <type>;              */
/*--------------------------------------------------------------*/

analyze_type_defn(type_idp)

    SYMTAB_NODE_PTR type_idp;           /* type id */

{
    if (error_count > 0) return;

    declaration_keyword(&type_flag, "TYPE");
    advance_left_margin();

    indent();
    emit(type_idp->name);
    emit(" = ");

    print_type(type_idp->typep,
	       type_idp == type_idp->typep->type_idp);
    emit(";");
    flush();
    retreat_left_margin();
}

/*--------------------------------------------------------------*/
/*  print_type              Pretty-print a type.                */
/*--------------------------------------------------------------*/

print_type(tp, defn_flag)

    TYPE_STRUCT_PTR tp;             /* type */
    BOOLEAN         defn_flag;      /* TRUE if named definition */

{
    /*
    --  Identifier type.
    */
    if (!defn_flag && (tp->type_idp != NULL))
	emit(tp->type_idp->name);

    /*
    --  Other type.
    */
    else switch (tp->form) {
	case ENUM_FORM:         print_enum_type(tp);        break;
	case SUBRANGE_FORM:     print_subrange_type(tp);    break;
	case ARRAY_FORM:        print_array_type(tp);       break;
	case RECORD_FORM:       print_record_type(tp);      break;
    }
}

/*--------------------------------------------------------------*/
/*  print_enum_type         Pretty-print an enumeration type:   */
/*                                                              */
/*                              (<id1>, <id2>, ..., <idn>)      */
/*--------------------------------------------------------------*/

print_enum_type(tp)

    TYPE_STRUCT_PTR tp;         /* type */

{
    SYMTAB_NODE_PTR idp = tp->info.enumeration.const_idp;

    emit("(");

    /*
    --  Loop to print the enumeration constant id list.
    */
    while (idp != NULL) {
	emit(idp->name);
	idp = idp->next;
	if (idp != NULL) emit(", ");
    }

    emit(")");
}

/*--------------------------------------------------------------*/
/*  print_subrange_type     Pretty-print a subrange type:       */
/*                                                              */
/*                              (<const1>..<const2>)            */
/*--------------------------------------------------------------*/

print_subrange_type(tp)

    TYPE_STRUCT_PTR tp;         /* type */

{
    print_subrange_limit(tp->info.subrange.min,
			 tp->info.subrange.range_typep);
    emit("..");
    print_subrange_limit(tp->info.subrange.max,
			 tp->info.subrange.range_typep);
}

/*--------------------------------------------------------------*/
/*  print_subrange_limit    Pretty-print a subrange constant    */
/*                          limit.                              */
/*--------------------------------------------------------------*/

print_subrange_limit(limit, range_tp)

    int             limit;
    TYPE_STRUCT_PTR range_tp;

{
    if (range_tp == integer_typep) {
	sprintf(string, "%d", limit);
	emit(string);
    }
    else if (range_tp == char_typep) {
	sprintf(string, "'%c'", limit);
	emit(string);
    }
    else if (range_tp->form == ENUM_FORM) {
	SYMTAB_NODE_PTR idp = range_tp->info.enumeration.const_idp;

	while (limit-- > 0) idp = idp->next;
	emit(idp->name);
    }
}

/*--------------------------------------------------------------*/
/*  print_array_type        Pretty-print an array type:         */
/*                                                              */
/*                              ARRAY [<type>] OF               */
/*                                  ARRAY [<type>] OF <type>    */
/*--------------------------------------------------------------*/

print_array_type(tp)

    TYPE_STRUCT_PTR tp;                 /* type */

{
    int             save_left_margin;   /* current left margin */
    TYPE_STRUCT_PTR index_tp = tp->info.array.index_typep;
    TYPE_STRUCT_PTR elmt_tp  = tp->info.array.elmt_typep;

    set_left_margin(save_left_margin);

    emit("ARRAY [");
    print_type(index_tp, FALSE);
    emit("] OF ");

    if (elmt_tp->type_idp != NULL) print_type(elmt_tp, FALSE);
    else {
	flush();

	/*
	--  Cascade multidimensional array definitions.
	*/
	advance_left_margin();
	indent();
	print_type(elmt_tp, FALSE);
    }

    reset_left_margin(save_left_margin);
}

/*--------------------------------------------------------------*/
/*  print_record_type       Pretty-print a record type:         */
/*                                                              */
/*                              RECORD                          */
/*                                  <field-list1> : <type>;     */
/*                                  <field-list2> : <type>      */
/*                              END                             */
/*--------------------------------------------------------------*/

print_record_type(tp)

    TYPE_STRUCT_PTR tp;

{
    int             save_left_margin;      /* current left margin */
    BOOLEAN         sublist_done;          /* TRUE iff done */
    SYMTAB_NODE_PTR field_idp = tp->info.record.field_symtab;
    TYPE_STRUCT_PTR common_tp;             /* type of field sublist */

    set_left_margin(save_left_margin);

    emit("RECORD");
    flush();
    advance_left_margin();

    /*
    --  Loop to print field sublists.
    */
    while (field_idp != NULL) {
	indent();
	emit(field_idp->name);
	common_tp = field_idp->typep;

	/*
	--  Loop to print fields of a sublist.
	*/
	do {
	    field_idp = field_idp->next;
	    sublist_done = ((field_idp == NULL) ||
			    (field_idp->typep != common_tp));
	    if (!sublist_done) {
		emit(", ");
		emit(field_idp->name);
	    }
	} while (!sublist_done);

	emit(" : ");
	print_type(common_tp, FALSE);
	emit(";");
	flush();
    }

    retreat_left_margin();
    indent();
    emit("END");
    reset_left_margin(save_left_margin);
}

/*--------------------------------------------------------------*/
/*  analyze_var_decl    Pretty-print a variable declaration:    */
/*                                                              */
/*                          VAR                                 */
/*                              <id> : <type>;                  */
/*--------------------------------------------------------------*/

analyze_var_decl(var_idp)

    SYMTAB_NODE_PTR var_idp;            /* variable id */

{
    TYPE_STRUCT_PTR common_tp;          /* type of id sublist */
    BOOLEAN         sublist_done;       /* TRUE iff sublist done */
    static SYMTAB_NODE_PTR done_var_idp = NULL;  /* id already
						    printed */

    if (error_count > 0) return;

    /*
    --  If this variable is part of a sublist that has
    --  already been printed, don't print it again.
    */
    if (var_idp == done_var_idp) {
	done_var_idp = var_idp->next;
	return;
    }
    else done_var_idp = var_idp->next;

    declaration_keyword(&var_flag, "VAR");
    advance_left_margin();

    indent();
    emit(var_idp->name);
    common_tp = var_idp->typep;

    /*
    --  Loop to print the variables in a sublist.
    */
    do {
	var_idp = var_idp->next;
	sublist_done = (var_idp == NULL);
	if (!sublist_done) {
	    emit(", ");
	    emit(var_idp->name);
	}
    } while (!sublist_done);

    emit(" : ");
    print_type(common_tp, FALSE);
    emit(";");
    flush();

    retreat_left_margin();
}

/****************************************************************/
/*                                                              */
/*      Program 9-1:  Pascal Interpreter I                      */
/*                                                              */
/*      Interpret assignement statements in procedures          */
/*      and functions.                                          */
/*                                                              */
/*      FILE:       run1.c                                      */
/*                                                              */
/*      REQUIRES:   Modules parser, symbol table, scanner,      */
/*                          executor, error                     */
/*                                                              */
/*      FLAGS:      Macro flag "trace" must be defined          */
/*								*/
/*      USAGE:      run1 sourcefile                             */
/*                                                              */
/*          sourcefile      name of source file containing      */
/*                          the statements to interpret         */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "symtab.h"
#include "exec.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern int exec_line_number;

extern TYPE_STRUCT_PTR integer_typep, real_typep,
		       boolean_typep, char_typep;

/*--------------------------------------------------------------*/
/*  main                Initialize the scanner and call         */
/*                      routine program.                        */
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
    --  Process a program.
    */
    get_token();
    program();
}

/*--------------------------------------------------------------*/
/*  trace_routine_entry         Trace the entry into a routine. */
/*--------------------------------------------------------------*/

trace_routine_entry(idp)

    SYMTAB_NODE_PTR idp;        /* routine id */

{
    printf(">> Entering routine %s\n", idp->name);
}

/*--------------------------------------------------------------*/
/*  trace_routine_exit          Trace the exit from a routine.  */
/*--------------------------------------------------------------*/

trace_routine_exit(idp)

    SYMTAB_NODE_PTR idp;        /* routine id */

{
    printf(">> Exiting routine %s\n", idp->name);
}

/*--------------------------------------------------------------*/
/*  trace_statement_execution   Trace the execution of a        */
/*                              statement.                      */
/*--------------------------------------------------------------*/

trace_statement_execution()

{
    printf(">>  Stmt %d\n", exec_line_number);
}

/*--------------------------------------------------------------*/
/*  trace_data_store            Trace the storing of data into  */
/*                              a variable.                     */
/*--------------------------------------------------------------*/

trace_data_store(idp, idp_tp, targetp, target_tp)

    SYMTAB_NODE_PTR idp;            /* id of target variable */
    TYPE_STRUCT_PTR idp_tp;         /* ptr to id's type */
    STACK_ITEM_PTR  targetp;        /* ptr to target location */
    TYPE_STRUCT_PTR target_tp;      /* ptr to target's type */

{
    printf(">>   %s", idp->name);
    if      (idp_tp->form == ARRAY_FORM)  printf("[*]");
    else if (idp_tp->form == RECORD_FORM) printf(".*");
    print_data_value(targetp, target_tp, ":=");
}

/*--------------------------------------------------------------*/
/*  trace_data_fetch            Trace the fetching of data from */
/*                              a variable.                     */
/*--------------------------------------------------------------*/

trace_data_fetch(idp, tp, datap)

    SYMTAB_NODE_PTR idp;            /* id of target variable */
    TYPE_STRUCT_PTR tp;             /* ptr to id's type */
    STACK_ITEM_PTR  datap;          /* ptr to data */

{
    printf(">>   %s", idp->name);
    if      (tp->form == ARRAY_FORM)  printf("[*]");
    else if (tp->form == RECORD_FORM) printf(".*");
    print_data_value(datap, tp, "=");
}

/*--------------------------------------------------------------*/
/*  print_data_value            Print a data value.             */
/*--------------------------------------------------------------*/

print_data_value(datap, tp, str)

    STACK_ITEM_PTR  datap;      /* ptr to data value to print */
    TYPE_STRUCT_PTR tp;         /* ptr to type of stack item */
    char            *str;       /* " = " or " := " */

{
    /*
    --  Reduce a subrange type to its range type.
    --  Convert a non-boolean enumeration type to integer.
    */
    if (tp->form == SUBRANGE_FORM)
	tp = tp->info.subrange.range_typep;
    if ((tp->form == ENUM_FORM) && (tp != boolean_typep))
	tp = integer_typep;

    if (tp == integer_typep)
	printf(" %s %d\n", str, datap->integer);
    else if (tp == real_typep)
	printf(" %s %0.6g\n", str, datap->real);
    else if (tp == boolean_typep)
	printf(" %s %s\n", str, datap->integer == 1
				    ? "true" : "false");
    else if (tp == char_typep)
	printf(" %s '%c'\n", str, datap->byte);

    else if (tp->form == ARRAY_FORM) {
	if (tp->info.array.elmt_typep == char_typep) {
	    char *chp = (char *) datap;
	    int  size = tp->info.array.elmt_count;

	    printf(" %s '", str);
	    while (size--) printf("%c", *chp++);
	    printf("'\n");
	}
	else printf(" %s <array>\n", str);
    }
    else if (tp->form == RECORD_FORM)
	printf(" %s <record>\n", str);
}


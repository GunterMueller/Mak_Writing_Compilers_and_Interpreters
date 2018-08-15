/****************************************************************/
/*                                                              */
/*      Program 6-1:  Declarations Analyzer			*/
/*                                                              */
/*	Analyze Pascal constant definitions, type definitions,	*/
/*	and variable declarations.				*/
/*								*/
/*      FILE:       analyze.c                                   */
/*								*/
/*      REQUIRES:   Modules symbol table, scanner, error        */
/*                                                              */
/*                  File decl.c                                 */
/*                                                              */
/*      FLAGS:      Macro flag "analyze" must be defined        */
/*								*/
/*      USAGE:      analyze sourcefile                          */
/*								*/
/*	    sourcefile	    name of source file containing	*/
/*			    declarations to be analyzed		*/
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"
#include "symtab.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TYPE_STRUCT_PTR integer_typep, real_typep,
		       boolean_typep, char_typep;

extern TOKEN_CODE token;
extern int        line_number, error_count;

extern TYPE_STRUCT dummy_type;

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

char buffer[MAX_PRINT_LINE_LENGTH];

char *defn_names[] = {"undefined",
		      "constant", "type", "variable",
		      "field", "procedure", "function"};

char *form_names[] = {"no form",
		      "scalar", "enum", "subrange",
		      "array", "record"};

/*--------------------------------------------------------------*/
/*  main		Initialize the scanner and the symbol	*/
/*			table, and then call the declarations	*/
/*			routine.				*/
/*--------------------------------------------------------------*/

main(argc, argv)

    int  argc;
    char *argv[];

{
    SYMTAB_NODE_PTR program_idp;        /* artificial program id */

    /*
    --  Initialize the scanner and the symbol table.
    */
    init_scanner(argv[1]);
    init_symtab();

    /*
    --  Create an artifical program id node.
    */
    program_idp = alloc_struct(SYMTAB_NODE);
    program_idp->defn.key = PROG_DEFN;
    program_idp->defn.info.routine.key = DECLARED;
    program_idp->defn.info.routine.parm_count = 0;
    program_idp->defn.info.routine.total_parm_size = 0;
    program_idp->defn.info.routine.total_local_size = 0;
    program_idp->typep = &dummy_type;
    program_idp->label_index = 0;

    /*
    --  Parse declarations.
    */
    get_token();
    declarations(program_idp);

    /*
    --  Look for the end of file.
    */
    while (token != END_OF_FILE) {
	error(UNEXPECTED_TOKEN);
	get_token();
    }

    quit_scanner();

    /*
    --  Print summary.
    */
    print_line("\n");
    print_line("\n");
    sprintf(buffer, "%20d Source lines.\n", line_number);
    print_line(buffer);
    sprintf(buffer, "%20d Source errors.\n", error_count);
    print_line(buffer);

    exit(0);
}

		/************************/
		/*			*/
		/*	Analysis	*/
		/*			*/
		/************************/

/*--------------------------------------------------------------*/
/*  analyze_const_defn      Analyze a constant definition.      */
/*--------------------------------------------------------------*/

analyze_const_defn(idp)

    SYMTAB_NODE_PTR idp;        /* constant id */

{
    char *bp;

    /*
    --  The constant's name ...
    */
    sprintf(buffer, ">> id = %s\n", idp->name);
    print_line(buffer);

    /*
    --  ... definition and value ...
    */
    sprintf(buffer, ">>    defn = %s, value = ",
		    defn_names[idp->defn.key]);
    bp = buffer + strlen(buffer);

    if ((idp->typep == integer_typep) ||
	(idp->typep->form == ENUM_FORM))
	sprintf(bp, "%d\n",
		idp->defn.info.constant.value.integer);
    else if (idp->typep == real_typep)
	sprintf(bp, "%g\n",
		idp->defn.info.constant.value.real);
    else if (idp->typep == char_typep)
	sprintf(bp, "'%c'\n",
		idp->defn.info.constant.value.character);
    else if (idp->typep->form == ARRAY_FORM)
	sprintf(bp, "'%s'\n",
		idp->defn.info.constant.value.stringp);

    print_line(buffer);

    /*
    --  ... and type.  (Don't try to re-analyze an
    --  enumeration type, or an infinite loop will occur.)
    */
    if (idp->typep->form != ENUM_FORM)
	analyze_type(idp->typep, FALSE);
}

/*--------------------------------------------------------------*/
/*  analyze_type_defn       Analyze a type definition.          */
/*--------------------------------------------------------------*/

analyze_type_defn(idp)

    SYMTAB_NODE_PTR idp;        /* type id */

{
    char *bp;

    /*
    --  The type's name, definition ...
    */
    sprintf(buffer, ">> id = %s\n", idp->name);
    print_line(buffer);

    sprintf(buffer, ">>    defn = %s\n",
		    defn_names[idp->defn.key]);
    print_line(buffer);

    /*
    --  ... and type.
    */
    analyze_type(idp->typep, TRUE);
}

/*--------------------------------------------------------------*/
/*  analyze_type            Analyze a type by calling the       */
/*                          appropriate type analysis routine.  */
/*--------------------------------------------------------------*/

analyze_type(tp, verbose_flag)

    TYPE_STRUCT_PTR tp;         /* ptr to type structure */
    BOOLEAN verbose_flag;       /* TRUE for verbose analysis */

{
    char *bp;

    if (tp == NULL) return;

    /*
    --  The form, byte size, and, if named, its type id.
    */
    sprintf(buffer, ">>    form = %s, size = %d bytes, type id = ",
		    form_names[tp->form], tp->size);
    bp = buffer + strlen(buffer);

    if (tp->type_idp != NULL)
	sprintf(bp, "%s\n", tp->type_idp->name);
    else {
	sprintf(bp, "<unnamed type>\n");
	verbose_flag = TRUE;
    }
    print_line(buffer);

    /*
    --  Call the appropriate type analysis routine.
    */
    switch (tp->form) {
	case ENUM_FORM:
	    analyze_enum_type(tp, verbose_flag);
	    break;

	case SUBRANGE_FORM:
	    analyze_subrange_type(tp, verbose_flag);
	    break;

	case ARRAY_FORM:
	    analyze_array_type(tp, verbose_flag);
	    break;

	case RECORD_FORM:
	    analyze_record_type(tp, verbose_flag);
	    break;
    }
}

/*--------------------------------------------------------------*/
/*  analyze_enum_type       Analyze an enumeration type.        */
/*--------------------------------------------------------------*/

analyze_enum_type(tp, verbose_flag)

    TYPE_STRUCT_PTR tp;         /* ptr to type structure */
    BOOLEAN verbose_flag;       /* TRUE for verbose analysis */

{
    SYMTAB_NODE_PTR idp;

    if (!verbose_flag) return;

    /*
    --  Loop to analyze each enumeration constant
    --  as a constant definition.
    */
    print_line(">>    --- Enum Constants ---\n");
    for (idp = tp->info.enumeration.const_idp;
	 idp != NULL;
	 idp = idp->next) analyze_const_defn(idp);
}

/*--------------------------------------------------------------*/
/*  analyze_subrange_type           Analyze a subrange type.    */
/*--------------------------------------------------------------*/

analyze_subrange_type(tp, verbose_flag)

    TYPE_STRUCT_PTR tp;         /* ptr to type structure */
    BOOLEAN verbose_flag;       /* TRUE for verbose analysis */

{
    if (!verbose_flag) return;

    sprintf(buffer, ">>    min value = %d, max value = %d\n",
		    tp->info.subrange.min,
		    tp->info.subrange.max);
    print_line(buffer);

    print_line(">>    --- Range Type ---\n");
    analyze_type(tp->info.subrange.range_typep, FALSE);
}

/*--------------------------------------------------------------*/
/*  analyze_array_type      Analyze an array type.              */
/*--------------------------------------------------------------*/

analyze_array_type(tp, verbose_flag)

    TYPE_STRUCT_PTR tp;         /* ptr to type structure */
    BOOLEAN verbose_flag;       /* TRUE for verbose analysis */

{
    if (!verbose_flag) return;

    sprintf(buffer, ">>    element count = %d\n",
		    tp->info.array.elmt_count);
    print_line(buffer);

    print_line(">>    --- INDEX TYPE ---\n");
    analyze_type(tp->info.array.index_typep, FALSE);

    print_line(">>    --- ELEMENT TYPE ---\n");
    analyze_type(tp->info.array.elmt_typep, FALSE);
}

/*--------------------------------------------------------------*/
/*  analyze_record_type     Analyze a record type.              */
/*--------------------------------------------------------------*/

analyze_record_type(tp, verbose_flag)

    TYPE_STRUCT_PTR tp;         /* ptr to type structure */
    BOOLEAN verbose_flag;       /* TRUE for verbose analysis */

{
    SYMTAB_NODE_PTR idp;

    if (!verbose_flag) return;

    /*
    --  Loop to analyze each record field
    --  as a variable declaration.
    */
    print_line(">>    --- Fields ---\n");
    for (idp = tp->info.record.field_symtab;
	 idp != NULL;
	 idp = idp->next) analyze_var_decl(idp);
}

/*--------------------------------------------------------------*/
/*  analyze_var_decl        Analyze a variable declaration.     */
/*--------------------------------------------------------------*/

analyze_var_decl(idp)

    SYMTAB_NODE_PTR idp;        /* variable id */

{
    sprintf(buffer, ">> id = %s\n", idp->name);
    print_line(buffer);

    sprintf(buffer, ">>    defn = %s, offset = %d\n",
		    defn_names[idp->defn.key],
		    idp->defn.info.data.offset);
    print_line(buffer);

    analyze_type(idp->typep, FALSE);
}

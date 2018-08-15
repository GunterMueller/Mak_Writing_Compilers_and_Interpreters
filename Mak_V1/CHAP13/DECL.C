/****************************************************************/
/*                                                              */
/*      D E C L A R A T I O N   P A R S E R                     */
/*								*/
/*      Parsing routines for delarations.                       */
/*                                                              */
/*      FILE:       decl.c                                      */
/*                                                              */
/*      MODULE:     parser                                      */
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
#include "parser.h"
#include "code.h"

/*--------------------------------------------------------------*/
/*  Externals                                                   */
/*--------------------------------------------------------------*/

extern TOKEN_CODE       token;
extern char             word_string[];
extern LITERAL          literal;

extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

extern int              label_index;

extern TOKEN_CODE       declaration_start_list[],
			statement_start_list[];

/*--------------------------------------------------------------*/
/*  Forwards							*/
/*--------------------------------------------------------------*/

TYPE_STRUCT_PTR do_type(),
		identifier_type(), enumeration_type(),
		subrange_type(), array_type(), record_type();

/*--------------------------------------------------------------*/
/*  declarations        Call the routines to process constant   */
/*                      definitions, type definitions, variable */
/*                      declarations, procedure definitions,    */
/*                      and function definitions.               */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_routine_list[] = {SEMICOLON, END_OF_FILE, 0};

declarations(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;    /* id of program or routine */

{
    if (token == CONST) {
	get_token();
	const_definitions();
    }

    if (token == TYPE) {
	get_token();
	type_definitions();
    }

    if (token == VAR) {
	get_token();
	var_declarations(rtn_idp);
    }

    /*
    --  Emit declarations for parameters and local variables.
    */
    if (rtn_idp->defn.key != PROG_DEFN) emit_declarations(rtn_idp);

    /*
    --  Loop to process routine (procedure and function)
    --  definitions.
    */
    while ((token == PROCEDURE) || (token == FUNCTION)) {
	routine();

	/*
	--  Error synchronization:  Should be ;
	*/
	synchronize(follow_routine_list,
		    declaration_start_list, statement_start_list);
	if_token_get(SEMICOLON);
	else if (token_in(declaration_start_list) ||
		 token_in(statement_start_list))
	    error(MISSING_SEMICOLON);
    }
}

		/************************/
		/*                      */
		/*      Constants       */
		/*                      */
		/************************/

/*--------------------------------------------------------------*/
/*  const_definitions   Process constant definitions:           */
/*                                                              */
/*                          <id> = <constant>                   */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_declaration_list[] = {SEMICOLON, IDENTIFIER,
					END_OF_FILE, 0};

const_definitions()

{
    SYMTAB_NODE_PTR const_idp;          /* constant id */

    /*
    --  Loop to process definitions separated by semicolons.
    */
    while (token == IDENTIFIER) {
	search_and_enter_local_symtab(const_idp);
	const_idp->defn.key = CONST_DEFN;

	get_token();
	if_token_get_else_error(EQUAL, MISSING_EQUAL);

	/*
	--  Process the constant.
	*/
	do_const(const_idp);
	analyze_const_defn(const_idp);

	/*
	--  Error synchronization:  Should be ;
	*/
	synchronize(follow_declaration_list,
		    declaration_start_list, statement_start_list);
	if_token_get(SEMICOLON);
	else if (token_in(declaration_start_list) ||
		 token_in(statement_start_list))
	    error(MISSING_SEMICOLON);
    }
}

/*--------------------------------------------------------------*/
/*  do_const            Process the constant of a constant      */
/*                      definition.                             */
/*--------------------------------------------------------------*/

do_const(const_idp)

    SYMTAB_NODE_PTR const_idp;          /* constant id */

{
    TOKEN_CODE      sign     = PLUS;    /* unary + or - sign */
    BOOLEAN         saw_sign = FALSE;   /* TRUE iff unary sign */

    /*
    --  Unary + or - sign.
    */
    if ((token == PLUS) || (token == MINUS)) {
	sign     = token;
	saw_sign = TRUE;
	get_token();
    }

    /*
    --  Numeric constant:  Integer or real type.
    */
    if (token == NUMBER) {
	if (literal.type == INTEGER_LIT) {
	    const_idp->defn.info.constant.value.integer =
		sign == PLUS ?  literal.value.integer
			     : -literal.value.integer;
	    const_idp->typep = integer_typep;
	}
	else {
	    const_idp->defn.info.constant.value.real =
		sign == PLUS ?  literal.value.real
			     : -literal.value.real;
	    const_idp->typep = real_typep;
	}
    }

    /*
    --  Identifier constant:  Integer, real, character, enumeration,
    --                        or string (character array) type.
    */
    else if (token == IDENTIFIER) {
	SYMTAB_NODE_PTR idp;

	search_all_symtab(idp);

	if (idp == NULL)
	    error(UNDEFINED_IDENTIFIER);
	else if (idp->defn.key != CONST_DEFN)
	    error(NOT_A_CONSTANT_IDENTIFIER);

	else if (idp->typep == integer_typep) {
	    const_idp->defn.info.constant.value.integer =
		sign == PLUS ?  idp->defn.info.constant.value.integer
			     : -idp->defn.info.constant.value.integer;
	    const_idp->typep = integer_typep;
	}
	else if (idp->typep == real_typep) {
	    const_idp->defn.info.constant.value.real =
		sign == PLUS ?  idp->defn.info.constant.value.real
			     : -idp->defn.info.constant.value.real;
	    const_idp->typep = real_typep;
	}
	else if (idp->typep == char_typep) {
	    if (saw_sign) error(INVALID_CONSTANT);

	    const_idp->defn.info.constant.value.character =
			      idp->defn.info.constant.value.character;
	    const_idp->typep = char_typep;
	}
	else if (idp->typep->form == ENUM_FORM) {
	    if (saw_sign) error(INVALID_CONSTANT);

	    const_idp->defn.info.constant.value.integer =
				idp->defn.info.constant.value.integer;
	    const_idp->typep = idp->typep;
	}
	else if (idp->typep->form == ARRAY_FORM) {
	    if (saw_sign) error(INVALID_CONSTANT);

	    const_idp->defn.info.constant.value.stringp =
				idp->defn.info.constant.value.stringp;
	    const_idp->typep = idp->typep;
	}
    }

    /*
    --  String constant:  Character or string (character array) type.
    */
    else if (token == STRING) {
	if (saw_sign) error(INVALID_CONSTANT);

	if (strlen(literal.value.string) == 1) {
	    const_idp->defn.info.constant.value.character =
					      literal.value.string[0];
	    const_idp->typep = char_typep;
	}
	else {
	    int length = strlen(literal.value.string);

	    const_idp->defn.info.constant.value.stringp =
					      alloc_bytes(length + 1);
	    strcpy(const_idp->defn.info.constant.value.stringp,
		   literal.value.string);
	    const_idp->typep = make_string_typep(length);
	}
    }

    else {
	const_idp->typep = &dummy_type;
	error(INVALID_CONSTANT);
    }

    get_token();
}

		/************************/
		/*                      */
		/*      Types           */
		/*                      */
		/************************/

/*--------------------------------------------------------------*/
/*  type_definitions    Process type definitions:               */
/*                                                              */
/*                          <id> = <type>                       */
/*--------------------------------------------------------------*/

type_definitions()

{
    SYMTAB_NODE_PTR type_idp;           /* type id */

    /*
    --  Loop to process definitions separated by semicolons.
    */
    while (token == IDENTIFIER) {
	search_and_enter_local_symtab(type_idp);
	type_idp->defn.key = TYPE_DEFN;

	get_token();
	if_token_get_else_error(EQUAL, MISSING_EQUAL);

	/*
	--  Process the type specification.
	*/
	type_idp->typep = do_type();
	if (type_idp->typep->type_idp == NULL)
	    type_idp->typep->type_idp = type_idp;

	analyze_type_defn(type_idp);

	/*
	--  Error synchronization:  Should be ;
	*/
	synchronize(follow_declaration_list,
		    declaration_start_list, statement_start_list);
	if_token_get(SEMICOLON);
	else if (token_in(declaration_start_list) ||
		 token_in(statement_start_list))
	    error(MISSING_SEMICOLON);
    }
}

/*--------------------------------------------------------------*/
/*  do_type             Process a type specification.  Call the */
/*                      functions that make a type structure    */
/*                      and return a pointer to it.             */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
do_type()

{
    switch (token) {
	case IDENTIFIER: {
	    SYMTAB_NODE_PTR idp;

	    search_all_symtab(idp);

	    if (idp == NULL) {
		error(UNDEFINED_IDENTIFIER);
		return(&dummy_type);
	    }
	    else if (idp->defn.key == TYPE_DEFN)
		return(identifier_type(idp));
	    else if (idp->defn.key == CONST_DEFN)
		return(subrange_type(idp));
	    else {
		error(NOT_A_TYPE_IDENTIFIER);
		return(&dummy_type);
	    }
	}

	case LPAREN:    return(enumeration_type());
	case ARRAY:     return(array_type());
	case RECORD:    return(record_type());

	case PLUS:
	case MINUS:
	case NUMBER:
	case STRING:    return(subrange_type(NULL));

	default:        error(INVALID_TYPE);
			return(&dummy_type);
    }
}

/*--------------------------------------------------------------*/
/*  identifier_type     Process an identifier type, i.e., the   */
/*                      identifier on the right side of a type  */
/*                      equate, and return a pointer to its     */
/*                      type structure.                         */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
identifier_type(idp)

    SYMTAB_NODE_PTR idp;        /* type id */

{
    TYPE_STRUCT_PTR tp = NULL;

    tp = idp->typep;
    get_token();

    return(tp);
}

/*--------------------------------------------------------------*/
/*  enumeration_type    Process an enumeration type:            */
/*                                                              */
/*                          ( <id1>, <id2>, ..., <idn> )        */
/*                                                              */
/*                      Make a type structure and return a      */
/*                      pointer to it.                          */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
enumeration_type()

{
    SYMTAB_NODE_PTR const_idp;          /* constant id */
    SYMTAB_NODE_PTR last_idp    = NULL; /* last constant id */
    TYPE_STRUCT_PTR tp          = alloc_struct(TYPE_STRUCT);
    int             const_value = -1;   /* constant value */

    tp->form     = ENUM_FORM;
    tp->size     = sizeof(int);
    tp->type_idp = NULL;

    get_token();

    /*
    --  Loop to process list of identifiers.
    */
    while (token == IDENTIFIER) {
	search_and_enter_local_symtab(const_idp);
	const_idp->defn.key = CONST_DEFN;
	const_idp->defn.info.constant.value.integer = ++const_value;
	const_idp->typep = tp;

	/*
	--  Link constant ids together.
	*/
	if (last_idp == NULL)
	    tp->info.enumeration.const_idp = last_idp = const_idp;
	else {
	    last_idp->next = const_idp;
	    last_idp = const_idp;
	}

	get_token();
	if_token_get(COMMA);
    }

    if_token_get_else_error(RPAREN, MISSING_RPAREN);

    tp->info.enumeration.max = const_value;
    return(tp);
}

/*--------------------------------------------------------------*/
/*  subrange_type       Process a subrange type:                */
/*                                                              */
/*                          <min-const> .. <max-const>          */
/*                                                              */
/*                      Make a type structure and return a      */
/*                      pointer to it.                          */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_min_limit_list[] = {DOTDOT, IDENTIFIER, PLUS, MINUS,
				      NUMBER, STRING, SEMICOLON,
				      END_OF_FILE, 0};

    TYPE_STRUCT_PTR
subrange_type(min_idp)

    SYMTAB_NODE_PTR min_idp;    /* min limit const id */

{
    TYPE_STRUCT_PTR max_typep;  /* type of max limit */
    TYPE_STRUCT_PTR tp = alloc_struct(TYPE_STRUCT);

    tp->form      = SUBRANGE_FORM;
    tp->type_idp  = NULL;

    /*
    --  Minimum constant.
    */
    get_subrange_limit(min_idp,
		       &(tp->info.subrange.min),
		       &(tp->info.subrange.range_typep));

    /*
    --  Error synchronization:  Should be ..
    */
    synchronize(follow_min_limit_list, NULL, NULL);
    if_token_get(DOTDOT);
    else if (token_in(follow_min_limit_list) ||
	     token_in(declaration_start_list) ||
	     token_in(statement_start_list))
	error(MISSING_DOTDOT);

    /*
    --  Maximum constant.
    */
    get_subrange_limit(NULL, &(tp->info.subrange.max), &max_typep);

    /*
    --  Check limits.
    */
    if (max_typep == tp->info.subrange.range_typep) {
	if (tp->info.subrange.min > tp->info.subrange.max)
	    error(MIN_GT_MAX);
    }
    else error(INCOMPATIBLE_TYPES);

    tp->size = max_typep == char_typep ? sizeof(char) : sizeof(int);
    return(tp);
}

/*--------------------------------------------------------------*/
/*  get_subrange_limit  Process the minimum and maximum limits  */
/*                      of a subrange type.                     */
/*--------------------------------------------------------------*/

get_subrange_limit(minmax_idp, minmaxp, typepp)

    SYMTAB_NODE_PTR minmax_idp; /* min const id */
    int             *minmaxp;   /* where to store min or max value */
    TYPE_STRUCT_PTR *typepp;    /* where to store ptr to type struct */

{
    SYMTAB_NODE_PTR idp      = minmax_idp;
    TOKEN_CODE      sign     = PLUS;    /* unary + or - sign */
    BOOLEAN         saw_sign = FALSE;   /* TRUE iff unary sign */

    /*
    --  Unary + or - sign.
    */
    if ((token == PLUS) || (token == MINUS)) {
	sign     = token;
	saw_sign = TRUE;
	get_token();
    }

    /*
    --  Numeric limit:  Integer type only.
    */
    if (token == NUMBER) {
	if (literal.type == INTEGER_LIT) {
	    *typepp  = integer_typep;
	    *minmaxp = (sign == PLUS) ?  literal.value.integer
				      : -literal.value.integer;
	}
	else error(INVALID_SUBRANGE_TYPE);
    }

    /*
    --  Identifier limit:  Value must be integer or character.
    */
    else if (token == IDENTIFIER) {
	if (idp == NULL) search_all_symtab(idp);

	if (idp == NULL)
	    error(UNDEFINED_IDENTIFIER);
	else if (idp->typep == real_typep)
	    error(INVALID_SUBRANGE_TYPE);
	else if (idp->defn.key == CONST_DEFN) {
	    *typepp  = idp->typep;
	    if (idp->typep == char_typep) {
		if (saw_sign) error(INVALID_CONSTANT);
		*minmaxp = idp->defn.info.constant.value.character;
	    }
	    else if (idp->typep == integer_typep) {
		*minmaxp = idp->defn.info.constant.value.integer;
		if (sign == MINUS) *minmaxp = -(*minmaxp);
	    }
	    else /* enumeration constant */ {
		if (saw_sign) error(INVALID_CONSTANT);
		*minmaxp = idp->defn.info.constant.value.integer;
	    }
	}
	else error(NOT_A_CONSTANT_IDENTIFIER);
    }

    /*
    --  String limit:  Character type only.
    */
    else if (token == STRING) {
	if (saw_sign) error(INVALID_CONSTANT);
	*typepp  = char_typep;
	*minmaxp = literal.value.string[0];

	if (strlen(literal.value.string) != 1)
	    error(INVALID_SUBRANGE_TYPE);
    }

    else error(MISSING_CONSTANT);

    get_token();
}

/*--------------------------------------------------------------*/
/*  array_type          Process an array type:                  */
/*                                                              */
/*                          ARRAY [<index-type-list>]           */
/*                              OF <elmt-type>                  */
/*                                                              */
/*                      Make a type structure and return a      */
/*                      pointer to it.                          */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_dimension_list[] = {COMMA, RBRACKET, OF,
				      SEMICOLON, END_OF_FILE, 0};

TOKEN_CODE index_type_start_list[] = {IDENTIFIER, NUMBER, STRING,
				      LPAREN, MINUS, PLUS, 0};

TOKEN_CODE follow_indexes_list[]   = {OF, IDENTIFIER, LPAREN, ARRAY,
				      RECORD, PLUS, MINUS, NUMBER,
				      STRING, SEMICOLON, END_OF_FILE,
				      0};

    TYPE_STRUCT_PTR
array_type()

{
    TYPE_STRUCT_PTR tp      = alloc_struct(TYPE_STRUCT);
    TYPE_STRUCT_PTR index_tp;           /* index type */
    TYPE_STRUCT_PTR elmt_tp = tp;       /* element type */
    int array_size();

    get_token();
    if (token != LBRACKET) error(MISSING_LBRACKET);

    /*
    --  Loop to process index type list.  For each
    --  type in the list after the first, create an
    --  array element type.
    */
    do {
	get_token();

	if (token_in(index_type_start_list)) {
	    elmt_tp->form     = ARRAY_FORM;
	    elmt_tp->size     = 0;
	    elmt_tp->type_idp = NULL;
	    elmt_tp->info.array.index_typep = index_tp = do_type();

	    switch (index_tp->form) {
		case ENUM_FORM:
		    elmt_tp->info.array.elmt_count =
				index_tp->info.enumeration.max + 1;
		    elmt_tp->info.array.min_index = 0;
		    elmt_tp->info.array.max_index =
				index_tp->info.enumeration.max;
		    break;

		case SUBRANGE_FORM:
		    elmt_tp->info.array.elmt_count =
				index_tp->info.subrange.max -
				    index_tp->info.subrange.min + 1;
		    elmt_tp->info.array.min_index =
					index_tp->info.subrange.min;
		    elmt_tp->info.array.max_index =
					index_tp->info.subrange.max;
		    break;

		default:
		    elmt_tp->form     = NO_FORM;
		    elmt_tp->size     = 0;
		    elmt_tp->type_idp = NULL;
		    elmt_tp->info.array.index_typep = &dummy_type;
		    error(INVALID_INDEX_TYPE);
		    break;
	    }
	}
	else {
	    elmt_tp->form     = NO_FORM;
	    elmt_tp->size     = 0;
	    elmt_tp->type_idp = NULL;
	    elmt_tp->info.array.index_typep = &dummy_type;
	    error(INVALID_INDEX_TYPE);
	}

	/*
	--  Error synchronization:  Should be , or ]
	*/
	synchronize(follow_dimension_list, NULL, NULL);

	/*
	--  Create an array element type.
	*/
	if (token == COMMA) elmt_tp = elmt_tp->info.array.elmt_typep =
					  alloc_struct(TYPE_STRUCT);
    } while (token == COMMA);

    if_token_get_else_error(RBRACKET, MISSING_RBRACKET);

    /*
    --  Error synchronization:  Should be OF
    */
    synchronize(follow_indexes_list,
		declaration_start_list, statement_start_list);
    if_token_get_else_error(OF, MISSING_OF);

    /*
    --  Element type.
    */
    elmt_tp->info.array.elmt_typep = do_type();

    tp->size = array_size(tp);
    return(tp);
}

/*--------------------------------------------------------------*/
/*  record_type         Process a record type:                  */
/*                                                              */
/*                          RECORD                              */
/*                              <id-list> : <type> ;            */
/*                                  ...                         */
/*                          END                                 */
/*                                                              */
/*                      Make a type structure and return a      */
/*                      pointer to it.                          */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
record_type()

{
    TYPE_STRUCT_PTR record_tp = alloc_struct(TYPE_STRUCT);

    record_tp->form     = RECORD_FORM;
    record_tp->type_idp = NULL;
    record_tp->info.record.field_symtab = NULL;

    get_token();
    var_or_field_declarations(NULL, record_tp, 0);

    if_token_get_else_error(END, MISSING_END);
    return(record_tp);
}

/*--------------------------------------------------------------*/
/*  make_string_typep   Make a type structure for a string of   */
/*                      the given length, and return a pointer  */
/*                      to it.                                  */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
make_string_typep(length)

    int length;                 /* string length */

{
    TYPE_STRUCT_PTR string_tp = alloc_struct(TYPE_STRUCT);
    TYPE_STRUCT_PTR index_tp  = alloc_struct(TYPE_STRUCT);

    /*
    --  Array type.
    */
    string_tp->form     = ARRAY_FORM;
    string_tp->size     = length;
    string_tp->type_idp = NULL;
    string_tp->info.array.index_typep = index_tp;
    string_tp->info.array.elmt_typep  = char_typep;
    string_tp->info.array.elmt_count  = length;

    /*
    --  Subrange index type.
    */
    index_tp->form     = SUBRANGE_FORM;
    index_tp->size     = sizeof(int);
    index_tp->type_idp = NULL;
    index_tp->info.subrange.range_typep = integer_typep;
    index_tp->info.subrange.min = 1;
    index_tp->info.subrange.max = length;

    return(string_tp);
}

/*--------------------------------------------------------------*/
/*  array_size          Return the size in bytes of an array    */
/*                      type by recursively calculating the     */
/*                      size of each dimension.                 */
/*--------------------------------------------------------------*/

    int
array_size(tp)

    TYPE_STRUCT_PTR tp;         /* ptr to array type structure */

{
    if (tp->info.array.elmt_typep->size == 0)
	tp->info.array.elmt_typep->size =
				array_size(tp->info.array.elmt_typep);

    tp->size = tp->info.array.elmt_count *
		   tp->info.array.elmt_typep->size;

    return(tp->size);
}

		/************************/
		/*                      */
		/*      Variables       */
		/*                      */
		/************************/

/*--------------------------------------------------------------*/
/*  var_declarations    Process variable declarations:          */
/*                                                              */
/*                          <id-list> : <type>                  */
/*--------------------------------------------------------------*/

var_declarations(rtn_idp)

    SYMTAB_NODE_PTR rtn_idp;    /* id of program or routine */

{
    var_or_field_declarations(rtn_idp, NULL,
			      rtn_idp->defn.key == PROC_DEFN
				  ? PROC_LOCALS_STACK_FRAME_OFFSET
				  : FUNC_LOCALS_STACK_FRAME_OFFSET);
}

/*--------------------------------------------------------------*/
/*  var_or_field_declarations   Process variable declarations   */
/*                              or record field definitions.    */
/*                              All ids declared with the same  */
/*                              type are linked together into   */
/*                              a sublist, and all the sublists */
/*                              are then linked together.       */
/*--------------------------------------------------------------*/

TOKEN_CODE follow_variables_list[] = {SEMICOLON, IDENTIFIER,
				      END_OF_FILE, 0};

TOKEN_CODE follow_fields_list[]    = {SEMICOLON, END, IDENTIFIER,
				      END_OF_FILE, 0};

var_or_field_declarations(rtn_idp, record_tp, offset)

    SYMTAB_NODE_PTR rtn_idp;
    TYPE_STRUCT_PTR record_tp;
    int             offset;

{
    SYMTAB_NODE_PTR idp, first_idp, last_idp;   /* variable or
						     field ids */
    SYMTAB_NODE_PTR prev_last_idp = NULL;       /* last id of list */
    TYPE_STRUCT_PTR tp;                         /* type */
    BOOLEAN var_flag = (rtn_idp != NULL);       /* TRUE:  variables */
						/* FALSE: fields */
    int size;
    int total_size = 0;

    /*
    --  Loop to process sublist, each of a type.
    */
    while (token == IDENTIFIER) {
	first_idp = NULL;

	/*
	--  Loop process each variable or field id in a sublist.
	*/
	while (token == IDENTIFIER) {
	    if (var_flag) {
		search_and_enter_local_symtab(idp);
		idp->defn.key = VAR_DEFN;
	    }
	    else {
		search_and_enter_this_symtab
		    (idp, record_tp->info.record.field_symtab);
		idp->defn.key = FIELD_DEFN;
	    }
	    idp->label_index = new_label_index();

	    /*
	    --  Link ids together into a sublist.
	    */
	    if (first_idp == NULL) {
		first_idp = last_idp = idp;
		if (var_flag &&
		    (rtn_idp->defn.info.routine.locals == NULL))
		    rtn_idp->defn.info.routine.locals = idp;
	    }
	    else {
		last_idp->next = idp;
		last_idp = idp;
	    }

	    get_token();
	    if_token_get(COMMA);
	}

	/*
	--  Process the sublist's type.
	*/
	if_token_get_else_error(COLON, MISSING_COLON);
	tp = do_type();
	size = tp->size;
	if (size & 1) ++size;   /* round up to even */

	/*
	--  Assign the offset and the type to all variable or field
	--  ids in the sublist.
	*/
	for (idp = first_idp; idp != NULL; idp = idp->next) {
	    idp->typep = tp;

	    if (var_flag) {
		offset -= size;
		total_size += size;
		idp->defn.info.data.offset = offset;
		analyze_var_decl(idp);
	    }

	    else   /* record fields */  {
		idp->defn.info.data.offset = offset;
		offset += size;

		/*
		--  Emit numeric equate for the field id's
		--  name and offset.
		*/
		emit_numeric_equate(idp);
	    }
	}

	/*
	--  Link this sublist to the previous sublist.
	*/
	if (prev_last_idp != NULL) prev_last_idp->next = first_idp;
	prev_last_idp = last_idp;

	/*
	--  Error synchronization:  Should be ; for variable
	--                          declaration, or ; or END for
	--                          record type definition.
	*/
	synchronize(var_flag ? follow_variables_list
			     : follow_fields_list,
		    declaration_start_list, statement_start_list);
	if_token_get(SEMICOLON);
	else if (var_flag && ((token_in(declaration_start_list)) ||
			      (token_in(statement_start_list))))
	    error(MISSING_SEMICOLON);
    }

    if (var_flag)
	rtn_idp->defn.info.routine.total_local_size = total_size;
    else
	record_tp->size = offset;
}

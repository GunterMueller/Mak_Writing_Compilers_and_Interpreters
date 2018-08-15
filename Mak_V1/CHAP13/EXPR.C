/****************************************************************/
/*                                                              */
/*      E X P R E S S I O N   P A R S E R                       */
/*                                                              */
/*      Parsing routines for expressions.                       */
/*                                                              */
/*      FILE:       expr.c                                      */
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

extern TOKEN_CODE token;
extern char       token_string[];
extern char       word_string[];
extern LITERAL    literal;

extern SYMTAB_NODE_PTR  symtab_display[];
extern int              level;

extern TYPE_STRUCT_PTR  integer_typep, real_typep,
			boolean_typep, char_typep;

extern TYPE_STRUCT      dummy_type;

extern SYMTAB_NODE_PTR  float_literal_list;
extern SYMTAB_NODE_PTR  string_literal_list;

extern int              label_index;
extern char             asm_buffer[];
extern char             *asm_bufferp;
extern FILE             *code_file;

/*--------------------------------------------------------------*/
/*  Forwards                                                    */
/*--------------------------------------------------------------*/

TYPE_STRUCT_PTR expression(), simple_expression(), term(), factor(),
		constant_identifier(), function_call();

TYPE_STRUCT_PTR float_literal(), string_literal();

/*--------------------------------------------------------------*/
/*  integer_operands    TRUE if both operands are integer,      */
/*                      else FALSE.                             */
/*--------------------------------------------------------------*/

#define integer_operands(tp1, tp2)  ((tp1 == integer_typep) &&  \
				     (tp2 == integer_typep))

/*--------------------------------------------------------------*/
/*  real_operands       TRUE if at least one or both operands   */
/*                      operands are real (and the other        */
/*                      integer), else FALSE.                   */
/*--------------------------------------------------------------*/

#define real_operands(tp1, tp2) (((tp1 == real_typep) &&        \
				    ((tp2 == real_typep) ||     \
				     (tp2 == integer_typep)))   \
					||                      \
				 ((tp2 == real_typep) &&        \
				    ((tp1 == real_typep) ||     \
				     (tp1 == integer_typep))))

/*--------------------------------------------------------------*/
/*  boolean_operands    TRUE if both operands are boolean       */
/*                      else FALSE.                             */
/*--------------------------------------------------------------*/

#define boolean_operands(tp1, pt2)  ((tp1 == boolean_typep) &&  \
				     (tp2 == boolean_typep))

/*--------------------------------------------------------------*/
/*  expression          Process an expression consisting of a	*/
/*			simple expression optionally followed	*/
/*			by a relational operator and a second	*/
/*                      simple expression.  Return a pointer to */
/*                      the type structure.                     */
/*--------------------------------------------------------------*/

TOKEN_CODE rel_op_list[] = {LT, LE, EQUAL, NE, GE, GT, 0};

    TYPE_STRUCT_PTR
expression()

{
    TOKEN_CODE      op;                 /* an operator token */
    TYPE_STRUCT_PTR result_tp, tp2;
    int             jump_label_index;   /* jump target label index */
    INSTRUCTION     jump_opcode;        /* opcode for cond. jump */

    result_tp = simple_expression();    /* first simple expr */

    /*
    --	If there is a relational operator, remember it and
    --	process the second simple expression.
    */
    if (token_in(rel_op_list)) {
	op = token;                     /* remember operator */

	result_tp = base_type(result_tp);
	emit_push_operand(result_tp);

	get_token();
	tp2 = base_type(simple_expression());   /* 2nd simple expr */

	check_rel_op_types(result_tp, tp2);

	/*
	--  Both operands are integer, character, boolean, or
	--  the same enumeration type.  Compare DX (operand 1)
	--  to AX (operand 2).
	*/
	if (integer_operands(result_tp, tp2) ||
	    (result_tp == char_typep) ||
	    (result_tp->form == ENUM_FORM)) {
	    emit_1(POP, reg(DX));
	    emit_2(COMPARE, reg(DX), reg(AX));
	}

	/*
	--  Both operands are real, or one is real and the other
	--  is integer.  Convert the integer operand to real.
	--  Call FLOAT_COMPARE to do the comparison, which returns
	--  -1 (less), 0 (equal), or +1 (greater).
	*/
	else if ((result_tp == real_typep) || (tp2 == real_typep)) {
	    emit_push_operand(tp2);
	    emit_promote_to_real(result_tp, tp2);

	    emit_1(CALL, name_lit(FLOAT_COMPARE));
	    emit_2(ADD, reg(SP), integer_lit(8));
	    emit_2(COMPARE, reg(AX), integer_lit(0));
	}

	/*
	--  Both operands are strings.  Compare the string pointed
	--  to by SI (operand 1) to the string pointed to by DI
	--  (operand 2).
	*/
	else if (result_tp->form == ARRAY_FORM) {
	    emit_1(POP,  reg(DI));
	    emit_1(POP,  reg(SI));
	    emit_2(MOVE, reg(AX), reg(DS));
	    emit_2(MOVE, reg(ES), reg(AX));
	    emit(CLEAR_DIRECTION);
	    emit_2(MOVE, reg(CX),
		   integer_lit(result_tp->info.array.elmt_count));
	    emit(COMPARE_STRINGS);
	}

	emit_2(MOVE, reg(AX), integer_lit(1));  /* default: load 1 */

	switch (op) {
	    case LT:    jump_opcode = JUMP_LT;  break;
	    case LE:    jump_opcode = JUMP_LE;  break;
	    case EQUAL: jump_opcode = JUMP_EQ;  break;
	    case NE:    jump_opcode = JUMP_NE;  break;
	    case GE:    jump_opcode = JUMP_GE;  break;
	    case GT:    jump_opcode = JUMP_GT;  break;
	}

	jump_label_index = new_label_index();
	emit_1(jump_opcode, label(STMT_LABEL_PREFIX,
	       jump_label_index));

	emit_2(SUBTRACT, reg(AX), reg(AX));     /* load 0 if false */
	emit_label(STMT_LABEL_PREFIX, jump_label_index);

	result_tp = boolean_typep;
    }

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  simple_expression	Process a simple expression consisting	*/
/*			of terms separated by +, -, or OR	*/
/*			operators.  There may be a unary + or -	*/
/*                      before the first term.  Return a        */
/*                      pointer to the type structure.          */
/*--------------------------------------------------------------*/

TOKEN_CODE add_op_list[] = {PLUS, MINUS, OR, 0};

    TYPE_STRUCT_PTR
simple_expression()

{
    TOKEN_CODE op;                      /* an operator token */
    BOOLEAN    saw_unary_op = FALSE;    /* TRUE iff unary operator */
    TOKEN_CODE unary_op = PLUS;		/* a unary operator token */
    TYPE_STRUCT_PTR result_tp, tp2;

    /*
    --	If there is a unary + or -, remember it.
    */
    if ((token == PLUS) || (token == MINUS)) {
	unary_op = token;
	saw_unary_op = TRUE;
	get_token();
    }

    result_tp = term();         /* first term */

    /*
    --  If there was a unary operator, check that the term
    --  is integer or real.  Negate the top of stack if it
    --  was a unary - either with the NEG instruction or by
    --  calling FLOAT_NEGATE.
    */
    if (saw_unary_op) {
	if (base_type(result_tp) == integer_typep) {
	    if (unary_op == MINUS) emit_1(NEGATE, reg(AX));
	}
	else if (result_tp == real_typep) {
	    if (unary_op == MINUS) {
		emit_push_operand(result_tp);
		emit_1(CALL, name_lit(FLOAT_NEGATE));
		emit_2(ADD, reg(SP), integer_lit(4));
	    }
	}
	else error(INCOMPATIBLE_TYPES);
    }

    /*
    --  Loop to process subsequent terms separated by operators.
    */
    while (token_in(add_op_list)) {
	op = token;                     /* remember operator */

	result_tp = base_type(result_tp);
	emit_push_operand(result_tp);

	get_token();
	tp2 = base_type(term());        /* subsequent term */

	switch (op) {

	    case PLUS:
	    case MINUS: {
		/*
		--  integer <op> integer => integer
		--  AX = AX +|- DX
		*/
		if (integer_operands(result_tp, tp2)) {
		    emit_1(POP, reg(DX));
		    if (op == PLUS) emit_2(ADD, reg(AX), reg(DX))
		    else {
			emit_2(SUBTRACT, reg(DX), reg(AX));
			emit_2(MOVE, reg(AX), reg(DX));
		    }
		    result_tp = integer_typep;
		}

		/*
		--  Both operands are real, or one is real and the
		--  other is integer.  Convert the integer operand
		--  to real.  The result is real.  Call FLOAT_ADD or
		--  FLOAT_SUBTRACT.
		*/
		else if (real_operands(result_tp, tp2)) {
		    emit_push_operand(tp2);
		    emit_promote_to_real(result_tp, tp2);

		    emit_1(CALL, name_lit(op == PLUS
					      ? FLOAT_ADD
					      : FLOAT_SUBTRACT));
		    emit_2(ADD, reg(SP), integer_lit(8));

		    result_tp = real_typep;
		}

		else {
		    error(INCOMPATIBLE_TYPES);
		    result_tp = &dummy_type;
		}

		break;
	    }

	    case OR: {
		/*
		--  boolean OR boolean => boolean
		--  AX = AX OR DX
		*/
		if (boolean_operands(result_tp, tp2)) {
		    emit_1(POP, reg(DX));
		    emit_2(OR_BITS, reg(AX), reg(DX));
		}
		else error(INCOMPATIBLE_TYPES);

		result_tp = boolean_typep;
		break;
	    }
	}
    }

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  term                Process a term consisting of factors	*/
/*			separated by *, /, DIV, MOD, or AND	*/
/*                      operators.  Return a pointer to the     */
/*                      type structure.                         */
/*--------------------------------------------------------------*/

TOKEN_CODE mult_op_list[] = {STAR, SLASH, DIV, MOD, AND, 0};

    TYPE_STRUCT_PTR
term()

{
    TOKEN_CODE op;                      /* an operator token */
    TYPE_STRUCT_PTR result_tp, tp2;

    result_tp = factor();               /* first factor */

    /*
    --	Loop to process subsequent factors
    --	separated by operators.
    */
    while (token_in(mult_op_list)) {
	op = token;                     /* remember operator */

	result_tp = base_type(result_tp);
	emit_push_operand(result_tp);

	get_token();
	tp2 = base_type(factor());      /* subsequent factor */

	switch (op) {

	    case STAR: {
		/*
		--  Both operands are integer.
		--  AX = AX*DX
		*/
		if (integer_operands(result_tp, tp2)) {
		    emit_1(POP, reg(DX));
		    emit_1(MULTIPLY, reg(DX));

		    result_tp = integer_typep;
		}

		/*
		--  Both operands are real, or one is real and the
		--  other is integer.  Convert the integer operand
		--  to real.  The result is real.
		--  Call FLOAT_MULTIPLY.
		*/
		else if (real_operands(result_tp, tp2)) {
		    emit_push_operand(tp2);
		    emit_promote_to_real(result_tp, tp2);

		    emit_1(CALL, name_lit(FLOAT_MULTIPLY));
		    emit_2(ADD, reg(SP), integer_lit(8));

		    result_tp = real_typep;
		}

		else {
		    error(INCOMPATIBLE_TYPES);
		    result_tp = &dummy_type;
		}

		break;
	    }

	    case SLASH: {
		/*
		--  Both operands are real, or both are integer, or
		--  one is real and the other is integer.  Convert
		--  any integer operand to real. The result is real.
		--  Call FLOAT_DIVIDE.
		*/
		if (real_operands(result_tp, tp2) ||
		    integer_operands(result_tp, tp2)) {
		    emit_push_operand(tp2);
		    emit_promote_to_real(result_tp, tp2);

		    emit_1(CALL, name_lit(FLOAT_DIVIDE));
		    emit_2(ADD, reg(SP), integer_lit(8));
		}
		else error(INCOMPATIBLE_TYPES);

		result_tp = real_typep;
		break;
	    }

	    case DIV:
	    case MOD: {
		/*
		--  integer <op> integer => integer
		--  AX = AX IDIV CX
		*/
		if (integer_operands(result_tp, tp2)) {
		    emit_2(MOVE, reg(CX), reg(AX));
		    emit_1(POP, reg(AX));
		    emit_2(SUBTRACT, reg(DX), reg(DX));
		    emit_1(DIVIDE, reg(CX));
		    if (op == MOD) emit_2(MOVE, reg(AX), reg(DX));
		}
		else error(INCOMPATIBLE_TYPES);

		result_tp = integer_typep;
		break;
	    }

	    case AND: {
		/*
		--  boolean AND boolean => boolean
		--  AX = AX AND DX
		*/
		if (boolean_operands(result_tp, tp2)) {
		    emit_1(POP, reg(DX));
		    emit_2(AND_BITS, reg(AX), reg(DX));
		}
		else error(INCOMPATIBLE_TYPES);

		result_tp = boolean_typep;
		break;
	    }
	}
    }

    return(result_tp);
}

/*--------------------------------------------------------------*/
/*  factor              Process a factor, which is an variable, */
/*                      a number, NOT followed by a factor, or  */
/*                      a parenthesized subexpression.  Return  */
/*                      a pointer to the type structure.        */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
factor()

{
    TYPE_STRUCT_PTR tp;

    switch (token) {

	case IDENTIFIER: {
	    SYMTAB_NODE_PTR idp;

	    search_and_find_all_symtab(idp);

	    switch (idp->defn.key) {

		case FUNC_DEFN:
		    get_token();
		    tp = routine_call(idp, TRUE);
		    break;

		case PROC_DEFN:
		    error(INVALID_IDENTIFIER_USAGE);
		    get_token();
		    actual_parm_list(idp, FALSE);
		    tp = &dummy_type;
		    break;

		case CONST_DEFN:
		    tp = constant_identifier(idp);
		    break;

		default:
		    tp = variable(idp, EXPR_USE);
		    break;
	    }

	    break;
	}

	case NUMBER: {
	    if (literal.type == INTEGER_LIT) {
		/*
		--  AX = value
		*/
		emit_2(MOVE, reg(AX),
		       integer_lit(literal.value.integer));
		tp = integer_typep;
	    }

	    else {  /* literal.type == REAL_LIT */
		/*
		--  DX:AX = value
		*/
		tp = float_literal(token_string, literal.value.real);
	    }

	    get_token();
	    break;
	}

	case STRING: {
	    int length = strlen(literal.value.string);

	    if (length == 1) {
		/*
		--  AH = 0
		--  AL = value
		*/
		emit_2(MOVE, reg(AX),
		       char_lit(literal.value.string[0]));
		tp = char_typep;
	    }
	    else {
		/*
		--  AX = address of string
		*/
		tp = string_literal(literal.value.string, length);
	    }

	    get_token();
	    break;
	}

	case NOT:
	    /*
	    --  AX = NOT AX
	    */
	    get_token();
	    tp = factor();
	    emit_2(XOR_BITS, reg(AX), integer_lit(1));
            break;

        case LPAREN:
	    get_token();
	    tp = expression();

	    if_token_get_else_error(RPAREN, MISSING_RPAREN);
            break;

	default:
	    error(INVALID_EXPRESSION);
	    tp = &dummy_type;
	    break;
    }

    return(tp);
}

/*--------------------------------------------------------------*/
/*  float_literal       Process a floating point literal.       */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
float_literal(string, value)

    char  string[];
    float value;

{
    SYMTAB_NODE_PTR np = search_symtab(string, symtab_display[1]);

    /*
    --  Enter the literal into the symbol table
    --  if it isn't already in there.
    */
    if (np == NULL) {
	np = enter_symtab(string, symtab_display[1]);
	np->defn.key = CONST_DEFN;
	np->defn.info.constant.value.real  = value;
	np->label_index = new_label_index();
	np->next = float_literal_list;
	float_literal_list = np;
    }

    /*
    --  DX:AX = value
    */
    emit_2(MOVE, reg(AX), word_label(FLOAT_LABEL_PREFIX,
				     np->label_index));
    emit_2(MOVE, reg(DX), high_dword_label(FLOAT_LABEL_PREFIX,
					   np->label_index));

    return(real_typep);
}

/*--------------------------------------------------------------*/
/*  string_literal      Process a string_literal.               */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
string_literal(string, length)

    char string[];
    int  length;

{
    SYMTAB_NODE_PTR np;
    TYPE_STRUCT_PTR tp = make_string_typep(length);
    char            buffer[MAX_SOURCE_LINE_LENGTH];

    sprintf(buffer, "'%s'", string);
    np = search_symtab(buffer, symtab_display[1]);

    /*
    --  Enter the literal into the symbol table
    --  if it isn't already in there.
    */
    if (np == NULL) {
	np = enter_symtab(buffer, symtab_display[1]);
	np->defn.key = CONST_DEFN;
	np->label_index = new_label_index();
	np->next = string_literal_list;
	string_literal_list = np;
    }

    /*
    --  AX = address of string
    */
    emit_2(LOAD_ADDRESS, reg(AX),
	   word_label(STRING_LABEL_PREFIX, np->label_index));
    emit_1(PUSH, reg(AX));
    return(tp);
}

/*--------------------------------------------------------------*/
/*  constant_identifier         Process a constant identifier.  */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
constant_identifier(idp)

    SYMTAB_NODE_PTR idp;                /* id of constant */

{
    TYPE_STRUCT_PTR tp = idp->typep;    /* type of constant */

    get_token();

    if ((tp == integer_typep) || (tp->form == ENUM_FORM)) {
	/*
	--  AX = value
	*/
	emit_2(MOVE, reg(AX),
	       integer_lit(idp->defn.info.constant.value.integer));
    }
    else if (tp == char_typep) {
	/*
	--  AX = value
	*/
	emit_2(MOVE, reg(AX),
	       char_lit(idp->defn.info.constant.value.character));
    }
    else if (tp == real_typep) {
	/*
	--  Create a literal and then call float_literal.
	*/
	float value = idp->defn.info.constant.value.real;
	char  string[MAX_SOURCE_LINE_LENGTH];

	sprintf(string, "%e", value);
	float_literal(string, value);
    }
    else  /* string constant */  {
	string_literal(idp->defn.info.constant.value.stringp,
		       strlen(idp->defn.info.constant.value.stringp));
    }

    return(tp);
}

/*--------------------------------------------------------------*/
/*  variable            Process a variable, which can be a      */
/*                      simple identifier, an array identifier  */
/*                      with subscripts, or a record identifier */
/*                      with fields.                            */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
variable(var_idp, use)

    SYMTAB_NODE_PTR var_idp;    /* variable id */
    USE             use;        /* how variable is used */

{
    TYPE_STRUCT_PTR tp           = var_idp->typep;
    DEFN_KEY        defn_key     = var_idp->defn.key;
    BOOLEAN         varparm_flag = defn_key == VARPARM_DEFN;
    TYPE_STRUCT_PTR array_subscript_list();
    TYPE_STRUCT_PTR record_field();

    /*
    --  Check the variable's definition.
    */
    switch (defn_key) {
	case VAR_DEFN:
	case VALPARM_DEFN:
	case VARPARM_DEFN:
	case FUNC_DEFN:
	case UNDEFINED:  break;         /* OK */

	default:  {                     /* error */
	    tp = &dummy_type;
	    error(INVALID_IDENTIFIER_USAGE);
	}
    }

    get_token();

    /*
    --  There must not be a parameter list, but if there is one,
    --  parse it anyway for error recovery.
    */
    if (token == LPAREN) {
	error(UNEXPECTED_TOKEN);
	actual_parm_list(var_idp, FALSE);
	return(tp);
    }

    /*
    --  Subscripts and/or field designators?
    */
    if ((token == LBRACKET) || (token == PERIOD)) {
	/*
	--  Push the address of the array or record onto the
	--  stack, where it is then modified by code generated
	--  in array_subscript_list and record_field.
	*/
	emit_push_address(var_idp);

	while ((token == LBRACKET) || (token == PERIOD)) {
	    tp = token == LBRACKET ? array_subscript_list(tp)
				   : record_field(tp);
	}

	/*
	--  Leave the modified address on top of the stack if:
	--      it is an assignment target, or
	--      it represents a parameter passed by reference, or
	--      it is the address of an array or record.
	--  Otherwise, load AX with the value that the modified
	--  address points to.
	*/
	if ((use != TARGET_USE) && (use != VARPARM_USE) &&
	    (tp->form != ARRAY_FORM) && (tp->form != RECORD_FORM)) {
	    emit_1(POP, reg(BX));
	    if (tp == char_typep) {
		emit_2(SUBTRACT, reg(AX), reg(AX));
		emit_2(MOVE, reg(AL), byte_indirect(BX));
	    }
	    else if (tp == real_typep) {
		emit_2(MOVE, reg(AX), word_indirect(BX));
		emit_2(MOVE, reg(DX), high_dword_indirect(BX));
	    }
	    else emit_2(MOVE, reg(AX), word_indirect(BX));
	}
    }

    else if (use == TARGET_USE) {
	/*
	--  Push the address of an assignment target onto the stack,
	--  unless it is a local or global scalar parameter or
	--  variable.
	*/
	if (defn_key == FUNC_DEFN)
	   emit_push_return_value_address(var_idp);
	else if (varparm_flag || (tp->form == ARRAY_FORM) ||
		 (tp->form == RECORD_FORM) ||
		 ((var_idp->level > 1) && (var_idp->level < level)))
	    emit_push_address(var_idp);
    }
    else if (use == VARPARM_USE) {
	/*
	--  Push the address of a variable
	--  being passed as a VAR parameter.
	*/
	emit_push_address(var_idp);
    }
    else if ((tp->form == ARRAY_FORM) || (tp->form == RECORD_FORM)) {
	/*
	--  Push the address of an array or record value.
	*/
	emit_push_address(var_idp);
    }
    else {
	/*
	--  AX = scalar value
	*/
	emit_load_value(var_idp, base_type(tp));
    }

    return(tp);
}

/*--------------------------------------------------------------*/
/*  array_subscript_list        Process a list of subscripts    */
/*                              following an array identifier:  */
/*                                                              */
/*                                  [ <expr> , <expr> , ... ]   */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
array_subscript_list(tp)

    TYPE_STRUCT_PTR tp;

{
    TYPE_STRUCT_PTR   index_tp, elmt_tp, ss_tp;
    int               min_index, elmt_size;
    extern TOKEN_CODE statement_end_list[];

    /*
    --  Loop to process a subscript list.
    */
    do {
	if (tp->form == ARRAY_FORM) {
	    index_tp = tp->info.array.index_typep;
	    elmt_tp  = tp->info.array.elmt_typep;

	    get_token();
	    ss_tp = expression();

	    /*
	    --  The subscript expression must be assignment type
	    --  compatible with the corresponding subscript type.
	    */
	    if (!is_assign_type_compatible(index_tp, ss_tp))
		error(INCOMPATIBLE_TYPES);

	    min_index = tp->info.array.min_index;
	    elmt_size = tp->info.array.elmt_typep->size;

	    /*
	    --  Convert the subscript into an offset by subracting
	    --  the mininum index from it and then multiplying the
	    --  result by the element size.   Add the offset to the
	    --  address at the top of the stack.
	    */
	    if (min_index != 0) emit_2(SUBTRACT, reg(AX),
				       integer_lit(min_index));
	    if (elmt_size > 1) {
		emit_2(MOVE, reg(DX), integer_lit(elmt_size));
		emit_1(MULTIPLY, reg(DX));
	    }
	    emit_1(POP,  reg(DX));
	    emit_2(ADD,  reg(DX), reg(AX));
	    emit_1(PUSH, reg(DX));

	    tp = elmt_tp;
	}
	else {
	    error(TOO_MANY_SUBSCRIPTS);
	    while ((token != RBRACKET) &&
		   (! token_in(statement_end_list)))
	       get_token();
	}
    } while (token == COMMA);

    if_token_get_else_error(RBRACKET, MISSING_RBRACKET);
    return(tp);
}

/*--------------------------------------------------------------*/
/*  record_field                Process a field designation     */
/*                              following a record identifier:  */
/*                                                              */
/*                                  . <field-variable>          */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
record_field(tp)

    TYPE_STRUCT_PTR tp;

{
    SYMTAB_NODE_PTR field_idp;

    get_token();

    if ((token == IDENTIFIER) && (tp->form == RECORD_FORM)) {
	search_this_symtab(field_idp,
			   tp->info.record.field_symtab);
	get_token();

	/*
	--  Add the field's offset (using the numeric equate)
	--  to the address at the top of the stack.
	*/
	if (field_idp != NULL) {
	    emit_1(POP,  reg(AX));
	    emit_2(ADD,  reg(AX), tagged_name(field_idp));
	    emit_1(PUSH, reg(AX));
	    return(field_idp->typep);
	}
	else {
	    error(INVALID_FIELD);
	    return(&dummy_type);
	}
    }
    else {
	get_token();
	error(INVALID_FIELD);
	return(&dummy_type);
    }
}

		/********************************/
		/*                              */
		/*      Type compatibility      */
		/*                              */
		/********************************/

/*--------------------------------------------------------------*/
/*  check_rel_op_types  Check the operand types for a rela-     */
/*                      tional operator.                        */
/*--------------------------------------------------------------*/

check_rel_op_types(tp1, tp2)

    TYPE_STRUCT_PTR tp1, tp2;           /* operand types */

{
    /*
    --  Two identical scalar or enumeration types.
    */
    if (   (tp1 == tp2)
	&& ((tp1->form == SCALAR_FORM) || (tp1->form == ENUM_FORM)))
	return;

    /*
    --  One integer and one real.
    */
    if (   ((tp1 == integer_typep) && (tp2 == real_typep))
	|| ((tp2 == integer_typep) && (tp1 == real_typep))) return;

    /*
    --  Two strings of the same length.
    */
    if ((tp1->form == ARRAY_FORM) &&
	(tp2->form == ARRAY_FORM) &&
	(tp1->info.array.elmt_typep == char_typep) &&
	(tp2->info.array.elmt_typep == char_typep) &&
	(tp1->info.array.elmt_count ==
				tp2->info.array.elmt_count)) return;

    error(INCOMPATIBLE_TYPES);
}

/*--------------------------------------------------------------*/
/*  is_assign_type_compatible   Return TRUE iff a value of type */
/*                              tp1 can be assigned to a vari-  */
/*                              able of type tp1.               */
/*--------------------------------------------------------------*/

    BOOLEAN
is_assign_type_compatible(tp1, tp2)

    TYPE_STRUCT_PTR tp1, tp2;

{
    tp1 = base_type(tp1);
    tp2 = base_type(tp2);

    if (tp1 == tp2) return(TRUE);

    /*
    --  real := integer
    */
    if ((tp1 == real_typep) && (tp2 == integer_typep)) return(TRUE);

    /*
    --  string1 := string2 of the same length
    */
    if ((tp1->form == ARRAY_FORM) &&
	(tp2->form == ARRAY_FORM) &&
	(tp1->info.array.elmt_typep == char_typep) &&
	(tp2->info.array.elmt_typep == char_typep) &&
	(tp1->info.array.elmt_count ==
			tp2->info.array.elmt_count)) return(TRUE);

    return(FALSE);
}

/*--------------------------------------------------------------*/
/*  base_type           Return the range type of a subrange     */
/*                      type.                                   */
/*--------------------------------------------------------------*/

    TYPE_STRUCT_PTR
base_type(tp)

    TYPE_STRUCT_PTR tp;

{
    return((tp->form == SUBRANGE_FORM)
	       ? tp->info.subrange.range_typep
	       : tp);
}



/****************************************************************/
/*                                                              */
/*      C O D E   G E N E R A T O R   (Header)                  */
/*                                                              */
/*      FILE:       code.h                                      */
/*                                                              */
/*      MODULE:     code                                        */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#ifndef code_h
#define code_h

#include "common.h"

/*--------------------------------------------------------------*/
/*  Assembly label prefixes                                     */
/*--------------------------------------------------------------*/

#define STMT_LABEL_PREFIX       "$L"
#define FLOAT_LABEL_PREFIX      "$F"
#define STRING_LABEL_PREFIX     "$S"

/*--------------------------------------------------------------*/
/*  Names of library routines                                   */
/*--------------------------------------------------------------*/

#define FLOAT_NEGATE    "_float_negate"
#define FLOAT_ADD       "_float_add"
#define FLOAT_SUBTRACT  "_float_subtract"
#define FLOAT_MULTIPLY  "_float_multiply"
#define FLOAT_DIVIDE    "_float_divide"
#define FLOAT_COMPARE   "_float_compare"
#define FLOAT_CONVERT   "_float_convert"

#define WRITE_INTEGER   "_write_integer"
#define WRITE_REAL      "_write_real"
#define WRITE_BOOLEAN   "_write_boolean"
#define WRITE_CHAR      "_write_char"
#define WRITE_STRING    "_write_string"
#define WRITE_LINE      "_write_line"

#define READ_INTEGER    "_read_integer"
#define READ_REAL       "_read_real"
#define READ_CHAR       "_read_char"
#define READ_LINE       "_read_line"

#define STD_END_OF_FILE "_std_end_of_file"
#define STD_END_OF_LINE "_std_end_of_line"

#define STD_ABS         "_std_abs"

#define STD_ARCTAN      "_std_arctan"
#define STD_COS         "_std_cos"
#define STD_EXP         "_std_exp"
#define STD_LN          "_std_ln"
#define STD_SIN         "_std_sin"
#define STD_SQRT        "_std_sqrt"

#define STD_ROUND       "_std_round"
#define STD_TRUNC       "_std_trunc"

/*--------------------------------------------------------------*/
/*  Stack frame                                                 */
/*--------------------------------------------------------------*/

#define PROC_LOCALS_STACK_FRAME_OFFSET   0
#define FUNC_LOCALS_STACK_FRAME_OFFSET  -4
#define PARAMETERS_STACK_FRAME_OFFSET   +6

#define STATIC_LINK         "$STATIC_LINK"          /* EQU <bp+4> */
#define RETURN_VALUE        "$RETURN_VALUE"         /* EQU <bp-4> */
#define HIGH_RETURN_VALUE   "$HIGH_RETURN_VALUE"    /* EQU <bp-2> */

/*--------------------------------------------------------------*/
/*  Registers and instruction op codes                          */
/*--------------------------------------------------------------*/

typedef enum {
    AX, AH, AL, BX, BH, BL, CX, CH, CL, DX, DH, DL,
    CS, DS, ES, SS, SP, BP, SI, DI,
} REGISTER;

typedef enum {
    MOVE, MOVE_BLOCK, LOAD_ADDRESS, EXCHANGE,
    COMPARE, COMPARE_STRINGS, POP, PUSH, AND_BITS, OR_BITS, XOR_BITS,
    NEGATE, INCREMENT, DECREMENT, ADD, SUBTRACT, MULTIPLY, DIVIDE,
    CLEAR_DIRECTION, CALL, RETURN,
    JUMP, JUMP_LT, JUMP_LE, JUMP_EQ, JUMP_NE, JUMP_GE, JUMP_GT,
} INSTRUCTION;

	/************************************************/
	/*                                              */
	/*      Macros to emit assembly statements      */
	/*                                              */
	/************************************************/

/*--------------------------------------------------------------*/
/*  emit                Emit a no-operand instruction.          */
/*--------------------------------------------------------------*/

#define emit(opcode)                        \
{                                           \
    operator(opcode);                       \
    fprintf(code_file, "%s\n", asm_buffer); \
    asm_bufferp = asm_buffer;               \
}

/*--------------------------------------------------------------*/
/*  emit_1              Emit a one-operand instruction.         */
/*--------------------------------------------------------------*/

#define emit_1(opcode, operand1)            \
{                                           \
    operator(opcode);                       \
    *asm_bufferp++ = '\t';                  \
    operand1;                               \
    fprintf(code_file, "%s\n", asm_buffer); \
    asm_bufferp = asm_buffer;               \
}

/*--------------------------------------------------------------*/
/*  emit_2              Emit a two-operand instruction.         */
/*--------------------------------------------------------------*/

#define emit_2(opcode, operand1, operand2)  \
{                                           \
    operator(opcode);                       \
    *asm_bufferp++ = '\t';                  \
    operand1;                               \
    *asm_bufferp++ = ',';                   \
    operand2;                               \
    fprintf(code_file, "%s\n", asm_buffer); \
    asm_bufferp = asm_buffer;               \
}

/*--------------------------------------------------------------*/
/*  emit_label          Emit a statement label.                 */
/*--------------------------------------------------------------*/

#define emit_label(prefix, index)   fprintf(code_file,      \
					    "%s_%03d:\n",   \
					    prefix, index);

/*--------------------------------------------------------------*/
/*  advance_asm_bufferp     Advance asm_bufferp to the end      */
/*                          of the assembly statement.          */
/*--------------------------------------------------------------*/

#define advance_asm_bufferp()   while (*asm_bufferp != '\0') \
				    ++asm_bufferp;

/*--------------------------------------------------------------*/
/*  new_label_index             Return a new label index.       */
/*--------------------------------------------------------------*/

#define new_label_index()       ++label_index

#endif

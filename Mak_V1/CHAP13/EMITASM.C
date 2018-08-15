/****************************************************************/
/*                                                              */
/*      E M I T   A S S E M B L Y   S T A T E M E N T S         */
/*                                                              */
/*      Routines for generating and emitting                    */
/*      language statements.                                    */
/*                                                              */
/*      FILE:       emitasm.c                                   */
/*                                                              */
/*      MODULE:     code                                        */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#include <stdio.h>
#include "symtab.h"
#include "code.h"

/*--------------------------------------------------------------*/
/*  Globals                                                     */
/*--------------------------------------------------------------*/

int label_index = 0;

char asm_buffer[MAX_PRINT_LINE_LENGTH];     /* assembly stmt buffer */
char *asm_bufferp = asm_buffer;             /* ptr into asm buffer */

char *register_strings[] = {
    "ax", "ah", "al", "bx", "bh", "bl", "cx", "ch", "cl",
    "dx", "dh", "dl", "cs", "ds", "es", "ss",
    "sp", "bp", "si", "di",
};

char *instruction_strings[] = {
    "mov", "rep\tmovsb", "lea", "xchg", "cmp", "repe\tcmpsb",
    "pop", "push", "and", "or", "xor",
    "neg", "inc", "dec", "add", "sub", "imul", "idiv",
    "cld", "call", "ret",
    "jmp", "jl", "jle", "je", "jne", "jge", "jg",
};

	/************************************************/
	/*                                              */
	/*      Write parts of assembly statements      */
	/*                                              */
	/************************************************/

/*--------------------------------------------------------------*/
/*  label               Write a generic label constructed from  */
/*                      the prefix and the label index.         */
/*                                                              */
/*                      Example:        $L_007                  */
/*--------------------------------------------------------------*/

label(prefix, index)

    char *prefix;
    int  index;

{
    sprintf(asm_bufferp, "%s_%03d", prefix, index);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  word_label          Write a word label constructed from     */
/*                      the prefix and the label index.         */
/*                                                              */
/*                      Example:        WORD PTR $F_007         */
/*--------------------------------------------------------------*/

word_label(prefix, index)

    char *prefix;
    int  index;

{
    sprintf(asm_bufferp, "WORD PTR %s_%03d", prefix, index);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  high_dword_label    Write a word label constructed from     */
/*                      the prefix and the label index and      */
/*                      offset by 2 to point to the high word   */
/*                      of a double word.                       */
/*                                                              */
/*                      Example:        WORD PTR $F_007+2       */
/*--------------------------------------------------------------*/

high_dword_label(prefix, index)

    char *prefix;
    int  index;

{
    sprintf(asm_bufferp, "WORD PTR %s_%03d+2", prefix, index);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  reg                 Write a register name.  Example:  ax    */
/*--------------------------------------------------------------*/

reg(r)

    REGISTER r;

{
    sprintf(asm_bufferp, "%s", register_strings[r]);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  operator              Write an opcode.  Example:  add       */
/*--------------------------------------------------------------*/

operator(opcode)

    INSTRUCTION opcode;

{
    sprintf(asm_bufferp, "\t%s", instruction_strings[opcode]);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  byte                Write a byte label constructed from     */
/*                      the id name and its label index.        */
/*                                                              */
/*                      Example:        BYTE_PTR ch_007         */
/*--------------------------------------------------------------*/

byte(idp)

    SYMTAB_NODE_PTR idp;

{
    sprintf(asm_bufferp, "BYTE PTR %s_%03d",
			 idp->name, idp->label_index);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  byte_indirect       Write an indirect reference to a byte   */
/*                      via a register.                         */
/*                                                              */
/*                      Example:        BYTE PTR [bx]           */
/*--------------------------------------------------------------*/

byte_indirect(r)

    REGISTER r;

{
    sprintf(asm_bufferp, "BYTE PTR [%s]", register_strings[r]);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  word                Write a word label constructed from     */
/*                      the id name and its label index.        */
/*                                                              */
/*                      Example:        WORD_PTR sum_007        */
/*--------------------------------------------------------------*/

word(idp)

    SYMTAB_NODE_PTR idp;

{
    sprintf(asm_bufferp, "WORD PTR %s_%03d",
			 idp->name, idp->label_index);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  high_dword          Write a word label constructed from     */
/*                      the id name and its label index and     */
/*                      offset by 2 to point to the high word   */
/*                      of a double word.                       */
/*                                                              */
/*                      Example:        WORD_PTR sum_007+2      */
/*--------------------------------------------------------------*/

high_dword(idp)

    SYMTAB_NODE_PTR idp;

{
    sprintf(asm_bufferp, "WORD PTR %s_%03d+2",
			 idp->name, idp->label_index);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  word_indirect       Write an indirect reference to a word   */
/*                      via a register.                         */
/*                                                              */
/*                      Example:        WORD PTR [bx]           */
/*--------------------------------------------------------------*/

word_indirect(r)

    REGISTER r;

{
    sprintf(asm_bufferp, "WORD PTR [%s]", register_strings[r]);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  high_dword_indirect     Write an indirect reference to the  */
/*                          high word of a double word via a    */
/*                          register.                           */
/*                                                              */
/*                          Example:        WORD PTR [bx+2]     */
/*--------------------------------------------------------------*/

high_dword_indirect(r)

    REGISTER r;

{
    sprintf(asm_bufferp, "WORD PTR [%s+2]", register_strings[r]);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  tagged_name         Write an id name tagged with the id's   */
/*                      label index.                            */
/*                                                              */
/*                      Example:        x_007                   */
/*--------------------------------------------------------------*/

tagged_name(idp)

    SYMTAB_NODE_PTR idp;

{
    sprintf(asm_bufferp, "%s_%03d", idp->name, idp->label_index);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  name_lit            Write a literal name.                   */
/*                                                              */
/*                      Example:        _float_convert          */
/*--------------------------------------------------------------*/

name_lit(name)

    char *name;

{
    sprintf(asm_bufferp, "%s", name);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  integer_lit         Write an integer as a string.           */
/*--------------------------------------------------------------*/

integer_lit(n)

    int n;

{
    sprintf(asm_bufferp, "%d", n);
    advance_asm_bufferp();
}

/*--------------------------------------------------------------*/
/*  char_lit            Write a character surrounded by single  */
/*                      quotes.                                 */
/*--------------------------------------------------------------*/

char_lit(ch)

    char ch;

{
    sprintf(asm_bufferp, "'%c'", ch);
    advance_asm_bufferp();
}

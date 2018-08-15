//fig 12-8
//  *************************************************************
//  *                                                           *
//  *   E M I T   A S S E M B L Y   S T A T E M E N T S         *
//  *                                                           *
//  *   Routines for generating and emitting                    *
//  *   language statements.                                    *
//  *                                                           *
//  *   CLASSES: TAssemblyBuffer, TCodeGenerator                *
//  *                                                           *
//  *   FILE:    prog13-1/emitasm.cpp                           *
//  *                                                           *
//  *   MODULE:  Code generator                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include "buffer.h"
#include "symtab.h"
#include "codegen.h"

//--------------------------------------------------------------
//  Registers and instructions
//--------------------------------------------------------------

char *registers[] = {
    "ax", "ah", "al", "bx", "bh", "bl", "cx", "ch", "cl",
    "dx", "dh", "dl", "cs", "ds", "es", "ss",
    "sp", "bp", "si", "di",
};

char *instructions[] = {
    "mov", "rep\tmovsb", "lea", "xchg", "cmp", "repe\tcmpsb",
    "pop", "push", "and", "or", "xor",
    "neg", "inc", "dec", "add", "sub", "imul", "idiv",
    "cld", "call", "ret",
    "jmp", "jl", "jle", "je", "jne", "jge", "jg",
};

//              *********************
//              *                   *
//              *  Assembly Buffer  *
//              *                   *
//              *********************

//--------------------------------------------------------------
//  Constructor     Construct an assembly buffer by opening the
//                  output assembly file.
//
//      pAssemblyFileName : ptr to the name of the assembly file
//      ac                : abort code to use if open failed
//--------------------------------------------------------------

TAssemblyBuffer::TAssemblyBuffer(const char *pAssemblyFileName,
				 TAbortCode ac)
{
    //--Open the assembly output file.  Abort if failed.
    file.open(pAssemblyFileName, ios::out);
    if (!file.good()) AbortTranslation(ac);
    
    Reset();
}

//--------------------------------------------------------------
//  Advance         Advance pText to the end of the buffer
//                  contents.
//--------------------------------------------------------------

void TAssemblyBuffer::Advance(void)
{
    while (*pText) {
	++pText;
	++textLength;
    }
}

//              ***************************************
//              *                                     *
//              *  Emit parts of assembly statements  *
//              *                                     *
//              ***************************************

//--------------------------------------------------------------
//  Reg                 Emit a register name.  Example:  ax
//
//      r : register code
//--------------------------------------------------------------

void TCodeGenerator::Reg(TRegister r)
{
    Put(registers[r]);
}

//--------------------------------------------------------------
//  Operator            Emit an opcode.  Example:  add
//
//      opcode : operator code
//--------------------------------------------------------------

void TCodeGenerator::Operator(TInstruction opcode)
{
    Put('\t');
    Put(instructions[opcode]);
}

//--------------------------------------------------------------
//  Label               Emit a generic label constructed from
//                      the prefix and the label index.
//                                                              
//                      Example:        $L_007
//
//      pPrefix : ptr to label prefix
//      index   : index value
//--------------------------------------------------------------

void TCodeGenerator::Label(const char *pPrefix, int index)
{
    sprintf(AsmText(), "%s_%03d", pPrefix, index);
    Advance();
}

//--------------------------------------------------------------
//  WordLabel           Emit a word label constructed from
//                      the prefix and the label index.
//                                                              
//                      Example:        WORD PTR $F_007         
//
//      pPrefix : ptr to label prefix
//      index   : index value
//--------------------------------------------------------------

void TCodeGenerator::WordLabel(const char *pPrefix, int index)
{
    sprintf(AsmText(), "WORD PTR %s_%03d", pPrefix, index);
    Advance();
}

//--------------------------------------------------------------
//  HighDWordLabel      Emit a word label constructed from
//                      the prefix and the label index and
//                      offset by 2 to point to the high Word
//                      of a double Word.
//
//                      Example:        WORD PTR $F_007+2
//
//      pPrefix : ptr to label prefix
//      index   : index value
//--------------------------------------------------------------

void TCodeGenerator::HighDWordLabel(const char *pPrefix, int index)
{
    sprintf(AsmText(), "WORD PTR %s_%03d+2", pPrefix, index);
    Advance();
}

//--------------------------------------------------------------
//  Byte                Emit a byte label constructed from
//                      the id name and its label index.
//
//                      Example:        BYTE_PTR ch_007
//
//      pId : ptr to symbol table node
//--------------------------------------------------------------

void TCodeGenerator::Byte(const TSymtabNode *pId)
{
    sprintf(AsmText(), "BYTE PTR %s_%03d",
		       pId->String(), pId->labelIndex);
    Advance();
}

//--------------------------------------------------------------
//  Word                Emit a word label constructed from
//                      the id name and its label index.
//
//                      Example:        WORD_PTR sum_007
//
//      pId : ptr to symbol table node
//--------------------------------------------------------------

void TCodeGenerator::Word(const TSymtabNode *pId)
{
    sprintf(AsmText(), "WORD PTR %s_%03d",
		       pId->String(), pId->labelIndex);
    Advance();
}

//--------------------------------------------------------------
//  HighDWord           Emit a word label constructed from     
//                      the id name and its label index and
//                      offset by 2 to point to the high word
//                      of a double Word.                       
//                                                              
//                      Example:        WORD_PTR sum_007+2      
//
//      pId : ptr to symbol table node
//--------------------------------------------------------------

void TCodeGenerator::HighDWord(const TSymtabNode *pId)
{
    sprintf(AsmText(), "WORD PTR %s_%03d+2",
		       pId->String(), pId->labelIndex);
    Advance();
}

//--------------------------------------------------------------
//  ByteIndirect        Emit an indirect reference to a byte
//                      via a register.
//
//                      Example:        BYTE PTR [bx]
//
//      r : register code
//--------------------------------------------------------------

void TCodeGenerator::ByteIndirect(TRegister r)
{
    sprintf(AsmText(), "BYTE PTR [%s]", registers[r]);
    Advance();
}

//--------------------------------------------------------------
//  WordIndirect        Emit an indirect reference to a word
//                      via a register.
//                                                              
//                      Example:        WORD PTR [bx]           
//
//      r : register code
//--------------------------------------------------------------

void TCodeGenerator::WordIndirect(TRegister r)
{
    sprintf(AsmText(), "WORD PTR [%s]", registers[r]);
    Advance();
}

//--------------------------------------------------------------
//  HighDWordIndirect   Emit an indirect reference to the high
//                      word of a double word via a register.
//                                                              
//                      Example:        WORD PTR [bx+2]
//
//      r : register code
//--------------------------------------------------------------

void TCodeGenerator::HighDWordIndirect(TRegister r)
{
    sprintf(AsmText(), "WORD PTR [%s+2]", registers[r]);
    Advance();
}

//--------------------------------------------------------------
//  TaggedName          Emit an id name tagged with the id's
//                      label index.
//                                                              
//                      Example:        x_007                   
//
//      pId : ptr to symbol table node
//--------------------------------------------------------------

void TCodeGenerator::TaggedName(const TSymtabNode *pId)
{
    sprintf(AsmText(), "%s_%03d", pId->String(), pId->labelIndex);
    Advance();
}

//--------------------------------------------------------------
//  NameLit             Emit a literal name.
//                                                              
//                      Example:        _FloatConvert
//
//      pName : ptr to name
//--------------------------------------------------------------

void TCodeGenerator::NameLit(const char *pName)
{
    sprintf(AsmText(), "%s", pName);
    Advance();
}

//--------------------------------------------------------------
//  IntegerLit          Emit an integer as a string.
//
//      n : integer value
//--------------------------------------------------------------

void TCodeGenerator::IntegerLit(int n)
{
    sprintf(AsmText(), "%d", n);
    Advance();
}

//--------------------------------------------------------------
//  CharLit             Emit a character surrounded by single
//                      quotes.
//
//      ch : character value
//--------------------------------------------------------------

void TCodeGenerator::CharLit(char ch)
{
    sprintf(AsmText(), "'%c'", ch);
    Advance();
}
//endfig

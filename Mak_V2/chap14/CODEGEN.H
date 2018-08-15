//fig 12-7
//  *************************************************************
//  *                                                           *
//  *   C O D E   G E N E R A T O R   (Header)                  *
//  *                                                           *
//  *   CLASSES: TAssemblyBuffer, TCodeGenerator                *
//  *                                                           *
//  *   FILE:    prog13-1/codegen.h                             *
//  *                                                           *
//  *   MODULE:  Code generator                                 *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef codegen_h
#define codegen_h

#include <string.h>
#include <fstream.h>
#include <iomanip.h>
#include "error.h"
#include "buffer.h"
#include "symtab.h"
#include "types.h"
#include "icode.h"
#include "backend.h"
#include "parser.h"

//--------------------------------------------------------------
//  Assembly label prefixes
//--------------------------------------------------------------

#define STMT_LABEL_PREFIX       "$L"
#define FLOAT_LABEL_PREFIX      "$F"
#define STRING_LABEL_PREFIX     "$S"

//--------------------------------------------------------------
//  Runtime stack frame items
//--------------------------------------------------------------

#define STATIC_LINK             "$STATIC_LINK"
#define RETURN_VALUE            "$RETURN_VALUE"
#define HIGH_RETURN_VALUE       "$HIGH_RETURN_VALUE"

//--------------------------------------------------------------
//  Names of library routines
//--------------------------------------------------------------

#define FLOAT_NEGATE    "_FloatNegate"
#define FLOAT_ADD       "_FloatAdd"
#define FLOAT_SUBTRACT  "_FloatSubtract"
#define FLOAT_MULTIPLY  "_FloatMultiply"
#define FLOAT_DIVIDE    "_FloatDivide"
#define FLOAT_COMPARE   "_FloatCompare"
#define FLOAT_CONVERT   "_FloatConvert"

#define READ_INTEGER    "_ReadInteger"
#define READ_REAL       "_ReadReal"
#define READ_CHAR       "_ReadChar"
#define READ_LINE       "_ReadLine"

#define WRITE_INTEGER   "_WriteInteger"
#define WRITE_REAL      "_WriteReal"
#define WRITE_BOOLEAN   "_WriteBoolean"
#define WRITE_CHAR      "_WriteChar"
#define WRITE_STRING    "_WriteString"
#define WRITE_LINE      "_WriteLine"

#define STD_EOF         "_StdEof"
#define STD_EOLN        "_StdEoln"
#define STD_ABS         "_StdAbs"
#define STD_ARCTAN      "_StdArctan"
#define STD_COS         "_StdCos"
#define STD_EXP         "_StdExp"
#define STD_LN          "_StdLn"
#define STD_SIN         "_StdSin"
#define STD_SQRT        "_StdSqrt"
#define STD_ROUND       "_StdRound"
#define STD_TRUNC       "_StdTrunc"

//--------------------------------------------------------------
//  Emit0               Emit a no-operand instruction.
//--------------------------------------------------------------

#define Emit0(opcode)                   \
{                                       \
    Operator(opcode);                   \
    pAsmBuffer->PutLine();              \
}

//--------------------------------------------------------------
//  Emit1               Emit a one-operand instruction.
//--------------------------------------------------------------

#define Emit1(opcode, operand1)         \
{                                       \
    Operator(opcode);                   \
    pAsmBuffer->Put('\t');              \
    operand1;                           \
    pAsmBuffer->PutLine();              \
}

//--------------------------------------------------------------
//  Emit2               Emit a two-operand instruction.
//--------------------------------------------------------------

#define Emit2(opcode, operand1, operand2)       \
{                                               \
    Operator(opcode);                           \
    pAsmBuffer->Put('\t');                      \
    operand1;                                   \
    pAsmBuffer->Put(',');                       \
    operand2;                                   \
    pAsmBuffer->PutLine();                      \
}

//--------------------------------------------------------------
//  TRegister           Machine registers.
//--------------------------------------------------------------

enum TRegister {
    ax, ah, al, bx, bh, bl, cx, ch, cl, dx, dh, dl,
    cs, ds, es, ss, sp, bp, si, di,
};

//--------------------------------------------------------------
//  TInstruction        Assembly instructions.
//--------------------------------------------------------------

enum TInstruction {
    mov, rep_movsb, lea, xchg, cmp, repe_cmpsb, pop, push,
    and, or, xor, neg, incr, decr, add, sub, imul, idiv,
    cld, call, ret, jmp, jl, jle, je, jne, jge, jg,
};

//--------------------------------------------------------------
//  TAssemblyBuffer     Assembly language buffer subclass of
//                      TTextOutBuffer.
//--------------------------------------------------------------

class TAssemblyBuffer : public TTextOutBuffer {
    enum {
	maxLength = 72,
    };

    fstream  file;        // assembly output file
    char    *pText;       // assembly buffer pointer
    int      textLength;  // length of assembly comment

public:
    TAssemblyBuffer(const char *pAsmFileName, TAbortCode ac);

    char *Text (void) const { return pText; }

    void Reset(void)
    {
	pText = text;
	text[0] = '\0';
	textLength = 0;
    }

    void Put(char ch) { *pText++ = ch; *pText = '\0'; ++textLength; }
    virtual void PutLine(void) { file << text << endl; Reset(); }

    void PutLine(const char *pText)
    {
	TTextOutBuffer::PutLine(pText);
    }

    void Advance(void);

    void Put(const char *pString)
    {
	strcpy(pText, pString);
	Advance();
    }

    void Reset(const char *pString) { Reset(); Put(pString); }

    int Fit(int length) const
    {
	return textLength + length < maxLength;
    }
};

//--------------------------------------------------------------
//  TCodeGenerator      Code generator subclass of TBackend.
//--------------------------------------------------------------

class TCodeGenerator : public TBackend {
    TAssemblyBuffer *const pAsmBuffer;

    //--Pointers to the list of all the float and string literals
    //--used in the source program.
    TSymtabNode *pFloatLitList;
    TSymtabNode *pStringLitList;

    void Reg              (TRegister r);
    void Operator         (TInstruction opcode);
    void Label            (const char *prefix, int index);
    void WordLabel        (const char *prefix, int index);
    void HighDWordLabel   (const char *prefix, int index);
    void Byte             (const TSymtabNode *pId);
    void Word             (const TSymtabNode *pId);
    void HighDWord        (const TSymtabNode *pId);
    void ByteIndirect     (TRegister r);
    void WordIndirect     (TRegister r);
    void HighDWordIndirect(TRegister r);
    void TaggedName       (const TSymtabNode *pId);
    void NameLit          (const char *pName);
    void IntegerLit       (int n);
    void CharLit          (char ch);

    void EmitStatementLabel(int index);

    //--Program
    void EmitProgramPrologue(void);
    void EmitProgramEpilogue(const TSymtabNode *pProgramId);
    void EmitMain(const TSymtabNode *pMainId);
    void EmitMainPrologue(void);
    void EmitMainEpilogue(void);

    //--Routines
    void   EmitRoutine               (const TSymtabNode *pRoutineId);
    void   EmitRoutinePrologue       (const TSymtabNode *pRoutineId);
    void   EmitRoutineEpilogue       (const TSymtabNode *pRoutineId);
    TType *EmitSubroutineCall        (const TSymtabNode *pRoutineId);
    TType *EmitDeclaredSubroutineCall(const TSymtabNode *pRoutineId);
    TType *EmitStandardSubroutineCall(const TSymtabNode *pRoutineId);
    void   EmitActualParameters      (const TSymtabNode *pRoutineId);

    //--Standard routines
    TType *EmitReadReadlnCall  (const TSymtabNode *pRoutineId);
    TType *EmitWriteWritelnCall(const TSymtabNode *pRoutineId);
    TType *EmitEofEolnCall     (const TSymtabNode *pRoutineId);
    TType *EmitAbsSqrCall      (const TSymtabNode *pRoutineId);
    TType *EmitArctanCosExpLnSinSqrtCall(const TSymtabNode *pRoutineId);
    TType *EmitPredSuccCall    (const TSymtabNode *pRoutineId);
    TType *EmitChrCall         (void);
    TType *EmitOddCall         (void);
    TType *EmitOrdCall         (void);
    TType *EmitRoundTruncCall  (const TSymtabNode *pRoutineId);

    //--Declarations
    void EmitDeclarations     (const TSymtabNode *pRoutineId);
    void EmitStackOffsetEquate(const TSymtabNode *pId);

    //--Loads and pushes
    void EmitAdjustBP     (int level);
    void EmitRestoreBP    (int level);
    void EmitLoadValue    (const TSymtabNode *pId);
    void EmitLoadFloatLit (TSymtabNode *pNode);
    void EmitPushStringLit(TSymtabNode *pNode);
    void EmitPushOperand  (const TType *pType);
    void EmitPushAddress  (const TSymtabNode *pId);
    void EmitPushReturnValueAddress(const TSymtabNode *pId);
    void EmitPromoteToReal(const TType *pType1, const TType *pType2);

    //--Statements
    void EmitStatement    (void);
    void EmitStatementList(TTokenCode terminator);
    void EmitAssignment   (const TSymtabNode *pTargetId);
    void EmitREPEAT       (void);
    void EmitWHILE        (void);
    void EmitIF           (void);
    void EmitFOR          (void);
    void EmitCASE         (void);
    void EmitCompound     (void);

    //--Expressions
    TType *EmitExpression(void);
    TType *EmitSimpleExpression(void);
    TType *EmitTerm      (void);
    TType *EmitFactor    (void);
    TType *EmitConstant  (TSymtabNode *pId);
    TType *EmitVariable  (const TSymtabNode *pId, int addressFlag);
    TType *EmitSubscripts(const TType *pType);
    TType *EmitField     (void);

    //--Assembly buffer
    char *AsmText(void)          { return pAsmBuffer->Text();  }
    void  Reset  (void)          { pAsmBuffer->Reset();        }
    void  Put    (char ch)       { pAsmBuffer->Put(ch);        }
    void  Put    (char *pString) { pAsmBuffer->Put(pString);   }
    void  PutLine(void)          { pAsmBuffer->PutLine();      }
    void  PutLine(char *pText)   { pAsmBuffer->PutLine(pText); }
    void  Advance(void)          { pAsmBuffer->Advance();      }

    //--Comments
    void PutComment  (const char *pString);
    void StartComment(int n);

    void StartComment(void) { Reset(); PutComment("; "); }

    void StartComment(const char *pString)
    {
	StartComment();
	PutComment(pString);
    }

    void EmitProgramHeaderComment    (const TSymtabNode *pProgramId);
    void EmitSubroutineHeaderComment (const TSymtabNode *pRoutineId);
    void EmitSubroutineFormalsComment(const TSymtabNode *pParmId);
    void EmitVarDeclComment          (const TSymtabNode *pVarId);
    void EmitTypeSpecComment         (const TType *pType);
    void EmitStmtComment             (void);
    void EmitAsgnOrCallComment       (void);
    void EmitREPEATComment           (void);
    void EmitUNTILComment            (void);
    void EmitWHILEComment            (void);
    void EmitIFComment               (void);
    void EmitFORComment              (void);
    void EmitCASEComment             (void);
    void EmitExprComment             (void);
    void EmitIdComment               (void);

public:
    TCodeGenerator(const char *pAsmName)
	: pAsmBuffer(new TAssemblyBuffer(pAsmName,
					 abortAssemblyFileOpenFailed))
    {
	pFloatLitList = pStringLitList = NULL;
    }

    virtual void Go(const TSymtabNode *pProgramId);
};

#endif
//endfig

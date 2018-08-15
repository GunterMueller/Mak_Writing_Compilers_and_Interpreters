//  *************************************************************
//  *                                                           *
//  *   S Y M B O L   T A B L E   (Header)                      *
//  *                                                           *
//  *   CLASSES: TDefn, TSymtabNode, TSymtab, TSymtabStack      *
//  *            TLineNumNode, TLineNumList                     *
//  *                                                           *
//  *   FILE:    prog8-1/symtab.h                               *
//  *                                                           *
//  *   MODULE:  Symbol table                                   *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef symtab_h
#define symtab_h

#include <string.h>
#include "misc.h"

extern int currentNestingLevel;
extern int currentLineNumber;
extern int asmLabelIndex;
extern int xrefFlag;

class TSymtab;
class TSymtabNode;
class TIcode;                       

//--------------------------------------------------------------
//  TDefnCode           Definition code: How an identifier
//                                       is defined.
//--------------------------------------------------------------

enum TDefnCode {
    dcUndefined,
    dcConstant, dcType, dcVariable, dcField,
    dcValueParm, dcVarParm,
    dcProgram, dcProcedure, dcFunction,
};

//--------------------------------------------------------------
//  TRoutineCode        Routine code: For procedures, functions,
//                                    and standard routines.
//--------------------------------------------------------------

enum TRoutineCode {
    rcDeclared, rcForward,
    rcRead, rcReadln, rcWrite, rcWriteln,
    rcAbs, rcArctan, rcChr, rcCos, rcEof, rcEoln,
    rcExp, rcLn, rcOdd, rcOrd, rcPred, rcRound,
    rcSin, rcSqr, rcSqrt, rcSucc, rcTrunc,
};

//--------------------------------------------------------------
//  TLocalIds           Local identifier lists structure.
//--------------------------------------------------------------

struct TLocalIds {
    TSymtabNode *pParmIds;      // ptr to local parm id list
    TSymtabNode *pConstantIds;  // ptr to local constant id list
    TSymtabNode *pTypeIds;      // ptr to local type id list
    TSymtabNode *pVariableIds;  // ptr to local variable id list
    TSymtabNode *pRoutineIds;   // ptr to local proc and func id list
};

//--------------------------------------------------------------
//  TDefn               Definition class.
//--------------------------------------------------------------

class TDefn {

public:
    TDefnCode how;  // the identifier was defined

    union {

	//--Constant
	struct {
	    TDataValue value;  // value of constant
	} constant;

	//--Procedure, function, or standard routine
	struct {
	    TRoutineCode  which;           // routine code
	    int           parmCount;       // count of parameters
	    int           totalParmSize;   // total byte size of parms
	    int           totalLocalSize;  // total byte size of locals
	    TLocalIds     locals;          // local identifiers
	    TSymtab      *pSymtab;         // ptr to local symtab
	    TIcode       *pIcode;          // ptr to routine's icode
	} routine;

	//--Variable, record field, or parameter
	struct {
	    int offset;  // vars and parms: sequence count
			 // fields: byte offset in record
	} data;
    };

    TDefn(TDefnCode dc) { how = dc; }
   ~TDefn(void);
};

//--------------------------------------------------------------
//  TSymtabNode         Symbol table node class.
//--------------------------------------------------------------

class TLineNumList;
class TType;

class TSymtabNode {
    TSymtabNode  *left, *right;  // ptrs to left and right subtrees
    char         *pString;       // ptr to symbol string
    short         xSymtab;       // symbol table index
    short         xNode;         // node index
    TLineNumList *pLineNumList;  // ptr to list of line numbers

    friend class TSymtab;

public:
    TSymtabNode *next;   // ptr to next sibling in chain
    TType       *pType;  // ptr to type info

    TDefn defn;          // definition info
    int   level;         // nesting level
    int   labelIndex;    // index for code label

    TSymtabNode(const char *pString, TDefnCode dc = dcUndefined);
   ~TSymtabNode(void);

    TSymtabNode *LeftSubtree (void) const { return left;    }
    TSymtabNode *RightSubtree(void) const { return right;   }
    char        *String      (void) const { return pString; }
    short        SymtabIndex (void) const { return xSymtab; }
    short        NodeIndex   (void) const { return xNode;   }
    void         Convert     (TSymtabNode *vpNodes[]);

    void Print          (void) const;
    void PrintIdentifier(void) const;
    void PrintConstant  (void) const;
    void PrintVarOrField(void) const;
    void PrintType      (void) const;
};

//--------------------------------------------------------------
//  TSymtab             Symbol table class.  The symbol table is
//                      organized as a binary tree that is
//                      sorted alphabetically by the nodes'
//                      name strings.
//--------------------------------------------------------------

class TSymtab {
    TSymtabNode  *root;      // ptr to binary tree root
    TSymtabNode **vpNodes;   // ptr to vector of node ptrs
    short         cntNodes;  // node counter
    short         xSymtab;   // symbol table index
    TSymtab      *next;      // ptr to next symbol table in list

public:
    TSymtab()
    {
	extern int      cntSymtabs;
	extern TSymtab *pSymtabList;

	root     = NULL;
	vpNodes  = NULL;
	cntNodes = 0;
	xSymtab  = cntSymtabs++;

	//--Insert at the head of the symbol table list.
	next        = pSymtabList;
	pSymtabList = this;
    }

   ~TSymtab()
    {
	delete   root;
	delete[] vpNodes;
    }

    TSymtabNode  *Search  (const char *pString) const;
    TSymtabNode  *Enter   (const char *pString,
			   TDefnCode dc = dcUndefined);
    TSymtabNode  *EnterNew(const char *pString,
			   TDefnCode dc = dcUndefined);

    TSymtabNode  *Root(void)        const { return root;           }
    TSymtabNode  *Get (short xNode) const { return vpNodes[xNode]; }
    TSymtab      *Next(void)        const { return next;           }
    TSymtabNode **NodeVector(void)  const { return vpNodes;        }
    int           NodeCount (void)  const { return cntNodes;       }
    void          Print     (void)  const { root->Print();         }
    void          Convert   (TSymtab *vpSymtabs[]);
};

//fig 8-5
//--------------------------------------------------------------
//  TSymtabStack      Symbol table stack class.
//--------------------------------------------------------------

class TSymtabStack {
    enum {maxNestingLevel = 8};

    TSymtab *pSymtabs[maxNestingLevel];  // stack of symbol table ptrs

public:
    TSymtabStack(void);
   ~TSymtabStack(void);

    TSymtabNode *SearchLocal(const char *pString)
    {
	return pSymtabs[currentNestingLevel]->Search(pString);
    }

    TSymtabNode *EnterLocal(const char *pString,
			    TDefnCode dc = dcUndefined)
    {
	return pSymtabs[currentNestingLevel]->Enter(pString, dc);
    }

    TSymtabNode *EnterNewLocal(const char *pString,
			       TDefnCode dc = dcUndefined)
    {
	return pSymtabs[currentNestingLevel]->EnterNew(pString, dc);
    }

    TSymtab *GetCurrentSymtab(void) const
    {
	return pSymtabs[currentNestingLevel];
    }

    void SetCurrentSymtab(TSymtab *pSymtab)
    {
	pSymtabs[currentNestingLevel] = pSymtab;
    }

    TSymtabNode *SearchAll (const char *pString) const;
    TSymtabNode *Find      (const char *pString) const;
    void         EnterScope(void);
    TSymtab     *ExitScope (void);
};
//endfig

//--------------------------------------------------------------
//  TLineNumNode        Line number node class.
//--------------------------------------------------------------

class TLineNumNode {
    TLineNumNode *next;    // ptr to next node
    const int     number;  // the line number

    friend class TLineNumList;

public:
    TLineNumNode(void)
	: number(currentLineNumber) { next = NULL; }
};

//--------------------------------------------------------------
//  TLineNumList        Line number list class.
//--------------------------------------------------------------

class TLineNumList {
    TLineNumNode *head, *tail;  // list head and tail

public:
    TLineNumList(void) { head = tail = new TLineNumNode; }
    virtual ~TLineNumList(void);

    void Update(void);
    void Print (int newLineFlag, int indent) const;
};

#endif

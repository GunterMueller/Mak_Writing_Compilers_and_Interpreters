//  *************************************************************
//  *                                                           *
//  *   S Y M B O L   T A B L E   (Header)                      *
//  *                                                           *
//  *	CLASSES: TSymtabNode,  TSymtab				*
//  *		 TLineNumNode, TLineNumList			*
//  *                                                           *
//  *   FILE:    prog5-2/symtab.h                               *
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

extern int xrefFlag;
extern int currentLineNumber;

//fig 5-17
//--------------------------------------------------------------
//  TSymtabNode         Symbol table node class.
//--------------------------------------------------------------

class TLineNumList;

class TSymtabNode {
    TSymtabNode  *left, *right;  // ptrs to left and right subtrees
    char         *pString;       // ptr to symbol string
    short         xSymtab;	 // symbol table index
    short         xNode;         // node index
    TLineNumList *pLineNumList;  // ptr to list of line numbers

    friend class TSymtab;

public:
    float value;  // temporary value data member

    TSymtabNode(const char *pStr);
   ~TSymtabNode(void);

    TSymtabNode *LeftSubtree (void) const { return left;    }
    TSymtabNode *RightSubtree(void) const { return right;   }
    char        *String      (void) const { return pString; }
    short        SymtabIndex (void) const { return xSymtab; }
    short        NodeIndex   (void) const { return xNode;   }
    void         Convert     (TSymtabNode *vpNodes[]);

    void Print(void) const;
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
    short	  xSymtab;   // symbol table index
    TSymtab      *next;	     // ptr to next symbol table in list

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

    TSymtabNode  *Search(const char *pString) const;
    TSymtabNode  *Enter (const char *pString);

    TSymtabNode  *Root(void)        const { return root;           }
    TSymtabNode  *Get (short xNode) const { return vpNodes[xNode]; }
    TSymtab      *Next(void)        const { return next;           }
    TSymtabNode **NodeVector(void)  const { return vpNodes;        }
    int           NodeCount (void)  const { return cntNodes;       }
    void          Print     (void)  const { root->Print();         }
    void          Convert   (TSymtab *vpSymtabs[]);
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

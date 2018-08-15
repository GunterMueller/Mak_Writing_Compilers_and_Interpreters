//  *************************************************************
//  *                                                           *
//  *   S Y M B O L   T A B L E                                 *
//  *                                                           *
//  *   Manage a symbol table.                      		*
//  *                                                           *
//  *	CLASSES: TDefn, TSymtabNode, TSymtab, TSymtabStack	*
//  *		 TLineNumNode, TLineNumList			*
//  *                                                           *
//  *   FILE:    prog8-1/symtab.cpp                             *
//  *                                                           *
//  *   MODULE:  Symbol table                                   *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <iostream.h>
#include "error.h"
#include "buffer.h"
#include "symtab.h"
#include "types.h"
#include "icode.h"

int asmLabelIndex = 0;      // assembly label index
int xrefFlag      = false;  // true = cross-referencing on, false = off

//              ****************
//              *              *
//              *  Definition  *
//              *              *
//              ****************

//--------------------------------------------------------------
//  Destructor      Delete the local symbol table and icode of a
//                  program, procedure or function definition.
//                  Note that the parameter and local identifier
//                  chains are deleted along with the local
//                  symbol table.
//--------------------------------------------------------------

TDefn::~TDefn(void)
{
    switch (how) {

	case dcProgram:
	case dcProcedure:
	case dcFunction:

	    if (routine.which == rcDeclared) {
		delete routine.pSymtab;
		delete routine.pIcode;
	    }
	    break;

	default:  break;
    }
}

//              ***********************
//              *                     *
//              *  Symbol Table Node  *
//              *                     *
//              ***********************

//--------------------------------------------------------------
//  Constructor     Construct a symbol table node by initial-
//                  izing its subtree pointers and the pointer
//                  to its symbol string.
//
//      pStr : ptr to the symbol string
//      dc   : definition code
//--------------------------------------------------------------

TSymtabNode::TSymtabNode(const char *pStr, TDefnCode dc)
    : defn(dc)
{
    left = right = next = NULL;
    pLineNumList = NULL;
    pType	 = NULL;
    xNode	 = 0;
    level	 = currentNestingLevel;
    labelIndex	 = ++asmLabelIndex;

    //--Allocate and copy the symbol string.
    pString = new char[strlen(pStr) + 1];
    strcpy(pString, pStr);

    //--If cross-referencing, update the line number list.
    if (xrefFlag) pLineNumList = new TLineNumList;
}

//--------------------------------------------------------------
//  Destructor      Deallocate a symbol table node.
//--------------------------------------------------------------

TSymtabNode::~TSymtabNode(void)
{
    void RemoveType(TType *&pType);

    //--First the subtrees (if any).
    delete left;
    delete right;

    //--Then delete this node's components.
    delete[] pString;
    delete   pLineNumList;
    RemoveType(pType);
}

//--------------------------------------------------------------
//  Print       Print the symbol table node to the list file.
//              First print the node's left subtree, then the
//              node itself, and finally the node's right
//              subtree.  For the node itself, first print its
//              symbol string, and then its line numbers.
//--------------------------------------------------------------

void TSymtabNode::Print(void) const
{
    const int maxNamePrintWidth = 16;

    //--Pirst, print left subtree
    if (left) left->Print();

    //--Print the node:  first the name, then the list of line numbers,
    //--                 and then the identifier information.
    sprintf(list.text, "%*s", maxNamePrintWidth, pString);
    if (pLineNumList) {
	pLineNumList->Print(strlen(pString) > maxNamePrintWidth,
			    maxNamePrintWidth);
    }
    else list.PutLine();
    PrintIdentifier();

    //--Finally, print right subtree
    if (right) right->Print();
}

//--------------------------------------------------------------
//  PrintIdentifier         Print information about an
//                          identifier's definition and type.
//--------------------------------------------------------------

void TSymtabNode::PrintIdentifier(void) const
{
    switch (defn.how) {
	case dcConstant:    PrintConstant();    break;
	case dcType:        PrintType();        break;

	case dcVariable:
	case dcField:       PrintVarOrField();  break;
    }
}

//--------------------------------------------------------------
//  PrintConstant       Print information about a constant
//                      identifier for the cross-reference.
//--------------------------------------------------------------

void TSymtabNode::PrintConstant(void) const
{
    extern TListBuffer list;

    list.PutLine();
    list.PutLine("Defined constant");

    //--Value
    if ((pType == pIntegerType) ||
	(pType->form == fcEnum)) {
	sprintf(list.text, "Value = %d",
			   defn.constant.value.integer);
    }
    else if (pType == pRealType) {
	sprintf(list.text, "Value = %g",
			   defn.constant.value.real);
    }
    else if (pType == pCharType) {
	sprintf(list.text, "Value = '%c'",
			   defn.constant.value.character);
    }
    else if (pType->form == fcArray) {
	sprintf(list.text, "Value = '%s'",
			   defn.constant.value.pString);
    }
    list.PutLine();

    //--Type information
    if (pType) pType->PrintTypeSpec(TType::vcTerse);
    list.PutLine();
}

//--------------------------------------------------------------
//  PrintVarOrField         Print information about a variable
//                          or record field identifier for the
//                          cross-reference.
//--------------------------------------------------------------

void TSymtabNode::PrintVarOrField(void) const
{
    extern TListBuffer list;

    list.PutLine();
    list.PutLine(defn.how == dcVariable ? "Declared variable"
					: "Declared record field");

    //--Type information
    if (pType) pType->PrintTypeSpec(TType::vcTerse);
    if ((defn.how == dcVariable) || (this->next)) list.PutLine();
}

//--------------------------------------------------------------
//  PrintType           Print information about a type
//                      identifier for the cross-reference.
//--------------------------------------------------------------

void TSymtabNode::PrintType(void) const
{
    list.PutLine();
    list.PutLine("Defined type");

    if (pType) pType->PrintTypeSpec(TType::vcVerbose);
    list.PutLine();
}

//--------------------------------------------------------------
//  Convert     Convert the symbol table node into a form
//		suitable for the back end.
//
//	vpNodes : vector of node ptrs
//--------------------------------------------------------------

void TSymtabNode::Convert(TSymtabNode *vpNodes[])
{
    //--First, convert the left subtree.
    if (left) left->Convert(vpNodes);

    //--Convert the node.
    vpNodes[xNode] = this;

    //--Finally, convert the right subtree.
    if (right) right->Convert(vpNodes);
}

//              ******************
//              *                *
//              *  Symbol Table  *
//              *                *
//              ******************

//--------------------------------------------------------------
//  Search      Search the symbol table for the node with a
//              given name string.
//
//      pString : ptr to the name string to search for
//
//  Return: ptr to the node if found, else NULL
//--------------------------------------------------------------

TSymtabNode *TSymtab::Search(const char *pString) const
{
    TSymtabNode *pNode = root;  // ptr to symbol table node
    int          comp;

    //--Loop to search the table.
    while (pNode) {
	comp = strcmp(pString, pNode->pString);  // compare names
	if (comp == 0) break;                    // found!

	//--Not yet found:  next search left or right subtree.
	pNode = comp < 0 ? pNode->left : pNode->right;
    }

    //--If found and cross-referencing, update the line number list.
    if (xrefFlag && (comp == 0)) pNode->pLineNumList->Update();

    return pNode;  // ptr to node, or NULL if not found
}

//--------------------------------------------------------------
//  Enter       Search the symbol table for the node with a
//              given name string.  If the node is found, return
//              a pointer to it.  Else if not found, enter a new
//              node with the name string, and return a pointer
//              to the new node.
//
//      pString : ptr to the name string to enter
//      dc      : definition code
//
//  Return: ptr to the node, whether existing or newly-entered
//--------------------------------------------------------------

TSymtabNode *TSymtab::Enter(const char *pString, TDefnCode dc)
{
    TSymtabNode  *pNode;           // ptr to node
    TSymtabNode **ppNode = &root;  // ptr to ptr to node

    //--Loop to search table for insertion point.
    while ((pNode = *ppNode) != NULL) {
	int comp = strcmp(pString, pNode->pString);  // compare strings
	if (comp == 0) return pNode;                 // found!

	//--Not yet found:  next search left or right subtree.
	ppNode = comp < 0 ? &(pNode->left) : &(pNode->right);
    }

    //--Create and insert a new node.
    pNode = new TSymtabNode(pString, dc);  // create a new node,
    pNode->xSymtab = xSymtab;              // set its symtab and
    pNode->xNode   = cntNodes++;           // node indexes,
    *ppNode        = pNode;                // insert it, and
    return pNode;                          // return a ptr to it
}

//--------------------------------------------------------------
//  EnterNew    Search the symbol table for the given name
//              string.  If the name is not already in there,
//              enter it.  Otherwise, flag the redefined
//              identifier error.
//
//      pString : ptr to name string to enter
//      dc      : definition code
//
//  Return: ptr to symbol table node
//--------------------------------------------------------------

TSymtabNode *TSymtab::EnterNew(const char *pString, TDefnCode dc)
{
    TSymtabNode *pNode = Search(pString);

    if (!pNode)  pNode = Enter(pString, dc);
    else         Error(errRedefinedIdentifier);

    return pNode;
}

//--------------------------------------------------------------
//  Convert     Convert the symbol table into a form suitable
//		for the back end.
//
//	vpSymtabs : vector of symbol table pointers
//--------------------------------------------------------------

void TSymtab::Convert(TSymtab *vpSymtabs[])
{
    //--Point the appropriate entry of the symbol table pointer vector
    //--to this symbol table.
    vpSymtabs[xSymtab] = this;

    //--Allocate the symbol table node pointer vector
    //--and convert the nodes.
    vpNodes = new TSymtabNode *[cntNodes];
    root->Convert(vpNodes);
}

//              ************************
//              *		       *
//              *  Symbol Table Stack  *
//              *		       *
//              ************************

//fig 8-6
//--------------------------------------------------------------
//  Constructor	    Initialize the global (level 0) symbol
//		    table, and set the others to NULL.
//--------------------------------------------------------------

TSymtabStack::TSymtabStack(void)
{
    extern TSymtab globalSymtab;
    void InitializeStandardRoutines(TSymtab *pSymtab);

    currentNestingLevel = 0;
    for (int i = 1; i < maxNestingLevel; ++i) pSymtabs[i] = NULL;

    //--Initialize the global nesting level.
    pSymtabs[0] = &globalSymtab;
    InitializePredefinedTypes (pSymtabs[0]);
    InitializeStandardRoutines(pSymtabs[0]);
}

//--------------------------------------------------------------
//  Destructor	    Remove the predefined types.
//--------------------------------------------------------------

TSymtabStack::~TSymtabStack(void)
{
    RemovePredefinedTypes();      
}

//--------------------------------------------------------------
//  SearchAll   Search the symbol table stack for the given
//              name string.
//
//      pString : ptr to name string to find
//
//  Return: ptr to symbol table node if found, else NULL
//--------------------------------------------------------------

TSymtabNode *TSymtabStack::SearchAll(const char *pString) const
{
    for (int i = currentNestingLevel; i >= 0; --i) {
	TSymtabNode *pNode = pSymtabs[i]->Search(pString);
	if (pNode) return pNode;
    }

    return NULL;
}

//--------------------------------------------------------------
//  Find        Search the symbol table stack for the given
//              name string.  If the name is not already in
//              there, flag the undefined identifier error,
//		and then enter the name into the local symbol
//		table.
//
//      pString : ptr to name string to find
//
//  Return: ptr to symbol table node
//--------------------------------------------------------------

TSymtabNode *TSymtabStack::Find(const char *pString) const
{
    TSymtabNode *pNode = SearchAll(pString);

    if (!pNode) {
	Error(errUndefinedIdentifier);
	pNode = pSymtabs[currentNestingLevel]->Enter(pString);
    }

    return pNode;
}

//--------------------------------------------------------------
//  EnterScope	Enter a new nested scope.  Increment the nesting
//		level.  Push new scope's symbol table onto the
//		stack.
//
//      pSymtab : ptr to scope's symbol table
//--------------------------------------------------------------

void TSymtabStack::EnterScope(void)
{
    if (++currentNestingLevel > maxNestingLevel) {
	Error(errNestingTooDeep);
	AbortTranslation(abortNestingTooDeep);
    }

    SetCurrentSymtab(new TSymtab);
}

//--------------------------------------------------------------
//  ExitScope	Exit the current scope and return to the
//		enclosing scope.  Decrement the nesting level.
//		Pop the closed scope's symbol table off the
//		stack and return a pointer to it.
//
//  Return: ptr to closed scope's symbol table
//--------------------------------------------------------------

TSymtab *TSymtabStack::ExitScope(void)
{
    return pSymtabs[currentNestingLevel--];
}
//endfig

//              **********************
//              *                    *
//              *  Line Number List  *
//              *                    *
//              **********************

//--------------------------------------------------------------
//  Destructor      Deallocate a line number list.
//--------------------------------------------------------------

TLineNumList::~TLineNumList(void)
{
    //--Loop to delete each node in the list.
    while (head) {
	TLineNumNode *pNode = head;  // ptr to node to delete
	head = head->next;           // move down the list
	delete pNode;                // delete node
    }
}

//--------------------------------------------------------------
//  Update      Update the list by appending a new line number
//              node if the line number isn't already in the
//              list.
//--------------------------------------------------------------

void TLineNumList::Update(void)
{
    //--If the line number is already there, it'll be at the tail.
    if (tail && (tail->number == currentLineNumber)) return;

    //--Append the new node.
    tail->next = new TLineNumNode;
    tail       = tail->next;
}

//--------------------------------------------------------------
//  Print       Print the line number list.  Use more than one
//              line if necessary; indent subsequent lines.
//
//      newLineFlag : if true, start a new line immediately
//      indent      : amount to indent subsequent lines
//--------------------------------------------------------------

void TLineNumList::Print(int newLineFlag, int indent) const
{
    const int maxLineNumberPrintWidth =  4;
    const int maxLineNumbersPerLine   = 10;

    int           n;      // count of numbers per line
    TLineNumNode *pNode;  // ptr to line number node
    char         *plt = &list.text[strlen(list.text)];
			  // ptr to where in list text to append

    n = newLineFlag ? 0 : maxLineNumbersPerLine;

    //--Loop over line number nodes in the list.
    for (pNode = head; pNode; pNode = pNode->next) {

	//--Start a new list line if the current one is full.
	if (n == 0) {
	    list.PutLine();
	    sprintf(list.text, "%*s", indent, " ");
	    plt = &list.text[indent];
	    n   = maxLineNumbersPerLine;
	}

	//--Append the line number to the list text.
	sprintf(plt, "%*d", maxLineNumberPrintWidth, pNode->number);
	plt += maxLineNumberPrintWidth;
	--n;
    }

    list.PutLine();
}


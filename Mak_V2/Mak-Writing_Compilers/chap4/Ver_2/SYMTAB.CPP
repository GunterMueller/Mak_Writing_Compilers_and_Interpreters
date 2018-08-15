//fig 4-4
//  *************************************************************
//  *                                                           *
//  *   S Y M B O L   T A B L E                                 *
//  *                                                           *
//  *   Manage a symbol table.                                  *
//  *                                                           *
//  *   CLASSES: TSymtabNode,  TSymtab                          *
//  *            TLineNumNode, TLineNumList                     *
//  *                                                           *
//  *   FILE:    prog4-1/symtab.cpp                             *
//  *                                                           *
//  *   MODULE:  Symbol table                                   *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <iostream.h>
#include "buffer.h"
#include "symtab.h"

int xrefFlag = false;  // true = cross-referencing on, false = off

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
//--------------------------------------------------------------

TSymtabNode::TSymtabNode(const char *pStr)
{
    left = right = NULL;
    pLineNumList = NULL;
    value = 0.0;
    xNode = 0;

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
    //--First the subtrees (if any).
    delete left;
    delete right;

    //--Then delete this node's components.
    delete[] pString;
    delete   pLineNumList;
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

    //--First, print left subtree.
    if (left) left->Print();

    //--Print the node:  first the name, then the list of line numbers.
    sprintf(list.text, "%*s", maxNamePrintWidth, pString);
    if (pLineNumList) {
	pLineNumList->Print(strlen(pString) > maxNamePrintWidth,
			    maxNamePrintWidth);
    }
    else list.PutLine();

    //--Finally, print right subtree.
    if (right) right->Print();
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
//
//  Return: ptr to the node, whether existing or newly-entered
//--------------------------------------------------------------

TSymtabNode *TSymtab::Enter(const char *pString)
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
    pNode          = new TSymtabNode(pString);  // create a new node,
    pNode->xSymtab = xSymtab;                   // set its symtab and
    pNode->xNode   = cntNodes++;                // node indexes,
    *ppNode        = pNode;                     // insert it, and
    return pNode;                               // return a ptr to it
}

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
//endfig

//fig 4-11
//  *************************************************************
//  *                                                           *
//  *   I N T E R M E D I A T E   C O D E                       *
//  *                                                           *
//  *   Create and access the intermediate code implemented as  *
//  *   a file.                                                 *
//  *                                                           *
//  *   CLASSES: TIcode                                         *
//  *                                                           *
//  *   FILE:    prog4-2/iform.cpp                              *
//  *                                                           *
//  *   MODULE:  Intermediate code                              *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "symtab.h"
#include "icode.h"

//--Vector of special symbol and reserved word strings.
char *symbolStrings[] = {
    NULL,
    NULL, NULL, NULL, NULL, NULL,

    "^", "*", "(", ")", "-", "+",
    "=", "[", "]", ":", ";", "<",
    ">", ",", ".", "/", ":=", "<=", ">=",
    "<>", "..",

    "and", "array", "begin", "case", "const", "div",
    "do", "downto", "else", "end", "file", "for", "function",
    "goto", "if", "in", "label", "mod", "nil", "not", "of", "or",
    "packed", "procedure", "program", "record", "repeat", "set",
    "then", "to", "type", "until", "var", "while", "with",
};

//--------------------------------------------------------------
//  Constructor     Open the intermediate code file.
//
//      pIformFileName : ptr to the name of the intermediate
//                       code file
//      mode           : input or output
//--------------------------------------------------------------

TIcode::TIcode(const char *pIformFileName, TMode mode)
    : pFileName(pIformFileName)
{
    if (mode == input) {
	//--Open the intermediate code file for input.
	//--Abort if failed.
	file.open(pIformFileName, ios::in | ios::binary);
	if (!file.good()) AbortTranslation(abortIFormFileOpenFailed);
    }
    else {
	//--Open the intermediate code file for output.
	//--Abort if failed.
	file.open(pIformFileName, ios::out | ios::binary);
	if (!file.good()) AbortTranslation(abortIFormFileOpenFailed);

	size = 0;
    }

    symtabStrings = NULL;
}

//--------------------------------------------------------------
//  Destructor      Deallocate the intermediate code.
//--------------------------------------------------------------

TIcode::~TIcode(void)
{
    //--Dellocate the symtab strings vector.
    if (symtabStrings) {
	for (int i = 0; i < symtabCount; ++i) delete[] symtabStrings[i];
	delete[] symtabStrings;
    }

    //--Close the intermediate code file.
    file.close();
}

//--------------------------------------------------------------
//  Put(int)                    Append an integer value to the
//                              intermediate code.
//
//      value : the integer value
//--------------------------------------------------------------

void TIcode::Put(int value)
{
    file.write((const signed char *) &value, sizeof(int));
    size += sizeof(int);
}

//--------------------------------------------------------------
//  Put(TTokenCode, short)      Append a token to the
//                              intermediate code.
//
//      tc    : token code
//      index : -1, or the symbol table node index
//--------------------------------------------------------------

void TIcode::Put(TTokenCode tc, short index)
{
    //--First append the token code.
    char code = tc;
    file.write((const signed char *) &code, sizeof(char));
    size += sizeof(char);

    //--Append the symbol table node index if it's not -1.
    if (index != -1) {
	file.write((const signed char *) &index, sizeof(short));
	size += sizeof(short);
    }
}

//--------------------------------------------------------------
//  Put(TSymtabNode *)          Recursively append a symbol
//                              table's nodes to the
//                              intermediate code.
//
//      pNode : ptr to the root of a symbol table subtree
//--------------------------------------------------------------

void TIcode::Put(const TSymtabNode *pNode)
{
    if (!pNode) return;

    //--First, crunch the left subtree.
    Put(pNode->LeftSubtree());

    //--Then, crunch the subtree root.
    char  length = strlen(pNode->String()) + 1;
    short index  = pNode->NodeIndex();
    file.write((const signed char *) &index,          sizeof(short));
    file.write((const signed char *) &length,         sizeof(char));
    file.write((const signed char *) pNode->String(), length);
    size += sizeof(short) + sizeof(char) + length;

    //--Finally, crunch the right subtree.
    Put(pNode->RightSubtree());
}

//--------------------------------------------------------------
//  Get         Extract the next token from the intermediate
//              code.
//
//  Return: pointer to the extracted token
//--------------------------------------------------------------

TToken *TIcode::Get(void)
{
    TToken *pToken;        // ptr to token to return
    char    code;          // token code read from the file
    TTokenCode token;

    //--First extract the token code.
    file.read((signed char *) &code, sizeof(char));
    token = (TTokenCode) code;

    //--Determine the token class, based on the token code.
    switch (token) {
	case tcNumber:  pToken = &numberToken;  break;
	case tcString:  pToken = &stringToken;  break;

	case tcIdentifier: {
	    pToken       = &wordToken;
	    pToken->code = tcIdentifier;
	    break;
	}

	default: {
	    if (token < tcAND) {
		pToken       = &specialToken;
		pToken->code = token;
	    }
	    else {
		pToken       = &wordToken;  // reserved word
		pToken->code = token;
	    }
	    break;
	}
    }

    //--Extract and set the token string.
    switch (token) {
	case tcIdentifier:
	case tcNumber:
	case tcString: {
	    short index;  // symbol table node index

	    file.read((signed char *) &index, sizeof(short));
	    strcpy(pToken->string, symtabStrings[index]);
	    break;
	}

	default: {
	    strcpy(pToken->string, symbolStrings[code]);
	    break;
	}
    }

    return pToken;
}

//--------------------------------------------------------------
//  GetInteger          Extract an integer value from the
//                      intermediate code.
//
//  Return: the integer value
//--------------------------------------------------------------

int TIcode::GetInteger(void)
{
    int value;  // value to extract

    file.read((signed char *) &value, sizeof(int));
    return value;
}

//--------------------------------------------------------------
//  GetSymtabStrings    Extract the vector of symbol table
//                      table strings from the intermediate
//                      code.
//
//  Return: pointer to the vector
//--------------------------------------------------------------

void TIcode::GetSymtabStrings(void)
{
    int   count;   // count of symtab strings
    short index;   // symbol table node index
    char  length;  // string length (including final '\0')

    //--First extract the count of strings and allocate
    //--the vector of string pointers.
    file.read((signed char *) &count, sizeof(int));
    symtabStrings = new char *[count];
    symtabCount   = count;

    //--Then extract the strings.
    do {
	file.read((signed char *) &index,  sizeof(short));
	file.read((signed char *) &length, sizeof(char));

	symtabStrings[index] = new char[length];
	file.read((signed char *) symtabStrings[index], length);
    } while (--count > 0);
}
//endfig




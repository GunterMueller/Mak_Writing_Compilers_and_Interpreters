//  *************************************************************
//  *                                                           *
//  *   I N T E R M E D I A T E   C O D E                       *
//  *                                                           *
//  *   Create and access the intermediate code implemented in  *
//  *   memory.                                                 *
//  *                                                           *
//  *   CLASSES: icode                                          *
//  *                                                           *
//  *   FILE:    prog10-1/icode.cpp                             *
//  *                                                           *
//  *   MODULE:  Intermediate code                              *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <memory.h>
#include "common.h"
#include "error.h"
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
//  Copy constructor    Make a copy of the icode.  Only copy as
//                      many bytes of icode as necessary.
//
//      icode : ref to source icode
//
//--------------------------------------------------------------

TIcode::TIcode(const TIcode &icode)
{
    int length = int(icode.cursor - icode.pCode);  // length of icode

    //--Copy icode.
    pCode = cursor = new char[length];
    memcpy(pCode, icode.pCode, length);
}

//--------------------------------------------------------------
//  CheckBounds         Guard against code segment overflow.
//
//      size : number of bytes to append
//--------------------------------------------------------------

void TIcode::CheckBounds(int size)
{
    if (cursor + size >= &pCode[codeSegmentSize]) {
	Error(errCodeSegmentOverflow);
	AbortTranslation(abortCodeSegmentOverflow);
    }
}

//--------------------------------------------------------------
//  Put(TTokenCode)     Append a token code to the intermediate
//                      code.
//
//      tc    : token code
//      pNode : ptr to symtab node, or NULL
//--------------------------------------------------------------

void TIcode::Put(TTokenCode tc)
{
    if (errorCount > 0) return;

    char code = tc;
    CheckBounds(sizeof(char));
    memcpy((void *) cursor, (const void *) &code, sizeof(char));
    cursor += sizeof(char);
}

//--------------------------------------------------------------
//  Put(TSymtabNode *)      Append a symbol table node's symbol
//                          table and node indexes to the
//                          intermediate code.
//
//      pNode : ptr to symtab node
//--------------------------------------------------------------

void TIcode::Put(const TSymtabNode *pNode)
{
    if (errorCount > 0) return;

    short xSymtab = pNode->SymtabIndex();
    short xNode   = pNode->NodeIndex();

    CheckBounds(2*sizeof(short));
    memcpy((void *) cursor,
	   (const void *) &xSymtab, sizeof(short));
    memcpy((void *) (cursor + sizeof(short)),
	   (const void *) &xNode,   sizeof(short));
    cursor += 2*sizeof(short);
}

//fig 10-7
//--------------------------------------------------------------
//  Get                 Extract the next token from the
//                      intermediate code.
//
//  Return: ptr to the extracted token
//--------------------------------------------------------------

TToken *TIcode::Get(void)
{
    TToken *pToken;        // ptr to token to return
    char    code;          // token code read from the file
    TTokenCode token;

    //--Loop to process any line markers
    //--and extract the next token code.
    do {
	//--First read the token code.
	memcpy((void *) &code, (const void *) cursor, sizeof(char));
	cursor += sizeof(char);
	token = (TTokenCode) code;

	//--If it's a line marker, extract the line number.
	if (token == mcLineMarker) {
	    short number;
	    
	    memcpy((void *) &number, (const void *) cursor,
		   sizeof(short));
	    currentLineNumber = number;
	    cursor += sizeof(short);
	}
    } while (token == mcLineMarker);

    //--Determine the token class, based on the token code.
    switch (token) {
	case tcNumber:  pToken = &numberToken;  break;
	case tcString:  pToken = &stringToken;  break;

	case tcIdentifier:
	    pToken       = &wordToken;
	    pToken->code = tcIdentifier;
	    break;

	case mcLocationMarker:
	    pToken       = &specialToken;
	    pToken->code = token;
	    break;

	default:
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

    //--Extract the symbol table node and set the token string.
    switch (token) {
	case tcIdentifier:
	case tcNumber:
	case tcString:
	    pNode = GetSymtabNode();
	    strcpy(pToken->string, pNode->String());
	    break;

	case mcLocationMarker:
	    pNode = NULL;
	    pToken->string[0] = '\0';
	    break;

	default:
	    pNode = NULL;
	    strcpy(pToken->string, symbolStrings[code]);
	    break;
    }

    return pToken;
}
//endfig

//--------------------------------------------------------------
//  GetSymtabNode       Extract a symbol table node pointer
//                      from the intermediate code.
//
//  Return: ptr to the symbol table node
//--------------------------------------------------------------

TSymtabNode *TIcode::GetSymtabNode(void)
{
    extern TSymtab **vpSymtabs;
    short xSymtab, xNode;         // symbol table and node indexes

    memcpy((void *) &xSymtab, (const void *) cursor,
	   sizeof(short));
    memcpy((void *) &xNode,   (const void *) (cursor + sizeof(short)),
	   sizeof(short));
    cursor += 2*sizeof(short);

    return vpSymtabs[xSymtab]->Get(xNode);
}

//--------------------------------------------------------------
//  InsertLineMarker    Insert a line marker into the
//                      intermediate code just before the
//                      last appended token code.
//--------------------------------------------------------------

void TIcode::InsertLineMarker(void)
{
    if (errorCount > 0) return;

    //--Remember the last appended token code;
    char lastCode;
    cursor -= sizeof(char);
    memcpy((void *) &lastCode, (const void *) cursor, sizeof(char));

    //--Insert a statement marker code
    //--followed by the current line number.
    char  code   = mcLineMarker;
    short number = currentLineNumber;
    CheckBounds(sizeof(char) + sizeof(short));
    memcpy((void *) cursor, (const void *) &code, sizeof(char));
    cursor += sizeof(char);
    memcpy((void *) cursor, (const void *) &number, sizeof(short));
    cursor += sizeof(short);

    //--Re-append the last token code;
    memcpy((void *) cursor, (const void *) &lastCode, sizeof(char));
    cursor += sizeof(char);
}

//fig 10-6
//--------------------------------------------------------------
//  PutLocationMarker   Append a location marker to the
//                      intermediate code.
//
//      location : location to mark
//
//  Return: location of the location marker's offset
//--------------------------------------------------------------

int TIcode::PutLocationMarker(void)
{
    if (errorCount > 0) return 0;

    //--Append the location marker code.
    char code = mcLocationMarker;
    CheckBounds(sizeof(char));
    memcpy((void *) cursor, (const void *) &code, sizeof(char));
    cursor += sizeof(char);

    //--Append 0 as a placeholder for the location offset.
    //--Remember the current location of the offset itself.
    short offset     = 0;
    int   atLocation = CurrentLocation();
    CheckBounds(sizeof(short));
    memcpy((void *) cursor, (const void *) &offset, sizeof(short));
    cursor += sizeof(short);

    return atLocation;
}

//--------------------------------------------------------------
//  FixupLocationMarker     Fixup a location marker in the
//                          intermediate code by patching in the
//                          offset of the current token's
//                          location.
//
//      location : location of the offset to fix up
//--------------------------------------------------------------

void TIcode::FixupLocationMarker(int location)
{
    //--Patch in the offset of the current token's location.
    short offset = CurrentLocation() - 1;
    memcpy((void *) (pCode + location), (const void *) &offset,
	   sizeof(short));
}

//--------------------------------------------------------------
//  GetLocationMarker       Extract a location marker from the
//                          intermediate code.
//
//  Return: location offset
//--------------------------------------------------------------

int TIcode::GetLocationMarker(void)
{
    short offset;  // location offset

    //--Extract the offset from the location marker.
    memcpy((void *) &offset, (const void *) cursor, sizeof(short));
    cursor += sizeof(short);

    return int(offset);
}
//endfig

//fig 10-10
//--------------------------------------------------------------
//  PutCaseItem         Append a CASE item to the intermediate
//                      code.
//
//      value   : CASE label value
//      location: location of CASE branch statement
//--------------------------------------------------------------

void TIcode::PutCaseItem(int value, int location)
{
    if (errorCount > 0) return;

    short offset = location & 0xffff;

    CheckBounds(sizeof(int) + sizeof(short));
    memcpy((void *) cursor, (const void *) &value, sizeof(int));
    cursor += sizeof(int);
    memcpy((void *) cursor, (const void *) &offset, sizeof(short));
    cursor += sizeof(short);
}

//--------------------------------------------------------------
//  GetCaseItem         Extract a CASE item from the
//                      intermediate code.
//
//      value   : ref to CASE label value
//      location: ref to location of CASE branch statement
//--------------------------------------------------------------

void TIcode::GetCaseItem(int &value, int &location)
{
    int   val;
    short offset;

    memcpy((void *) &val, (const void *) cursor, sizeof(int));
    cursor += sizeof(int);
    memcpy((void *) &offset, (const void *) cursor, sizeof(short));
    cursor += sizeof(short);

    value    = val;
    location = offset;
}
//endfig


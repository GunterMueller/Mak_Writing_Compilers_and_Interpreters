//fig 4-14
//  *************************************************************
//  *                                                           *
//  *   Program 4-3:  Source File Decruncher                    *
//  *                                                           *
//  *   Decrunch a Pascal source file from its intermediate     *
//  *   code to a compacted text form.                          *
//  *                                                           *
//  *   FILE:   prog4-3/decrunch.cpp                            *
//  *                                                           *
//  *   USAGE:  decrunch <icode file>                           *
//  *                                                           *
//  *               <icode file>  name of the intermediate      *
//  *                             code file                     *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream.h>
#include "common.h"
#include "error.h"
#include "complist.h"
#include "token.h"
#include "icode.h"

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

void main(int argc, char *argv[])
{
    TToken *pToken;                  // ptr to the current token
    int     currIsDelimiter;         // true if current token is a
				     //   delimiter, else false
    int     prevIsDelimiter = true;  // likewise for previous token

    //--Check the command line arguments.
    if (argc != 2) {
	cerr << "Usage: decrunch <icode file>" << endl;
	AbortTranslation(abortInvalidCommandLineArgs);
    }

    //--Create the icode and compact list objects.
    TIcode             icode(argv[1], TIcode::input);
    TCompactListBuffer compact;

    //--Read the location of the crunched symbol table strings,
    //--and then extract the strings.
    int atSymtab = icode.GetInteger();
    icode.GoTo(atSymtab);
    icode.GetSymtabStrings();

    //--Get ready to read the crunched program.
    icode.GoTo(sizeof(int));

    //--Loop to extract the crunched tokens
    //--until the end of the program.
    do {
	//--Extract the next token.
	pToken          = icode.Get();
	currIsDelimiter = pToken->IsDelimiter();

	//--Append a blank only if both the previous and the
	//--current tokens are not delimiters.  Then append the
	//--token string to the output record.
	if (!prevIsDelimiter && !currIsDelimiter) {
	    compact.PutBlank();
	}
	compact.Put(pToken->String());

	prevIsDelimiter = currIsDelimiter;
    } while (pToken->Code() != tcPeriod);

    //--Print the last output record.
    compact.PutLine();
}
//endfig


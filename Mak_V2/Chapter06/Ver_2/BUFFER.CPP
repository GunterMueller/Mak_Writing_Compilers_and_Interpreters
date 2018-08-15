//fig 2-4
//  *************************************************************
//  *                                                           *
//  *   I / O   B U F F E R S                                   *
//  *                                                           *
//  *   Process text I/O files.  Included are member functions  *
//  *   to read the source file and write to the list file.     *
//  *                                                           *
//  *   CLASSES: TTextInBuffer,  TSourceBuffer                  *
//  *            TTextOutBuffer, TListBuffer                    *
//  *                                                           *
//  *   FILE:    prog2-1/buffer.cpp                             *
//  *                                                           *
//  *   MODULE:  Buffer                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <time.h>
#include "common.h"
#include "buffer.h"

//              ***********************
//              *                     *
//              *  Text Input Buffer  *
//              *                     *
//              ***********************

char eofChar = 0x7F;   // special end-of-file character
int  inputPosition;    // "virtual" position of the current char
		       //   in the input buffer (with tabs expanded)
int  listFlag = true;  // true if list source lines, else false

//--------------------------------------------------------------
//  Constructor     Construct a input text buffer by opening the
//                  input file.
//
//      pInputFileName : ptr to the name of the input file
//      ac             : abort code to use if open failed
//--------------------------------------------------------------

TTextInBuffer::TTextInBuffer(const char *pInputFileName, TAbortCode ac)
    : pFileName(new char[strlen(pInputFileName) + 1])
{
    //--Copy the input file name.
    strcpy(pFileName, pInputFileName);

    //--Open the input file.  Abort if failed.
    file.open(pFileName, ios::in|ios::nocreate);
    if (!file.good()) AbortTranslation(ac);
}

//--------------------------------------------------------------
//  GetChar         Fetch and return the next character from the
//                  text buffer.  If at the end of the buffer,
//                  read the next source line.  If at the end of
//                  the file, return the end-of-file character.
//
//  Return: next character from the source file
//          or the end-of-file character
//--------------------------------------------------------------

char TTextInBuffer::GetChar(void)
{
    const int tabSize = 8;  // size of tabs
    char ch;                // character to return

    if      (*pChar == eofChar) return eofChar;  // end of file
    else if (*pChar == '\0')    ch = GetLine();  // end of line
    else {                                       // next char
	++pChar;
	++inputPosition;
	ch = *pChar;
    }

    //--If tab character, increment inputPosition to the next
    //--multiple of tabSize.
    if (ch == '\t') inputPosition += tabSize - inputPosition%tabSize;

    return ch;
}

//--------------------------------------------------------------
//  PutBackChar     Put the current character back into the
//                  input buffer so that the next call to
//                  GetChar will fetch this character. (Only
//                  called to put back a '.')
//
//  Return: the previous character
//--------------------------------------------------------------

char TTextInBuffer::PutBackChar(void)
{
    --pChar;
    --inputPosition;

    return *pChar;
}

//              *******************
//              *                 *
//              *  Source Buffer  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  Constructor     Construct a source buffer by opening the
//                  source file.  Initialize the list file, and
//                  read the first line from the source file.
//
//      pSourceFileName : ptr to name of source file
//--------------------------------------------------------------

TSourceBuffer::TSourceBuffer(const char *pSourceFileName)
    : TTextInBuffer(pSourceFileName, abortSourceFileOpenFailed)
{
    //--Initialize the list file and read the first source line.
    if (listFlag) list.Initialize(pSourceFileName);
    GetLine();
}

//--------------------------------------------------------------
//  GetLine         Read the next line from the source file, and
//                  print it to the list file preceded by the
//                  line number and the current nesting level.
//
//  Return: first character of the source line, or the
//          end-of-file character if at the end of the file
//--------------------------------------------------------------

char TSourceBuffer::GetLine(void)
{
    extern int lineNumber, currentNestingLevel;

    //--If at the end of the source file, return the end-of-file char.
    if (file.eof()) pChar = &eofChar;

    //--Else read the next source line and print it to the list file.
    else {
	file.getline(text, maxInputBufferSize);
	pChar = text;   // point to first source line char

	if (listFlag) list.PutLine(text, ++currentLineNumber,
				   currentNestingLevel);
    }

    inputPosition = 0;
    return *pChar;
}

//              *****************
//              *               *
//              *  List Buffer  *
//              *               *
//              *****************

const int maxPrintLineLength = 80;
const int maxLinesPerPage    = 50;

TListBuffer list;   // the list file buffer

//--------------------------------------------------------------
//  PrintPageHeader     Start a new page of the list file and
//                      print the page header.
//--------------------------------------------------------------

void TListBuffer::PrintPageHeader(void)
{
    const char formFeedChar = '\f';

    cout << formFeedChar << "Page "  << ++pageNumber
	 << "   " << pSourceFileName << "   " << date
	 << endl  << endl;

    lineCount = 0;
}

//--------------------------------------------------------------
//  Initialize      Initialize the list buffer.  Set the date
//                  for the page header, and print the first
//                  header.
//
//      pFileName : ptr to source file name (for page header)
//--------------------------------------------------------------

void TListBuffer::Initialize(const char *pFileName)
{
    text[0]    = '\0';
    pageNumber = 0;

    //--Copy the input file name.
    pSourceFileName = new char[strlen(pFileName) + 1];
    strcpy(pSourceFileName, pFileName);

    //--Set the date.
    time_t timer;
    time(&timer);
    strcpy(date, asctime(localtime(&timer)));
    date[strlen(date) - 1] = '\0';  // remove '\n' at end

    PrintPageHeader();
}

//--------------------------------------------------------------
//  PutLine         Print a line of text to the list file.
//--------------------------------------------------------------

void TListBuffer::PutLine(void)
{
    //--Start a new page if the current one is full.
    if (listFlag && (lineCount == maxLinesPerPage)) PrintPageHeader();

    //--Truncate the line if it's too long.
    text[maxPrintLineLength] = '\0';

    //--Print the text line, and then blank out the text.
    cout << text << endl;
    text[0] = '\0';

    ++lineCount;
}
//endfig


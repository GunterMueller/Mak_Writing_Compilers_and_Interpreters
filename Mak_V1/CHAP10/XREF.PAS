PROGRAM xref (input, output);

    {Generate a cross-reference listing from a text file.}

CONST
    maxwordlen       =   20;
    wordtablesize    =  500;
    numbertablesize  = 1000;
    maxlinenumber    =  999;

TYPE
    charindex        = 1..maxwordlen;
    wordtableindex   = 1..wordtablesize;
    numbertableindex = 0..numbertablesize;
    linenumbertype   = 1..maxlinenumber;

    wordtype         = ARRAY [charindex] OF char;  {string type}

    wordentrytype    = RECORD  {entry in word table}
			   word : wordtype;	{word string}
			   firstnumberindex,    {head and tail of    }
			   lastnumberindex      {  linked number list}
			       : numbertableindex;
		       END;

    numberentrytype  = RECORD  {entry in number table}
			   number                   {line number}
			       : linenumbertype;
			   nextindex                {index of next   }
			       : numbertableindex;  {  in linked list}
		       END;

    wordtabletype    = ARRAY [wordtableindex]   OF wordentrytype;
    numbertabletype  = ARRAY [numbertableindex] OF numberentrytype;

VAR
    wordtable                      : wordtabletype;
    numbertable                    : numbertabletype;
    nextwordindex   		   : wordtableindex;
    nextnumberindex                : numbertableindex;
    linenumber                     : linenumbertype;
    wordtablefull, numbertablefull : boolean;
    newline, gotword               : boolean;


FUNCTION nextchar : char;

    {Fetch and echo the next character.
     Print the line number before each new line.}

    CONST
	blank = ' ';

    VAR
	ch : char;

    BEGIN
	newline := eoln;
	IF newline THEN BEGIN
	    readln;
	    writeln;
	    linenumber := linenumber + 1;
	    write(linenumber:5, ' : ');
	END;
	IF newline OR eof THEN BEGIN
	    ch := blank;
	END
	ELSE BEGIN
	    read(ch);
	    write(ch);
	END;
	nextchar := ch;
    END;


FUNCTION isletter (ch : char) : boolean;

    {Return true if the character is a letter, false otherwise.}

    BEGIN
	isletter :=    ((ch >= 'a') AND (ch <= 'z'))
		    OR ((ch >= 'A') AND (ch <= 'Z'));
    END;


PROCEDURE readword (VAR buffer : wordtype);

    {Extract the next word and place it into the buffer.}

    CONST
	blank = ' ';

    VAR
	charcount : 0..maxwordlen;
	ch : char;

    BEGIN
	gotword := false;

	{Skip over any preceding non-letters.}
	IF NOT eof THEN BEGIN
	    REPEAT
		ch := nextchar;
	    UNTIL eof OR isletter(ch);
	END;

	{Find a letter?}
	IF NOT eof THEN BEGIN
	    charcount := 0;

	    {Place the word's letters into the buffer.
	     Downshift uppercase letters.}
	    WHILE isletter(ch) DO BEGIN
		IF charcount < maxwordlen THEN BEGIN
		    IF (ch >= 'A') AND (ch <= 'Z') THEN BEGIN
			ch := chr(ord(ch) + (ord('a') - ord('A')));
		    END;
		    charcount := charcount + 1;
		    buffer[charcount] := ch;
		END;
		ch := nextchar;
	    END;

	    {Pad the rest of the buffer with blanks.}
	    FOR charcount := charcount + 1 TO maxwordlen DO BEGIN
		buffer[charcount] := blank;
	    END;

	    gotword := true;
	END;
    END;


FUNCTION appendlinenumber(lastnumberindex : numbertableindex)
	     : numbertableindex;

    {Append the current line number to the end of the current word's
     linked list.  Lastnumberindex is 0 if this is the word's first
     number; else, it is the index of the last number in the list.}

    BEGIN
        IF nextnumberindex < numbertablesize THEN BEGIN
	    IF lastnumberindex <> 0 THEN BEGIN
	        numbertable[lastnumberindex].nextindex := nextnumberindex;
	    END;
	    numbertable[nextnumberindex].number    := linenumber;
	    numbertable[nextnumberindex].nextindex := 0;
	    appendlinenumber := nextnumberindex;
	    nextnumberindex  := nextnumberindex + 1;
        END
        ELSE BEGIN
            numbertablefull  := true;
            appendlinenumber := 0;
        END;
    END;


PROCEDURE enterword;

    {Enter the current word into the word table.  Each word is first
     read into the end of the table.}

    VAR
	i : wordtableindex;

    BEGIN
	{By the time we process a word at the end of an input line,
	 linenumber has already been incremented, so temporarily
	 decrement it.}
	IF newline THEN linenumber := linenumber - 1;

	{Search to see if the word has previously been entered.}
	i := 1;
	WHILE    wordtable[i].word
	      <> wordtable[nextwordindex].word DO BEGIN
	    i := i + 1;
	END;

	{Yes.  Update the previous entry.}
	IF i < nextwordindex THEN BEGIN
	    wordtable[i].lastnumberindex :=
		appendlinenumber(wordtable[i].lastnumberindex);
	END

	{No.  Initialize the entry at the end of the table.}
	ELSE IF nextwordindex < wordtablesize THEN BEGIN
	    nextwordindex := nextwordindex + 1;
	    wordtable[i].firstnumberindex := appendlinenumber(0);
	    wordtable[i].lastnumberindex :=
		wordtable[i].firstnumberindex;
	END

	{Oops.  Table overflow!}
	ELSE wordtablefull := true;

	IF newline THEN linenumber := linenumber + 1;
    END;


PROCEDURE sortwords;

    {Sort the words alphabetically.}

    VAR
	i, j : wordtableindex;
	temp : wordentrytype;

    BEGIN
	FOR i := 1 TO nextwordindex - 2 DO BEGIN
	    FOR j := i + 1 TO nextwordindex - 1 DO BEGIN
		IF wordtable[i].word > wordtable[j].word THEN BEGIN
		    temp := wordtable[i];
		    wordtable[i] := wordtable[j];
		    wordtable[j] := temp;
		END;
	    END;
	END;
    END;


PROCEDURE printnumbers (i : numbertableindex);

    {Print a word's linked list of line numbers.}

    BEGIN
	REPEAT
	    write(numbertable[i].number:4);
	    i := numbertable[i].nextindex;
	UNTIL i = 0;
	writeln;
    END;


PROCEDURE printxref;

    {Print the cross reference listing.}

    VAR
	i : wordtableindex;

    BEGIN
	writeln;
	writeln;
	writeln('Cross-reference');
	writeln('---------------');
	writeln;
	sortwords;
	FOR i := 1 TO nextwordindex - 1 DO BEGIN
	    write(wordtable[i].word);
	    printnumbers(wordtable[i].firstnumberindex);
	END;
    END;


BEGIN {xref}
    wordtablefull   := false;
    numbertablefull := false;
    nextwordindex   := 1;
    nextnumberindex := 1;
    linenumber      := 1;
    write('    1 : ');

    {First read the words.}
    WHILE NOT (eof OR wordtablefull OR numbertablefull) DO BEGIN
	readword(wordtable[nextwordindex].word);
	IF gotword THEN enterword;
    END;

    {Then print the cross reference listing if all went well.}
    IF wordtablefull THEN BEGIN
        writeln;
	writeln('*** The word table is not large enough. ***');
    END
    ELSE IF numbertablefull THEN BEGIN
        writeln;
	writeln('*** The number table is not large enough. ***');
    END
    ELSE BEGIN
	printxref;
    END;

    {Print final stats.}
    writeln;
    writeln((nextwordindex - 1):5,   ' word entries.');
    writeln((nextnumberindex - 1):5, ' line number entries.');
END {xref}.


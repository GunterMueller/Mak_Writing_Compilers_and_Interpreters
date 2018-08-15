PROGRAM Xref (input, output);

    {Generate a cross-reference listing from a text file.}

CONST
    MaxWordLength    =   20;
    WordTableSize    =  500;
    NumberTableSize  = 2000;
    MaxLineNumber    =  999;

TYPE
    charIndexRange   = 1..MaxWordLength;
    wordIndexRange   = 1..WordTableSize;
    numberIndexRange = 0..NumberTableSize;
    lineNumberRange  = 1..MaxLineNumber;

    wordType         = ARRAY [charIndexRange] OF char;  {string type}

    wordEntryType    = RECORD  {entry in word table}
                           word : wordType;     {word string}
                           firstNumberIndex,    {head and tail of    }
                           lastNumberIndex      {  linked number list}
                               : numberIndexRange;
                       END;

    numberEntryType  = RECORD  {entry in number table}
                           number : lineNumberRange; {line number}
                           nextIndex                 {index of next   }
                               : numberIndexRange;   {  in linked list}
                       END;

    wordTableType    = ARRAY [wordIndexRange]   OF wordEntryType;
    numberTableType  = ARRAY [numberIndexRange] OF numberEntryType;

VAR
    wordTable                      : wordTableType;
    numberTable                    : numberTableType;
    nextWordIndex                  : wordIndexRange;
    nextNumberIndex                : numberIndexRange;
    lineNumber                     : lineNumberRange;
    wordTableFull, numberTableFull : boolean;
    newLine                        : boolean;


FUNCTION NextChar : char;

    {Fetch and echo the next character.
     Print the line number before each new line.}

    CONST
        blank = ' ';

    VAR
        ch : char;

    BEGIN
        newLine := eoln;
        IF newLine THEN BEGIN
            readln;
            writeln;
            lineNumber := lineNumber + 1;
            write(lineNumber:5, ' : ');
        END;
        IF newLine OR eof THEN BEGIN
            ch := blank;
        END
        ELSE BEGIN
            read(ch);
            write(ch);
        END;
        NextChar := ch;
    END;


FUNCTION IsLetter(ch : char) : boolean;

    {Return true if the character is a letter, false otherwise.}

    BEGIN
        IsLetter :=    ((ch >= 'a') AND (ch <= 'z'))
                    OR ((ch >= 'A') AND (ch <= 'Z'));
    END;


FUNCTION ReadWord(VAR buffer : wordType) : boolean;

    {Extract the next word and place it into the buffer.}

    CONST
        blank = ' ';

    VAR
        charcount : integer;
        ch : char;

    BEGIN
        ReadWord := false;

        {Skip over any preceding non-letters.}
        IF NOT eof THEN BEGIN
            REPEAT
                ch := NextChar;
            UNTIL eof OR IsLetter(ch);
        END;

        {Find a letter?}
        IF NOT eof THEN BEGIN
            charcount := 0;

            {Place the word's letters into the buffer.
             Downshift uppercase letters.}
            WHILE IsLetter(ch) DO BEGIN
                IF charcount < MaxWordLength THEN BEGIN
                    IF (ch >= 'A') AND (ch <= 'Z') THEN BEGIN
                        ch := chr(ord(ch) + (ord('a') - ord('A')));
                    END;
                    charcount := charcount + 1;
                    buffer[charcount] := ch;
                END;
                ch := NextChar;
            END;

            {Pad the rest of the buffer with blanks.}
            FOR charcount := charcount + 1 TO MaxWordLength DO BEGIN
                buffer[charcount] := blank;
            END;

            ReadWord := true;
        END;
    END;


PROCEDURE AppendLineNumber(VAR entry : wordEntryType);

    {Append the current line number to the end of the current word entry's
     linked list of numbers.}

    BEGIN
        IF nextNumberIndex < NumberTableSize THEN BEGIN

            {entry.lastnumberindex is 0 if this is the word's first number.
             Otherwise, it is the number table index of the last number
             in the list, which we now extend for the new number.}
            IF entry.lastNumberIndex = 0 THEN BEGIN
                entry.firstNumberIndex := nextNumberIndex;
            END
            ELSE BEGIN
                numberTable[entry.lastNumberIndex].nextIndex := nextNumberIndex;
            END;

            {Set the line number at the end of the list.}
            numberTable[nextNumberIndex].number    := lineNumber;
            numberTable[nextNumberIndex].nextIndex := 0;
            entry.lastNumberIndex := nextNumberIndex;
            nextNumberIndex       := nextNumberIndex + 1;
        END
        ELSE BEGIN
            numberTableFull := true;
        END;
    END;


PROCEDURE EnterWord;

    {Enter the current word into the word table. Each word is first
     read into the end of the table.}

    VAR
        i : wordIndexRange;

    BEGIN
        {By the time we process a word at the end of an input line,
         lineNumber has already been incremented, so temporarily
         decrement it.}
        IF newLine THEN lineNumber := lineNumber - 1;

        {Search to see if the word had already been entered previously.
         Each time it's read in, it's placed at the end of the word table.}
        i := 1;
        WHILE wordTable[i].word <> wordTable[nextWordIndex].word DO BEGIN
            i := i + 1;
        END;

        {Entered previously:  Update the existing entry.}
        IF i < nextWordIndex THEN BEGIN
            AppendLineNumber(wordTable[i]);
        END

        {New word:  Initialize the entry at the end of the table.}
        ELSE IF nextWordIndex < WordTableSize THEN BEGIN
            wordTable[i].lastNumberIndex := 0;
            AppendLineNumber(wordTable[i]);
            nextWordIndex := nextWordIndex + 1;
        END

        {Oops.  Table overflow!}
        ELSE wordTableFull := true;

        IF newLine THEN lineNumber := lineNumber + 1;
    END;


PROCEDURE SortWords;

    {Sort the words alphabetically.}

    VAR
        i, j : wordIndexRange;
        temp : wordEntryType;

    BEGIN
        FOR i := 1 TO nextWordIndex - 2 DO BEGIN
            FOR j := i + 1 TO nextWordIndex - 1 DO BEGIN
                IF wordTable[i].word > wordTable[j].word THEN BEGIN
                    temp := wordTable[i];
                    wordTable[i] := wordTable[j];
                    wordTable[j] := temp;
                END;
            END;
        END;
    END;


PROCEDURE PrintNumbers(i : numberIndexRange);

    {Print a word's linked list of line numbers.}

    BEGIN
        REPEAT
            write(numberTable[i].number:4);
            i := numberTable[i].nextIndex;
        UNTIL i = 0;
        writeln;
    END;


PROCEDURE PrintXref;

    {Print the cross reference listing.}

    VAR
        i : wordIndexRange;

    BEGIN
        writeln;
        writeln;
        writeln('Cross-reference');
        writeln('---------------');
        writeln;
        SortWords;
        FOR i := 1 TO nextWordIndex - 1 DO BEGIN
            write(wordTable[i].word);
            PrintNumbers(wordTable[i].firstNumberIndex);
        END;
    END;


BEGIN {Xref}
    wordTableFull   := false;
    numberTableFull := false;
    nextWordIndex   := 1;
    nextNumberIndex := 1;
    lineNumber      := 1;
    write('    1 : ');

    {First read the words.}
    WHILE NOT (eof OR wordTableFull OR numberTableFull) DO BEGIN

        {Read each word into the last entry of the word table
         and then enter it into its correct location.}
        IF ReadWord(wordtable[nextwordIndex].Word) THEN BEGIN
            EnterWord;
        END;
    END;

    {Then print the cross reference listing if all went well.}
    IF wordTableFull THEN BEGIN
        writeln;
        writeln('*** The word table is not large enough. ***');
    END
    ELSE IF numberTableFull THEN BEGIN
        writeln;
        writeln('*** The number table is not large enough. ***');
    END
    ELSE BEGIN
        PrintXref;
    END;

    {Print final stats.}
    writeln;
    writeln((nextWordIndex - 1):5,   ' word entries.');
    writeln((nextNumberIndex - 1):5, ' line number entries.');
END {Xref}.

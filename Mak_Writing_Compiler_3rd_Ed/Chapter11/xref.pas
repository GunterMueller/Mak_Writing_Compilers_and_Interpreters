001 PROGRAM Xref (input, output);
002 
003     {Generate a cross-reference listing from a text file.}
004 
005 CONST
006     MaxWordLength    =   20;
007     WordTableSize    =  500;
008     NumberTableSize  = 2000;
009     MaxLineNumber    =  999;
010 
011 TYPE
012     charIndexRange   = 1..MaxWordLength;
013     wordIndexRange   = 1..WordTableSize;
014     numberIndexRange = 0..NumberTableSize;
015     lineNumberRange  = 1..MaxLineNumber;
016 
017     wordType         = ARRAY [charIndexRange] OF char;  {string type}
018 
019     wordEntryType    = RECORD  {entry in word table}
020                            word : wordType;     {word string}
021                            firstNumberIndex,    {head and tail of    }
022                            lastNumberIndex      {  linked number list}
023                                : numberIndexRange;
024                        END;
025 
026     numberEntryType  = RECORD  {entry in number table}
027                            number : lineNumberRange; {line number}
028                            nextIndex                 {index of next   }
029                                : numberIndexRange;   {  in linked list}
030                        END;
031 
032     wordTableType    = ARRAY [wordIndexRange]   OF wordEntryType;
033     numberTableType  = ARRAY [numberIndexRange] OF numberEntryType;
034 
035 VAR
036     wordTable                      : wordTableType;
037     numberTable                    : numberTableType;
038     nextWordIndex                  : wordIndexRange;
039     nextNumberIndex                : numberIndexRange;
040     lineNumber                     : lineNumberRange;
041     wordTableFull, numberTableFull : boolean;
042     newLine                        : boolean;
043 
044 
045 FUNCTION NextChar : char;
046 
047     {Fetch and echo the next character.
048      Print the line number before each new line.}
049 
050     CONST
051         blank = ' ';
052 
053     VAR
054         ch : char;
055 
056     BEGIN
057         newLine := eoln;
058         IF newLine THEN BEGIN
059             readln;
060             writeln;
061             lineNumber := lineNumber + 1;
062             write(lineNumber:5, ' : ');
063         END;
064         IF newLine OR eof THEN BEGIN
065             ch := blank;
066         END
067         ELSE BEGIN
068             read(ch);
069             write(ch);
070         END;
071         NextChar := ch;
072     END;
073 
074 
075 FUNCTION IsLetter(ch : char) : boolean;
076 
077     {Return true if the character is a letter, false otherwise.}
078 
079     BEGIN
080         IsLetter :=    ((ch >= 'a') AND (ch <= 'z'))
081                     OR ((ch >= 'A') AND (ch <= 'Z'));
082     END;
083 
084 
085 FUNCTION ReadWord(VAR buffer : wordType) : boolean;
086 
087     {Extract the next word and place it into the buffer.}
088 
089     CONST
090         blank = ' ';
091 
092     VAR
093         charcount : 0..MaxWordLength;
094         ch : char;
095 
096     BEGIN
097         ReadWord := false;
098 
099         {Skip over any preceding non-letters.}
100         IF NOT eof THEN BEGIN
101             REPEAT
102                 ch := NextChar;
103             UNTIL eof OR IsLetter(ch);
104         END;
105 
106         {Find a letter?}
107         IF NOT eof THEN BEGIN
108             charcount := 0;
109 
110             {Place the word's letters into the buffer.
111              Downshift uppercase letters.}
112             WHILE IsLetter(ch) DO BEGIN
113                 IF charcount < MaxWordLength THEN BEGIN
114                     IF (ch >= 'A') AND (ch <= 'Z') THEN BEGIN
115                         ch := chr(ord(ch) + (ord('a') - ord('A')));
116                     END;
117                     charcount := charcount + 1;
118                     buffer[charcount] := ch;
119                 END;
120                 ch := NextChar;
121             END;
122 
123             {Pad the rest of the buffer with blanks.}
124             FOR charcount := charcount + 1 TO MaxWordLength DO BEGIN
125                 buffer[charcount] := blank;
126             END;
127 
128             ReadWord := true;
129         END;
130     END;
131 
132 
133 PROCEDURE AppendLineNumber(VAR entry : wordEntryType);
134 
135     {Append the current line number to the end of the current word entry's
136      linked list of numbers.}
137 
138     BEGIN
139         IF nextNumberIndex < NumberTableSize THEN BEGIN
140 
141             {entry.lastnumberindex is 0 if this is the word's first number.
142              Otherwise, it is the number table index of the last number
143              in the list, which we now extend for the new number.}
144             IF entry.lastNumberIndex = 0 THEN BEGIN
145                 entry.firstNumberIndex := nextNumberIndex;
146             END
147             ELSE BEGIN
148                 numberTable[entry.lastNumberIndex].nextIndex := nextNumberIndex;
149             END;
150 
151             {Set the line number at the end of the list.}
152             numberTable[nextNumberIndex].number    := lineNumber;
153             numberTable[nextNumberIndex].nextIndex := 0;
154             entry.lastNumberIndex := nextNumberIndex;
155             nextNumberIndex       := nextNumberIndex + 1;
156         END
157         ELSE BEGIN
158             numberTableFull := true;
159         END;
160     END;
161 
162 
163 PROCEDURE EnterWord;
164 
165     {Enter the current word into the word table. Each word is first
166      read into the end of the table.}
167 
168     VAR
169 	i : wordIndexRange;
170 
171     BEGIN
172         {By the time we process a word at the end of an input line,
173          lineNumber has already been incremented, so temporarily
174          decrement it.}
175         IF newLine THEN lineNumber := lineNumber - 1;
176 
177         {Search to see if the word had already been entered previously.
178          Each time it's read in, it's placed at the end of the word table.}
179         i := 1;
180         WHILE wordTable[i].word <> wordTable[nextWordIndex].word DO BEGIN
181             i := i + 1;
182         END;
183 
184         {Entered previously:  Update the existing entry.}
185         IF i < nextWordIndex THEN BEGIN
186             AppendLineNumber(wordTable[i]);
187         END
188 
189         {New word:  Initialize the entry at the end of the table.}
190         ELSE IF nextWordIndex < WordTableSize THEN BEGIN
191             wordTable[i].lastNumberIndex := 0;
192             AppendLineNumber(wordTable[i]);
193             nextWordIndex := nextWordIndex + 1;
194         END
195 
196         {Oops.  Table overflow!}
197         ELSE wordTableFull := true;
198 
199         IF newLine THEN lineNumber := lineNumber + 1;
200     END;
201 
202 
203 PROCEDURE SortWords;
204 
205     {Sort the words alphabetically.}
206 
207     VAR
208 	i, j : wordIndexRange;
209 	temp : wordEntryType;
210 
211     BEGIN
212         FOR i := 1 TO nextWordIndex - 2 DO BEGIN
213             FOR j := i + 1 TO nextWordIndex - 1 DO BEGIN
214                 IF wordTable[i].word > wordTable[j].word THEN BEGIN
215                     temp := wordTable[i];
216                     wordTable[i] := wordTable[j];
217                     wordTable[j] := temp;
218                 END;
219             END;
220         END;
221     END;
222 
223 
224 PROCEDURE PrintNumbers(i : numberIndexRange);
225 
226     {Print a word's linked list of line numbers.}
227 
228     BEGIN
229         REPEAT
230             write(numberTable[i].number:4);
231             i := numberTable[i].nextIndex;
232         UNTIL i = 0;
233         writeln;
234     END;
235 
236 
237 PROCEDURE PrintXref;
238 
239     {Print the cross reference listing.}
240 
241     VAR
242 	i : wordIndexRange;
243 
244     BEGIN
245         writeln;
246         writeln;
247         writeln('Cross-reference');
248         writeln('---------------');
249         writeln;
250         SortWords;
251         FOR i := 1 TO nextWordIndex - 1 DO BEGIN
252             write(wordTable[i].word);
253             PrintNumbers(wordTable[i].firstNumberIndex);
254         END;
255     END;
256 
257 
258 BEGIN {Xref}
259     wordTableFull   := false;
260     numberTableFull := false;
261     nextWordIndex   := 1;
262     nextNumberIndex := 1;
263     lineNumber      := 1;
264     write('    1 : ');
265 
266     {First read the words.}
267     WHILE NOT (eof OR wordTableFull OR numberTableFull) DO BEGIN
268 
269         {Read each word into the last entry of the word table
270          and then enter it into its correct location.}
271         IF ReadWord(wordtable[nextwordIndex].Word) THEN BEGIN
272             EnterWord;
273         END;
274     END;
275 
276     {Then print the cross reference listing if all went well.}
277     IF wordTableFull THEN BEGIN
278         writeln;
279         writeln('*** The word table is not large enough. ***');
280     END
281     ELSE IF numberTableFull THEN BEGIN
282         writeln;
283         writeln('*** The number table is not large enough. ***');
284     END
285     ELSE BEGIN
286         PrintXref;
287     END;
288 
289     {Print final stats.}
290     writeln;
291     writeln((nextWordIndex - 1):5,   ' word entries.');
292     writeln((nextNumberIndex - 1):5, ' line number entries.');
293 END {Xref}.

                 293 source lines.
                   0 syntax errors.
                0.16 seconds total parsing time.
    1 : 
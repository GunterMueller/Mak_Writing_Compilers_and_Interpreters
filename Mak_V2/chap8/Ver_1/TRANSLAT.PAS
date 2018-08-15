PROGRAM NumberTranslator (input, output);

{   Translate a list of integers from numeric form into
    words.  The integers must not be negative nor be
    greater than the value of maxnumber.  The last
    integer in the list has the value of terminator.
}

CONST
    maxnumber  = 30000;	{maximum allowable number}
    terminator = 0;     {last number in list}

VAR
    number : integer; 	{number to be translated}


PROCEDURE Translate (n : integer);

    {Translate number n into words.}

    VAR
	partbefore,    	{part before the comma}
	partafter     	{part after the comma}
	 : integer;


    PROCEDURE DoPart (part : integer);

	{Translate a part of a number into words,
	 where 1 <= part <= 999.}

	VAR
	    hundredsdigit,	{hundreds digit 0..9}
	    tenspart,        	{tens part 0..99}
	    tensdigit,       	{tens digit 0..9}
	    onesdigit        	{ones digit 0..9}
		: integer;


	PROCEDURE DoOnes (digit : integer);

	    {Translate a single ones digit into a word,
	     where 1 <= digit <= 9.}

	    BEGIN
		CASE digit OF
		    1:  write (' one');
		    2:  write (' two');
		    3:  write (' three');
		    4:  write (' four');
		    5:  write (' five');
		    6:  write (' six');
		    7:  write (' seven');
		    8:  write (' eight');
		    9:  write (' nine');
		END;
	    END {DoOnes};


	PROCEDURE DoTeens (teens : integer);

	    {Translate the teens into a word,
	     where 10 <= teens <= 19.}

	    BEGIN
		CASE teens OF
		    10:  write (' ten');
		    11:  write (' eleven');
		    12:  write (' twelve');
		    13:  write (' thirteen');
		    14:  write (' fourteen');
		    15:  write (' fifteen');
		    16:  write (' sixteen');
		    17:  write (' seventeen');
		    18:  write (' eighteen');
		    19:  write (' nineteen');
		END;
	    END {DoTeens};


	PROCEDURE DoTens (digit : integer);

	    {Translate a single tens digit into a word,
	     where 2 <= digit <= 9.}

	    BEGIN
		CASE digit OF
		    2:  write (' twenty');
		    3:  write (' thirty');
		    4:  write (' forty');
		    5:  write (' fifty');
		    6:  write (' sixty');
		    7:  write (' seventy');
		    8:  write (' eighty');
		    9:  write (' ninety');
		END;
	    END {DoTens};


	BEGIN {DoPart}

	    {Break up the number part.}
	    hundredsdigit := part DIV 100;
	    tenspart      := part MOD 100;

	    {Translate the hundreds digit.}
	    IF hundredsdigit > 0 THEN BEGIN
		DoOnes (hundredsdigit);
		write (' hundred');
	    END;

	    {Translate the tens part.}
	    IF  (tenspart >= 10)
	    AND (tenspart <= 19) THEN BEGIN
		DoTeens (tenspart);
	    END
	    ELSE BEGIN
		tensdigit := tenspart DIV 10;
		onesdigit := tenspart MOD 10;

		IF tensdigit > 0 THEN DoTens (tensdigit);
		IF onesdigit > 0 THEN DoOnes (onesdigit);
	    END;
	END {DoPart};


    BEGIN {Translate}

	{Break up the number.}
	partbefore := n DIV 1000;
	partafter  := n MOD 1000;

	IF partbefore > 0 THEN BEGIN
	    DoPart (partbefore);
	    write (' thousand');
	END;
	IF partafter > 0 THEN DoPart (partafter);
    END {Translate};


BEGIN {NumberTranslator}

    {Loop to read, write, check, and translate the numbers.}
    REPEAT
	read (number);
	write (number:6, ' :');

	IF number < 0 THEN BEGIN
	    write (' ***** Error -- number < 0');
	END
	ELSE IF number > maxnumber THEN BEGIN
	    write (' ***** Error -- number > ', maxnumber:1);
	END
	ELSE IF number = 0 THEN BEGIN
	    write (' zero');
	END
	ELSE BEGIN
	    Translate (number);
	END;

	writeln;  {complete output line}
    UNTIL number = terminator;
END {NumberTranslator}.

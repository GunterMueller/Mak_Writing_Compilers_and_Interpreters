PROGRAM excase (output);

CONST
    six = 6;

VAR
    i  : integer;
    ch : char;

BEGIN
    FOR i := 1 TO 9 DO BEGIN
	write(i, ' : ');
	CASE i OF
	    1:      writeln('one');
	    2:      writeln('two');
	    3:      writeln('three');
	    5,7,4:  writeln('four, five, or seven');
	    six:    writeln('six');
	    8:      writeln('eight');
	    9:      writeln('nine');
	END;
    END;
END.

PROGRAM copy (input, output);

VAR
    ch : char;

BEGIN
    WHILE NOT eof DO BEGIN
	WHILE NOT eoln DO BEGIN
	    read(ch);
	    write(ch);
	END;
	readln;
	writeln;
    END;
END.
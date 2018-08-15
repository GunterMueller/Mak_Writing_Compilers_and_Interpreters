PROGRAM fortest (output);

VAR
    i  : integer;
    ch, ch1, ch2, ch3 : char;

BEGIN
    FOR i := 1 TO 5 DO writeln('i = ', i:0);
    writeln('------');
    writeln('i = ', i:0);
    writeln('------');

    FOR i := 5 DOWNTO 1 DO writeln('i = ', i:0);
    writeln('------');
    writeln('i = ', i:0);
    writeln('------');

    FOR ch := 'a' TO 'e' DO writeln('ch = ', ch:0);
    writeln('------');
    writeln('ch = ', ch:0);
    writeln('------');

    FOR ch := 'E' DOWNTO 'A' DO writeln('ch = ', ch:0);
    writeln('------');
    writeln('ch = ', ch:0);
    writeln('------');

    FOR ch1 := 'a' TO 'c' DO BEGIN
	FOR ch2 := 'a' TO 'c' DO BEGIN
	    FOR ch3 := 'a' TO 'c' DO BEGIN
		writeln(ch1:0, ch2:0, ch3:0);
	    END;
	END;
    END;
END.

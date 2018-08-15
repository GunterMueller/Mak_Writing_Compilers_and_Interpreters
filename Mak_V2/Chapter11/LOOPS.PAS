PROGRAM loops (output);

VAR
    i : integer;

BEGIN
    i := 1;
    REPEAT
	i := i + 1;
	writeln(i);
    UNTIL i = 3;

    i := 1;
    WHILE i < 4 DO BEGIN
	i := i + 1;
	writeln(i);
    END;

    FOR i := 1 TO 3 DO BEGIN
	writeln(i);
    END;

    FOR i := 3 DOWNTO 1 DO BEGIN
	writeln(i);
    END;
END.

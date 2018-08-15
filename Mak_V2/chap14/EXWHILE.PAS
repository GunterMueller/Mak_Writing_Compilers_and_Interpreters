PROGRAM exwhile (output);

VAR
    i : integer;

BEGIN
    i := 1;
    WHILE i <= 10 DO BEGIN
	writeln(i);
	i := i + 1;
    END;
END.
